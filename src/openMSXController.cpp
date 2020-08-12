#include "openMSXController.h"
#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "MiscControlPage.h"
#include "AudioControlPage.h"
#include "SessionPage.h"
#include "wxCatapultApp.h"
#include "ScreenShotDlg.h"
#include "PipeReadThread.h"
#include "utils.h"
#include <cassert>
#include <cerrno>
#include <cstring> // for strerror
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/wxprec.h>
#ifdef __WXMSW__
#include <config.h>
#include "PipeConnectThread.h"
#include <process.h>
#else
#include <unistd.h>
#include <wx/process.h>
#endif

static const int S_CONVERT = 1;
static const int S_EVENT   = 2;
static const int S_INVERT  = 4;

openMSXController::openMSXController(wxWindow* target)
{
#ifdef __WXMSW__
	m_launchCounter = 0;
	m_namedPipeHandle = INVALID_HANDLE_VALUE;
#else
	m_openMSXstdin = m_openMSXstdout = m_openMSXstderr = -1;
#endif

	m_openMSXID = 0;
	m_appWindow = (wxCatapultFrame*)target;
	m_openMsxRunning = false;
	m_isRelaunching = false;
	InitLaunchScript();
}

openMSXController::~openMSXController()
{
	if (m_openMsxRunning) {
		WriteCommand(wxT("exit"));
#ifndef __WXMSW__
		m_stdOutThread->Wait();
		m_stdErrThread->Wait();
#endif
	}
}

bool openMSXController::HandleMessage(wxCommandEvent& event)
{
	switch (event.GetId()) {
#ifdef __WXMSW__
	case MSGID_PIPECREATED:
		HandlePipeCreated();
		break;
#endif
	case MSGID_STDOUT:
		HandleStdOut(event);
		break;
	case MSGID_STDERR:
		HandleStdErr(event);
		break;
	case MSGID_PARSED:
		HandleParsedOutput(event);
		break;
	case MSGID_ENDPROCESS:
		HandleEndProcess(event);
		break;
	default:
		return false; // invalid ID
	}
	return true;
}

void openMSXController::PostLaunch()
{
	const char* initial = "<openmsx-control>\n";
	WriteMessage((const xmlChar*)initial, strlen(initial));
	ExecuteStart();
}

void openMSXController::HandleEndProcess(wxCommandEvent& event)
{
	if (!m_openMsxRunning) return;
	m_openMsxRunning = false;

	m_parser.reset();
	m_commands.clear();
	m_launchCallback.clear();
	m_appWindow->OpenMSXStopped();

#ifndef __WXMSW__
	close(m_openMSXstdin);
#endif
}

void openMSXController::HandleStdOut(wxCommandEvent& event)
{
	std::unique_ptr<wxString> data((wxString*)event.GetClientData());
	m_parser->ParseXmlInput(*data, m_openMSXID);
}

void openMSXController::HandleStdErr(wxCommandEvent& event)
{
	std::unique_ptr<wxString> data((wxString*)event.GetClientData());
	if (*data == wxT("\n")) return;

	for (unsigned i = 0; i < m_appWindow->m_tabControl->GetPageCount(); ++i) {
		if (m_appWindow->m_tabControl->GetPageText(i) == wxT("Status Info")) {
			m_appWindow->m_tabControl->SetSelection(i);
		}
	}
	m_appWindow->m_statusPage->Add(wxColour(255, 23, 23), *data);
}

void openMSXController::HandleParsedOutput(wxCommandEvent& event)
{
	std::unique_ptr<CatapultXMLParser::ParseResult> data(
		(CatapultXMLParser::ParseResult*)event.GetClientData());
	if (!m_openMsxRunning || (data->openMSXID != m_openMSXID)) {
		return; // another instance is already started, ignore this message
	}
	switch (data->parseState) {
	case CatapultXMLParser::TAG_UPDATE: {
		wxArrayString lastCmd;
		if (!m_commands.empty()) {
		        lastCmd = utils::parseTclList(m_commands.front().command);
		}
		lastCmd.Add(wxT(""), 2); // make sure there are at least 2 elements
		if (data->updateType == CatapultXMLParser::UPDATE_LED) {
			m_appWindow->UpdateLed (data->name, data->contents);
		} else if (data->updateType == CatapultXMLParser::UPDATE_STATE) {
			if (data->name == wxT("cassetteplayer")) {
				m_appWindow->m_sessionPage->SetCassetteMode(data->contents);
			} else {
				m_appWindow->UpdateState(data->name, data->contents);
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_SETTING) {
			if ((lastCmd[0] != wxT("set")) ||
			    (lastCmd[1] != data->name)) {
				UpdateSetting2(data->name, data->contents);
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_PLUG) {
			if ((lastCmd[0] != wxT("plug")) ||
			    (lastCmd[1] != data->name)) {
				UpdateSetting2(data->name, data->contents);
				ExecuteStart(m_relaunch); // reinit stuff now
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_HARDWARE) {
			ExecuteStart(m_relaunch); // reinit stuff now
		} else if (data->updateType == CatapultXMLParser::UPDATE_EXTENSION) {
			ExecuteStart(m_relaunch); // reinit stuff now
		} else if (data->updateType == CatapultXMLParser::UPDATE_CONNECTOR) {
			ExecuteStart(m_relaunch); // reinit stuff now
		} else if (data->updateType == CatapultXMLParser::UPDATE_MEDIA) {
			if ((lastCmd[0] != data->name) ||
			    (lastCmd[1] != data->contents)) {
				UpdateSetting2(data->name, data->contents);
			}
		}
		break;
	}
	case CatapultXMLParser::TAG_REPLY:
		switch (data->replyState) {
		case CatapultXMLParser::REPLY_OK: {
			assert(!m_commands.empty());
			auto cmd = m_commands.front();
			m_commands.pop_front();
			if (cmd.okCallback) {
				cmd.okCallback(cmd.command, data->contents);
			}
			break;
		}
		case CatapultXMLParser::REPLY_NOK: {
			assert(!m_commands.empty());
			auto cmd = m_commands.front();
			m_commands.pop_front();
			if (cmd.errorCallback) {
				cmd.errorCallback(cmd.command, data->contents);
			} else {
				commandError(cmd.command, data->contents);
			}
			break;
		}
		case CatapultXMLParser::REPLY_UNKNOWN:
			m_appWindow->m_statusPage->Add(wxColour(174, 0, 0),
				wxT("Warning: Unknown reply received!\n"));
			break;
		}
		break;
	case CatapultXMLParser::TAG_LOG: {
		wxColour col = (data->logLevel == CatapultXMLParser::LOG_WARNING)
		             ? wxColour(174, 0, 0) // warning
			     : wxColour(0, 0, 0);  // info (or unknown)
		m_appWindow->m_statusPage->Add(col, data->contents + wxT("\n"));
		if (data->contents.Left(15) == wxT("Screen saved to")) {
			int inhibit;
			ConfigurationData::instance().GetParameter(ConfigurationData::CD_SCREENSHOTINFO, &inhibit);
			if (inhibit == 0) {
				ScreenshotDlg dlg(m_appWindow);
				dlg.ShowModal();
			}
		}
		break;
	}
	default:
		break;
	}
}

void openMSXController::WriteCommand(
	const wxString& command,
	std::function<void (const wxString&, const wxString&)> okCallback,
	std::function<void (const wxString&, const wxString&)> errorCallback)
{
	if (!m_openMsxRunning) return;

	xmlChar* buffer = xmlEncodeEntitiesReentrant(nullptr, (const xmlChar*)(const char*)(command.mb_str(wxConvUTF8)));
	if (!buffer) return;

	CommandEntry temp;
	temp.command       = command;
	temp.okCallback    = okCallback;
	temp.errorCallback = errorCallback;
	m_commands.push_back(temp);

	auto len = strlen(reinterpret_cast<const char*>(buffer));
	auto len2 = 9 + len + 11;
	VLA(char, cmd, len2);
	memcpy(cmd, "<command>", 9);
	memcpy(cmd + 9, buffer, len);
	memcpy(cmd + 9 + len, "</command>\n", 11);
	WriteMessage((xmlChar*)cmd, len2);

	xmlFree(buffer);
}

void openMSXController::commandError(const wxString& cmd, const wxString& result)
{
	wxString msg = wxT("Warning: NOK received on command: ");
	msg << cmd << wxT("\n");
	if (!result.IsEmpty()) {
		msg << wxT("error = ") << result << wxT("\n");
	}
	m_appWindow->m_statusPage->Add(wxColour(174, 0, 0), msg);
}

bool openMSXController::CheckVersion(const wxString& cmd)
{
	wxArrayString output;
	int code = wxExecute(cmd + wxT(" -v"), output);
	wxString version = ((code != -1) && !output.IsEmpty())
		? output[0] : wxString();

	long ver = -1;
	long majver = 0;
	long minver = 0;
	long subver = 0;
	if (version.StartsWith(wxT("openMSX "))) {
		int pos = version.Find('.');
		if (pos != wxNOT_FOUND) {
			version.Mid(8, pos - 8).ToLong(&majver);
			version = version.Mid(pos + 1);
			pos = version.Find('.');
			if (pos != wxNOT_FOUND) {
				version.Mid(0, pos).ToLong(&minver);
				version.Mid(pos + 1).ToLong(&subver);
			}
		}
		ver = (((majver * 100) + minver) * 100) + subver;
	}
	if (ver <= 0) {
		wxMessageBox(
			wxT("Unable to determine openMSX version!\nPlease upgrade to 0.10.0 or higher.\n(Or contact the authors.)"),
			wxT("Error"));
		return false;
	}
	if (ver < 100) {
		wxMessageBox(
			wxT("The openMSX version you are using is too old!\nPlease upgrade to 0.10.0 or higher."),
			wxT("Error"));
		return false;
	}
	return true;
}

void openMSXController::StartOpenMSX(const wxString& cmd)
{
	++m_openMSXID;
	Launch(cmd);
}

wxString openMSXController::GetConnectorClass(const wxString& name) const
{
	if (m_connectorclasses.IsEmpty()) return wxString();

	for (unsigned i = 0; i < m_connectors.GetCount(); ++i) {
		if (m_connectors[i] == name) {
			return m_connectorclasses[i];
		}
	}
	assert(false); return wxString();
}

wxString openMSXController::GetConnectorPlugged(const wxString& name) const
{
	if (m_connectorpluggeds.IsEmpty()) return wxString();

	for (unsigned i = 0; i < m_connectors.GetCount(); ++i) {
		if (m_connectors[i] == name) {
			return m_connectorpluggeds[i];
		}
	}
	assert(false); return wxString();
}

static bool isTclTrue(const wxString& str)
{
	return (str == wxT("on"))   ||
	       (str == wxT("true")) ||
	       (str == wxT("1"))    ||
	       (str == wxT("yes"));
}

void openMSXController::InitLaunchScript()
{
	AddCommand(wxT("openmsx_update enable setting"));
	AddCommand(wxT("openmsx_update enable led"));
	AddCommand(wxT("set power on"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("unset renderer"));
	AddCommand(wxT(""),
		[&](const wxString&, const wxString&) {
			m_appWindow->EnableMainWindow(); });
	AddCommand(wxT("lindex [openmsx_info setting renderer] 2"),
		[&](const wxString&, const wxString& r) {
			FillComboBox(wxT("RendererSelector"), r); });
	AddCommand(wxT("lindex [openmsx_info setting scale_algorithm] 2"),
		[&](const wxString&, const wxString& r) {
			FillComboBox(wxT("ScalerAlgoSelector"), r) ;});
	AddCommand(wxT("lindex [openmsx_info setting scale_factor] 2"),
		[&](const wxString&, const wxString& r) {
			FillRangeComboBox(wxT("ScalerFactorSelector"), r); });
	AddCommand(wxT("lindex [openmsx_info setting videosource] 2"),
		[&](const wxString&, const wxString& r) {
			FillComboBox(wxT("VideoSourceSelector"), r); });
	AddCommand(wxT("openmsx_update enable media"));
	AddCommand(wxT("info exist frontswitch"),
		[&](const wxString& c, const wxString& r) {
			EnableFirmware(c, r); });
	AddCommand(wxT("info exist firmwareswitch"),
		[&](const wxString& c, const wxString& r) {
			EnableFirmware(c, r); });
	AddCommand(wxT("set renderer"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set scale_algorithm"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set scale_factor"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set vsync"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set videosource"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set deinterlace"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set limitsprites"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set disablesprites"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set fullscreen"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set blur"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set glow"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set gamma"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set noise"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set scanline"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("info exist renshaturbo"),
		[&](const wxString&, const wxString& r) {
			if (isTclTrue(r)) {
				m_appWindow->m_miscControlPage->EnableRenShaTurbo();
			}
		});
	AddCommand(wxT("set renshaturbo"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set save_settings_on_exit"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set printerlogfilename"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT(""),
		[&](const wxString&, const wxString&) {
			m_appWindow->m_videoControlPage->SetSliderDefaults(); });
	AddCommand(wxT("set speed"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set fastforwardspeed"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set fastforward"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set fullspeedwhenloading"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set maxframeskip"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set cmdtiming"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("machine_info pluggable"),
		[&](const wxString&, const wxString& r) {
			HandlePluggables(r); });
	m_relaunch = m_launchScript.size(); // !!HACK!!
	AddCommand(wxT("machine_info connector"),
		[&](const wxString&, const wxString& r) {
			HandleConnectors(r); });
	AddCommand(wxT("set midi-in-readfilename"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set midi-out-logfilename"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("set audio-inputfilename"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT(""),
		[&](const wxString&, const wxString&) {
			m_appWindow->m_miscControlPage->InitConnectorPanel(); });
	AddCommand(wxT(""),
		[&](const wxString&, const wxString&) {
			m_appWindow->m_audioControlPage->InitAudioIO(); });
	AddCommand(wxT("set catapult_result [dict create] ; foreach dev [machine_info sounddevice] { dict append catapult_result $dev [machine_info sounddevice $dev]} ; set catapult_result"),
		[&](const wxString&, const wxString& r) {
			HandleSoundDevices(r); });
	AddCommand(wxT("set mute"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	AddCommand(wxT("catch {cassetteplayer}"),
		[&](const wxString&, const wxString& r) {
			m_appWindow->m_sessionPage->EnableCassettePort(r); });
	AddCommand(wxT("cassetteplayer"),
		[&](const wxString&, const wxString& r) {
			wxArrayString info = utils::parseTclList(r);
			m_appWindow->m_sessionPage->SetCassetteMode(info.Last());
		});
	AddCommand(wxT("openmsx_update enable plug"));
	AddCommand(wxT("openmsx_update enable connector"));
	AddCommand(wxT("openmsx_update enable hardware"));
	AddCommand(wxT("openmsx_update enable extension"));
	AddCommand(wxT("openmsx_update enable status"));
	AddCommand(wxT(""), // finish launch
		[&](const wxString&, const wxString&) {
			m_appWindow->OpenMSXStarted();
			m_isRelaunching = false;
		});

	AddSetting(wxT("renderer"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("RendererSelector")); });
	AddSetting(wxT("scale_algorithm"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("ScalerAlgoSelector")); });
	AddSetting(wxT("scale_factor"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("ScalerFactorSelector")); });
	AddSetting(wxT("vsync"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("VSyncButton"), S_CONVERT); });
	AddSetting(wxT("videosource"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("VideoSourceSelector")); });
	AddSetting(wxT("deinterlace"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("DeInterlaceButton"), S_CONVERT); });
	AddSetting(wxT("limitsprites"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("LimitSpriteButton"), S_CONVERT); });
	AddSetting(wxT("disablesprites"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("DisableSpritesButton"), S_CONVERT); });
	AddSetting(wxT("blur"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("BlurIndicator")); });
	AddSetting(wxT("glow"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("GlowIndicator")); });
	AddSetting(wxT("gamma"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("GammaIndicator")); });
	AddSetting(wxT("noise"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("NoiseIndicator")); });
	AddSetting(wxT("scanline"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("ScanlineIndicator")); });
	AddSetting(wxT("speed"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("SpeedIndicator")); });
	AddSetting(wxT("fastforwardspeed"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("FastForwardSpeedIndicator")); });
	AddSetting(wxT("maxframeskip"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("MaxFrameSkipIndicator")); });
	AddSetting(wxT("fastforward"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("FastForwardButton"), S_EVENT); });
	AddSetting(wxT("fullspeedwhenloading"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("FastLoadingButton"), S_EVENT); });
	AddSetting(wxT("power"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("PowerButton")); });
	AddSetting(wxT("pause"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("PauseButton")); });
	AddSetting(wxT("frontswitch"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("FirmwareButton")); });
	AddSetting(wxT("firmwareswitch"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("FirmwareButton")); });
	AddSetting(wxT("mute"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("MuteButton")); });
	AddSetting(wxT("midi-in-readfilename"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("MidiInFileInput")); });
	AddSetting(wxT("midi-out-logfilename"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("MidiOutFileInput")); });
	AddSetting(wxT("audio-inputfilename"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("SampleFileInput")); });
	AddSetting(wxT("*_volume"),
		[&](const wxString& n, const wxString& v) {
			wxString channel = n.Mid(0, n.Length() - 7); // remove "_volume"
			m_appWindow->m_audioControlPage->SetChannelVolume(channel, v);
		});
	AddSetting(wxT("*_balance"),
		[&](const wxString& n, const wxString& v) {
			wxString channel = n.Mid(0, n.Length() - 8); // remove "_balance"
			m_appWindow->m_audioControlPage->SetChannelMode(channel, v);
		});
	AddSetting(wxT("*-in"),
		[&](const wxString& n, const wxString& v) {
			m_appWindow->m_audioControlPage->UpdateMidiPlug(
				wxT("MidiInSelector"), v); });
	AddSetting(wxT("*-out"),
		[&](const wxString& n, const wxString& v) {
			m_appWindow->m_audioControlPage->UpdateMidiPlug(
				wxT("MidiOutSelector"), v); });
	AddSetting(wxT("pcminput"),
		[&](const wxString&, const wxString& v) {
			UpdatePluggable(v, wxT("SampleInSelector")); });
	AddSetting(wxT("joyporta"),
		[&](const wxString&, const wxString& v) {
			UpdatePluggable(v, wxT("Joyport1Selector")); });
	AddSetting(wxT("joyportb"),
		[&](const wxString&, const wxString& v) {
			UpdatePluggable(v, wxT("Joyport2Selector")); });
	AddSetting(wxT("printerport"),
		[&](const wxString&, const wxString& v) {
			UpdatePluggable(v, wxT("PrinterportSelector")); });
	AddSetting(wxT("renshaturbo"),
		[&](const wxString&, const wxString& v) {
			UpdateSlider(v, wxT("RenshaTurboSlider")); });
	AddSetting(wxT("diska"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("DiskAContents")); });
	AddSetting(wxT("diskb"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("DiskBContents")); });
	AddSetting(wxT("carta"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("CartAContents")); });
	AddSetting(wxT("cartb"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("CartBContents")); });
	AddSetting(wxT("cassetteplayer"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("CassetteContents")); });
	AddSetting(wxT("hda"),
		[&](const wxString&, const wxString& v) {
			UpdateCombo(v, wxT("HardDiskContents")); });
	AddSetting(wxT("fullscreen"),
		[&](const wxString&, const wxString& v) {
			UpdateToggle(v, wxT("FullScreenButton"), S_CONVERT); });
	AddSetting(wxT("save_settings_on_exit"),
		[&](const wxString&, const wxString& v) {
			int menusetting = m_appWindow->GetMenuBar()->FindMenuItem(
				wxT("&Settings"), wxT("Save openMSX Settings On &Exit"));
			assert(menusetting != wxNOT_FOUND);
			m_appWindow->GetMenuBar()->Check(menusetting, isTclTrue(v));
		});
	AddSetting(wxT("printerlogfilename"),
		[&](const wxString&, const wxString& v) {
			UpdateIndicator(v, wxT("PrinterLogFile"), S_CONVERT); });
}

void openMSXController::AddCommand(
	const wxString& cmd,
	std::function<void (const wxString&, const wxString&)> callback)
{
	LaunchInstruction instr;
	instr.command  = cmd;
	instr.callback = callback;
	m_launchScript.push_back(instr);
}

void openMSXController::AddSetting(
	const wxString& setting,
	std::function<void (const wxString&, const wxString&)> callback)
{
	SettingElement elem;
	elem.setting  = setting;
	elem.callback = callback;
	m_settingTable.push_back(elem);
}

void openMSXController::ExecuteStart(int startLine)
{
	if (m_isRelaunching && (startLine > 0)) {
		// Canceling relaunch, already relaunching
		return;
	}
	m_isRelaunching = (startLine > 0);
	sendStep = startLine;
	ExecuteNext();
}

void openMSXController::ExecuteNext()
{
	WriteCommand(m_launchScript[sendStep].command,
		[&](const wxString& c, const wxString& r) {
			HandleLaunch(c, r, true); },
		[&](const wxString& c, const wxString& r) {
			HandleLaunch(c, r, false); });
	m_launchCallback.push_back(m_launchScript[sendStep].callback);
}

void openMSXController::HandleLaunch(const wxString& command, const wxString& result, bool ok)
{
	// Handle received command
	assert(!m_launchCallback.empty());
	auto callback = m_launchCallback.front();
	m_launchCallback.pop_front();
	if (ok && callback) {
		callback(command, result);
	}

	// Execute next command
	++sendStep;
	if (sendStep < int(m_launchScript.size())) {
		ExecuteNext();
	}
}

void openMSXController::UpdateSetting(const wxString& cmd, const wxString& data)
{
	auto tokens = utils::parseTclList(cmd);
	assert(tokens.GetCount() >= 2);
	UpdateSetting2(tokens[1], data);
}

void openMSXController::UpdateSetting2(const wxString& name_, const wxString& data)
{
	wxString name = name_; // TODO HACK: need a proper Tcl parser
	name.Replace(wxT("\\"), wxT(""));

	for (auto& elem : m_settingTable) {
		if (name.Matches(elem.setting.c_str())) {
			elem.callback(name, data);
			break;
		}
	}
}

void openMSXController::UpdateToggle(const wxString& data, const wxString& control, int flags)
{
	if (auto* button = (wxToggleButton*)wxWindow::FindWindowByName(control)) {
		bool active = !(flags & S_INVERT);
		bool sendvalue = isTclTrue(data) ? active : !active;
		button->SetValue(sendvalue);

		if (flags & S_EVENT) {
			wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, button->GetId());
			event.SetInt(active);
			event.SetEventObject(button);
			button->GetEventHandler()->ProcessEvent(event);
		}

		if (flags & S_CONVERT) {
			button->SetLabel(isTclTrue(data) ? wxT("On") : wxT("Off"));
		}
	}
}

void openMSXController::UpdateCombo(const wxString& data, const wxString& control, int flags)
{
	wxString valuetext = data;
	if (flags & S_CONVERT) {
		valuetext = data.Mid(0, 1).Upper() + data.Mid(1).Lower();
	}
	if (auto* box = (wxComboBox*)wxWindow::FindWindowByName(control)) {
		if (box->GetWindowStyle() & wxCB_READONLY) {
			box->SetSelection(box->FindString(valuetext));
		} else {
			box->SetValue(valuetext);
		}
	}
}

void openMSXController::UpdateIndicator(const wxString& data, const wxString& control, int flags)
{
	if (auto* indicator = (wxTextCtrl*)wxWindow::FindWindowByName(control)) {
		wxString tempData = data;
		if (flags & S_CONVERT) {
			tempData = utils::ConvertPathBack(data);
		}
		if (indicator->GetValue() != tempData) {
			indicator->SetValue(tempData);
		}
	}
}

void openMSXController::UpdateSlider(const wxString& data, const wxString& control)
{
	if (auto* slider = (wxSlider*)wxWindow::FindWindowByName(control)) {
		long value;
		data.ToLong(&value, 10);
		if (slider->GetValue() != value) {
			slider->SetValue(value);
		}
	}
}

void openMSXController::UpdatePluggable(const wxString& data, const wxString& control)
{
	if (auto* box = (wxComboBox*)wxWindow::FindWindowByName(control)) {
		box->SetValue(data.IsEmpty() ? wxT("--empty--") : data);
	}
}

void openMSXController::FillComboBox(const wxString& control, const wxString& result)
{
	auto* box = (wxComboBox*)wxWindow::FindWindowByName(control);
	box->Clear();
	for (auto& item : utils::parseTclList(result)) {
		box->Append(item);
	}
}

void openMSXController::FillRangeComboBox(const wxString& control, const wxString& result)
{
	long min;
	long max;
	wxString range;
	int pos = result.Find(' ');
	if (pos != wxNOT_FOUND) {
		result.Left(pos).ToLong(&min);
		result.Mid(pos + 1).ToLong(&max);
		for (long index = min; index <= max; ++index) {
			range << index << wxT(" ");
		}
	}
	FillComboBox(control, range);
}

void openMSXController::EnableFirmware(const wxString& cmd, const wxString& result)
{
	if (isTclTrue(result)) {
		int pos = cmd.Find(' ', true); // last space
		assert(pos != wxNOT_FOUND);
		wxString name = cmd.Mid(pos + 1); // firmwareswitch or frontswitch
		m_appWindow->m_miscControlPage->EnableFirmware(name);
	}
}

void openMSXController::UpdateMixer()
{
	ExecuteStart(m_relaunch);
}

void openMSXController::HandlePluggables(const wxString& result)
{
	m_pluggables = utils::parseTclList(result);
	m_pluggabledescriptions.Clear();
	m_pluggableclasses.Clear();
	for (auto& p : m_pluggables) {
		wxString pluggable = utils::tclEscapeWord(p);
		WriteCommand(wxT("machine_info pluggable ") + pluggable,
			[&](const wxString&, const wxString& r) {
				m_pluggabledescriptions.Add(r); });
		WriteCommand(wxT("machine_info connectionclass ") + pluggable,
			[&](const wxString&, const wxString& r) {
				m_pluggableclasses.Add(r); });
	}
}

void openMSXController::HandleConnectors(const wxString& result)
{
	m_connectors = utils::parseTclList(result);
	m_connectorclasses.Clear();
	m_connectorpluggeds.Clear();
	for (auto& con : m_connectors) {
		WriteCommand(wxT("machine_info connectionclass ") +
		             utils::tclEscapeWord(con),
			[&](const wxString&, const wxString& r) {
				m_connectorclasses.Add(r); });
		WriteCommand(wxT("plug ") + utils::tclEscapeWord(con),
			[&](const wxString&, const wxString& r) {
				int pos = r.Find(wxT(": "));
				wxString pluggable = r.Right(r.Length() - pos - 2); // remove "connector: "
				m_connectorpluggeds.Add(pluggable); });
	}
}

void openMSXController::HandleSoundDevices(const wxString& result)
{
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_appWindow->m_audioControlPage->InitAudioChannels();
	auto devInfos = utils::parseTclList(result);
	assert((devInfos.size() % 2) == 0);
	for (auto it = devInfos.begin(); it != devInfos.end(); it += 2) {
		m_appWindow->m_audioControlPage->AddChannelType(*it, *(it + 1));
	}
	m_appWindow->m_audioControlPage->SetupAudioMixer();

	WriteCommand(wxT("set master_volume"),
		[&](const wxString& c, const wxString& r) {
			UpdateSetting(c, r); });
	for (auto it = devInfos.begin(); it != devInfos.end(); it += 2) {
		auto dev = utils::tclEscapeWord(*it);
		WriteCommand(wxT("set ") + dev + wxT("_volume"),
			[&](const wxString& c, const wxString& r) {
				UpdateSetting(c, r); });
		WriteCommand(wxT("set ") + dev + wxT("_balance"),
			[&](const wxString& c, const wxString& r) {
				UpdateSetting(c, r); });
	}
}

void openMSXController::RaiseOpenMSX()
{
#ifdef __WXMSW__
	HWND openmsxWindow = FindOpenMSXWindow();
	if (openmsxWindow != nullptr) {
		SetParent(openmsxWindow, m_catapultWindow);
		SetActiveWindow(openmsxWindow);
		SetForegroundWindow(openmsxWindow);
		SetParent(openmsxWindow, nullptr);
	}
#else
	// nothing
#endif
}

void openMSXController::RestoreOpenMSX()
{
#ifdef __WXMSW__
	HWND openmsxWindow = FindOpenMSXWindow();
	if (openmsxWindow != nullptr) {
		SetParent(openmsxWindow, m_catapultWindow);
		SetWindowPos(openmsxWindow, HWND_TOP, 0, 0, 640, 480, SWP_NOSIZE || SWP_SHOWWINDOW);
		SetParent(openmsxWindow, nullptr);
	}
#else
	// nothing
#endif
}

void openMSXController::WriteMessage(const xmlChar* msg, size_t length)
{
	if (!m_openMsxRunning) return;
	wxString errMsg;
#ifdef __WXMSW__
	unsigned long BytesWritten;
	::WriteFile(m_outputHandle, msg, length, &BytesWritten, nullptr);
	// TODO print more specific error message?
	if (length != size_t(BytesWritten)) {
		errMsg = wxT("incomplete buffer written");
	}
#else
	ssize_t r = write(m_openMSXstdin, msg, length);
	if (r == ssize_t(-1)) {
		const char* msg = strerror(errno);
		errMsg = msg ? wxString(msg, wxConvUTF8) : wxT("Unknown error");
	}
	if (length != size_t(r)) {
		errMsg = wxT("incomplete buffer written");
	}
#endif
	if (!errMsg.IsEmpty()) {
		m_appWindow->m_statusPage->Add(wxColour(174, 0, 0),
			wxT("Error writing to openMSX process: ") + errMsg + wxT("\n"));
	}
}

bool openMSXController::Launch(wxString cmdline)
{
	m_parser.reset(new CatapultXMLParser(m_appWindow));
#ifdef __WXMSW__
	m_catapultWindow = GetActiveWindow();
	cmdline += CreateControlParameter();
	HANDLE hInputRead, hOutputWrite, hErrorWrite, hErrorRead, hOutputRead;
	CreatePipes(&hInputRead, &hOutputWrite, &hErrorWrite, &hOutputRead, &hErrorRead);

	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

	STARTUPINFO si;
	DWORD dwFlags = STARTF_USESTDHANDLES;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = dwFlags;
	si.hStdInput = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError  = hErrorWrite;

	// Note: m_pipeName is set by CreateControlParameter().
	m_namedPipeHandle = CreateNamedPipe(m_pipeName, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 10000, 0, 100, nullptr);
	if (m_namedPipeHandle == INVALID_HANDLE_VALUE) {
		wxMessageBox(wxString::Format(
			wxT("Error creating pipe: %ld"), long(GetLastError())));
		return false;
	}

	LPTSTR szCmdLine = _tcsdup(cmdline.c_str());
	if (szCmdLine) {
		CreateProcess(
			nullptr, szCmdLine, nullptr, nullptr, true,
			dwProcessFlags, nullptr, nullptr, &si,
			&m_openmsxProcInfo); //testing suspended
		free(szCmdLine);
	}

	auto* thread = new PipeReadThread(m_appWindow, MSGID_STDOUT);
	if (thread->Create() == wxTHREAD_NO_ERROR) {
		thread->SetHandle(hOutputRead);
		thread->Run();
	}
	auto* thread2 = new PipeReadThread (m_appWindow, MSGID_STDERR);
	if (thread2->Create() == wxTHREAD_NO_ERROR) {
		thread2->SetHandle(hErrorRead);
		thread2->Run();
	}

	::ResumeThread(m_openmsxProcInfo.hThread);

	PipeConnectThread *m_connectThread = new PipeConnectThread(m_appWindow);
	m_connectThread->Create();
	m_connectThread->SetHandle(m_namedPipeHandle);
	m_connectThread->Run();
	m_outputHandle = m_namedPipeHandle;
	m_openMsxRunning = true;
	CloseHandles(m_openmsxProcInfo.hThread, hInputRead, hOutputWrite, hErrorWrite);

	return true;
#else
	cmdline += wxT(" -control stdio");
	if (!execute((const char*)(cmdline.mb_str(wxConvUTF8)),
	             m_openMSXstdin, m_openMSXstdout, m_openMSXstderr)) {
		return false;
	}
	m_stdOutThread.reset(new PipeReadThread(m_appWindow, MSGID_STDOUT, wxTHREAD_JOINABLE));
	if (m_stdOutThread->Create() == wxTHREAD_NO_ERROR) {
		m_stdOutThread->SetFileDescriptor(m_openMSXstdout);
		m_stdOutThread->Run();
	}
	m_stdErrThread.reset(new PipeReadThread(m_appWindow, MSGID_STDERR, wxTHREAD_JOINABLE));
	if (m_stdErrThread->Create() == wxTHREAD_NO_ERROR) {
		m_stdErrThread->SetFileDescriptor(m_openMSXstderr);
		m_stdErrThread->Run();
	}

	m_openMsxRunning = true;
	PostLaunch();
	return true;
#endif
}

// windows or linux specific stuff
#ifdef __WXMSW__

wxString openMSXController::CreateControlParameter()
{
	wxString parameter = wxT(" -control");

	m_launchCounter++;
	m_pipeName = wxString::Format(
		wxT("\\\\.\\pipe\\Catapult-%u-%lu"), _getpid(), m_launchCounter);
	parameter += wxT(" pipe:") + m_pipeName.Mid(9);
	return parameter;
}

bool openMSXController::CreatePipes(
	HANDLE* input, HANDLE* output, HANDLE* error,
	HANDLE* outputWrite, HANDLE* errorWrite)
{
	HANDLE hOutputReadTmp, hOutputWrite;
	HANDLE hErrorReadTmp, hErrorWrite;
	HANDLE hInputRead = 0;
	HANDLE hInputHandle = GetStdHandle(STD_INPUT_HANDLE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0)) {
		ShowError(wxT("Error creating pipe for stdout"));
		return false;
	}
	if (!CreatePipe(&hErrorReadTmp, &hErrorWrite, &sa, 0)) {
		ShowError(wxT("Error creating pipe for stderr"));
		return false;
	}
	*input = hInputHandle;
	*output = hOutputWrite;
	*error  = hErrorWrite;
	*outputWrite = hOutputReadTmp;
	*errorWrite = hErrorReadTmp;
	return true;
}

void openMSXController::ShowError(const wxString& msg)
{
	wxMessageBox(msg + wxString::Format(wxT(": error %ld"), long(GetLastError())));
}

void openMSXController::CloseHandles(
	HANDLE hThread, HANDLE hInputRead,
	HANDLE hOutputWrite, HANDLE hErrorWrite)
{
	if (!CloseHandle(hThread)) {
		wxMessageBox(wxT("Unable to close thread handle"));
		return;
	}
	if (!CloseHandle(hOutputWrite)) {
		wxMessageBox(wxT("Unable to close Output Write"));
		return;
	}
	if (!CloseHandle(hErrorWrite)) {
		wxMessageBox(wxT("Unable to close Error Write"));
		return;
	}
}

void openMSXController::HandlePipeCreated()
{
	PostLaunch();
}

HWND openMSXController::FindOpenMSXWindow()
{
	if (!m_openMsxRunning) return nullptr;

	FindOpenmsxInfo findInfo;
	findInfo.hWndFound = nullptr;
	findInfo.ProcessInfo = &m_openmsxProcInfo;

	WaitForInputIdle(m_openmsxProcInfo.hProcess, INFINITE);
	EnumWindows(EnumWindowCallBack, (LPARAM)&findInfo);
	return findInfo.hWndFound;
}

BOOL CALLBACK openMSXController::EnumWindowCallBack(HWND hwnd, LPARAM lParam)
{
	FindOpenmsxInfo* info = (FindOpenmsxInfo*)lParam;
	DWORD ProcessId;
	GetWindowThreadProcessId(hwnd, &ProcessId);
	TCHAR title[11];
	GetWindowText(hwnd, title, 10);
	if (ProcessId == info->ProcessInfo->dwProcessId && _tcslen(title) != 0) {
		info->hWndFound = hwnd;
		return false;
	} else {
		// Keep enumerating
		return true;
	}
}

#else

bool openMSXController::execute(const std::string& command, int& fdIn, int& fdOut, int& fdErr)
{
	// create pipes
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	int pipeStdin[2], pipeStdout[2], pipeStderr[2];
	if ((pipe(pipeStdin)  == -1) ||
	    (pipe(pipeStdout) == -1) ||
	    (pipe(pipeStderr) == -1)) {
		return false;
	}

	// create new thread
	int pid = fork();
	if (pid == -1) {
		return false;
	}
	if (pid == 0) {
		// child thread

		// redirect IO
		close(pipeStdin[PIPE_WRITE]);
		close(pipeStdout[PIPE_READ]);
		close(pipeStderr[PIPE_READ]);
		dup2(pipeStdin[PIPE_READ], STDIN_FILENO);
		dup2(pipeStdout[PIPE_WRITE], STDOUT_FILENO);
		dup2(pipeStderr[PIPE_WRITE], STDERR_FILENO);

		// prepare cmdline
		// HACK: use sh to handle quoting
		unsigned len = command.length();
		VLA(char, cmd, len + 1);
		memcpy(cmd, command.c_str(), len + 1);
		char* argv[4];
		argv[0] = const_cast<char*>("sh");
		argv[1] = const_cast<char*>("-c");
		argv[2] = cmd;
		argv[3] = 0;

		// really execute command
		execvp(argv[0], argv);
	} else {
		// parent thread
		close(pipeStdin[PIPE_READ]);
		close(pipeStdout[PIPE_WRITE]);
		close(pipeStderr[PIPE_WRITE]);

		fdIn  = pipeStdin[PIPE_WRITE];
		fdOut = pipeStdout[PIPE_READ];
		fdErr = pipeStderr[PIPE_READ];
	}
	return true;
}

#endif
