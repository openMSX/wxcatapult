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
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
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
	m_connectThread = nullptr;
	m_pipeActive = false;
	m_openMsxRunning = false;
	m_namedPipeHandle = INVALID_HANDLE_VALUE;
#else
	m_openMSXstdin = m_openMSXstdout = m_openMSXstderr = -1;
#endif

	m_openMSXID = 0;
	m_appWindow = (wxCatapultFrame*)target;
	m_openMsxRunning = false;
	m_pluggables.Clear();
	m_connectors.Clear();
	InitLaunchScript();
}

openMSXController::~openMSXController()
{
	if (m_openMsxRunning) {
		WriteCommand(wxT("exit"));
#ifndef __WXMSW__
		m_stdOutThread->Wait();
		m_stdErrThread->Wait();
		delete m_stdOutThread;
		delete m_stdErrThread;
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

bool openMSXController::PreLaunch()
{
	m_parser = new CatapultXMLParser(m_appWindow);
	return true;
}

bool openMSXController::PostLaunch()
{
	char initial[] = "<openmsx-control>\n";
	WriteMessage((unsigned char*)initial, strlen(initial));
	executeLaunch();
	m_appWindow->StartTimers();
	return true;
}

void openMSXController::HandleEndProcess(wxCommandEvent& event)
{
	if (!m_openMsxRunning) return;

	wxString leds[] = {
		wxT("power"), wxT("caps"),  wxT("kana"),
		wxT("pause"), wxT("turbo"), wxT("FDD")
	};
	for (auto& l : leds) {
		m_appWindow->UpdateLed(l, wxT("off"));
	}

	m_appWindow->StopTimers();
	m_appWindow->SetStatusText(wxT("Ready"));
	m_appWindow->EnableSaveSettings(false);
	delete m_parser;
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_appWindow->m_audioControlPage->DisableAudioPanel();
	m_openMsxRunning = false;
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_appWindow->SetControlsOnEnd();
	m_appWindow->m_launch_AbortButton->SetLabel(wxT("Start"));
	HandleNativeEndProcess();
	m_commands.clear();

#ifdef __WXMSW__
	if (!m_pipeActive) {
		m_connectThread = nullptr;
	}
#endif
}

void openMSXController::HandleStdOut(wxCommandEvent& event)
{
	auto* data = (wxString*)event.GetClientData();
	m_parser->ParseXmlInput(*data, m_openMSXID);
	delete data;
}

void openMSXController::HandleStdErr(wxCommandEvent& event)
{
	auto* data = (wxString*)event.GetClientData();
	if (*data == wxT("\n")) {
		delete data;
		return;
	}
	for (unsigned i = 0; i < m_appWindow->m_tabControl->GetPageCount(); ++i) {
		if (m_appWindow->m_tabControl->GetPageText(i) == wxT("Status Info")) {
			m_appWindow->m_tabControl->SetSelection(i);
		}
	}
	m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(
		wxColour(255, 23, 23),
		wxNullColour,
		wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)));
	m_appWindow->m_statusPage->m_outputtext->AppendText(*data);
	delete data;
}

void openMSXController::HandleParsedOutput(wxCommandEvent& event)
{
	auto* data = (CatapultXMLParser::ParseResult*)event.GetClientData();
	if (data->openMSXID != m_openMSXID) {
		delete data;
		return; // another instance is already started, ignore this message
	}
	switch (data->parseState) {
	case CatapultXMLParser::TAG_UPDATE:
		if (data->updateType == CatapultXMLParser::UPDATE_LED) {
			m_appWindow->UpdateLed (data->name, data->contents);
		}
		if (data->updateType == CatapultXMLParser::UPDATE_STATE) {
			if (data->name == wxT("cassetteplayer")) {
				m_appWindow->m_sessionPage->SetCassetteMode(data->contents);
			} else {
				m_appWindow->UpdateState(data->name, data->contents);
			}
		}
		if (data->updateType == CatapultXMLParser::UPDATE_SETTING) {
			wxString lastcmd = PeekPendingCommand();
			if (!lastcmd.StartsWith(wxT("set ")) ||
			    (lastcmd.Find(' ', true) == 3) ||
			    (lastcmd.Mid(4, lastcmd.Find(' ', true) - 4) != data->name)) {
				UpdateSetting2(data->name, data->contents);
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_PLUG) {
			wxString lastcmd = PeekPendingCommand();
			if (!lastcmd.StartsWith(wxT("plug ")) ||
			    (lastcmd.Find(' ', true) == 4) ||
			    (lastcmd.Mid(5, lastcmd.Find(' ', true) - 5) != data->name)) {
				UpdateSetting2(data->name, data->contents);
				executeLaunch(nullptr, m_relaunch);
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_UNPLUG) {
			wxString lastcmd = PeekPendingCommand();
			if (!lastcmd.StartsWith(wxT("unplug ")) ||
			  /*(lastcmd.Find(' ', true) == 6)) {*/
			    (lastcmd.Mid(7) != data->name)) {
				UpdateSetting2(data->name, data->contents);
				executeLaunch(nullptr, m_relaunch);
			}
		} else if (data->updateType == CatapultXMLParser::UPDATE_MEDIA) {
			wxString lastcmd = PeekPendingCommand();
			bool eject = false;
			int space = lastcmd.Find(' ', false);
			if ((space != wxNOT_FOUND) && (space != (int)lastcmd.Len() - 1)) {
				eject = true;
			}
			if ((lastcmd.Mid(0, data->name.Len() + 1) != (data->name + wxT(" "))) ||
			    (!eject && (lastcmd.Mid(space + 1) != (wxT("\"") + data->contents + wxT("\"")))) ||
			    (lastcmd.Left(18) == wxT("cassetteplayer new"))) {
				UpdateSetting2(data->name, data->contents);
				m_appWindow->m_sessionPage->UpdateSessionData();
			}
		}
		break;
	case CatapultXMLParser::TAG_REPLY:
		switch (data->replyState) {
		case CatapultXMLParser::REPLY_OK:
			if (PeekPendingCommandTarget() == TARGET_STARTUP) {
				HandleNormalLaunchReply(event);
			} else {
				wxString command = GetPendingCommand();
				if (command == wxT("openmsx_info fps")) {
					m_appWindow->SetFPSdisplay(data->contents);
				} else if (command == wxT("save_settings")) {
					wxMessageBox(wxT("openMSX settings saved successfully!"));
				} else if (command.Left(10) == wxT("screenshot")) {
					m_appWindow->m_videoControlPage->UpdateScreenshotCounter();
				}
			}
			break;
		case CatapultXMLParser::REPLY_NOK:
			if (PeekPendingCommandTarget() == TARGET_STARTUP) {
				HandleNormalLaunchReply(event);
			} else {
				wxString command = GetPendingCommand();
				if (command == wxT("plug msx-midi-in midi-in-reader")) {
					m_appWindow->m_audioControlPage->InvalidMidiInReader();
				} else if (command == wxT("plug msx-midi-out midi-out-logger")) {
					m_appWindow->m_audioControlPage->InvalidMidiOutLogger();
				} else if (command == wxT("plug pcminput wavinput")) {
					m_appWindow->m_audioControlPage->InvalidSampleFilename();
				} else if (command == wxT("plug printerport logger")) {
					m_appWindow->m_miscControlPage->InvalidPrinterLogFilename();
				} else if (command == wxT("save_settings")) {
					wxMessageBox(wxT("Error saving openMSX settings\n") + data->contents);
				} else {
					m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(
						wxColour(174, 0, 0),
						wxNullColour,
						wxFont(10, wxMODERN, wxNORMAL, wxBOLD)));
					m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("Warning: NOK received on command: "));
					m_appWindow->m_statusPage->m_outputtext->AppendText(command);
					m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
					if (!data->contents.IsEmpty()) {
						m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("contents = "));
						m_appWindow->m_statusPage->m_outputtext->AppendText(data->contents);
						m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
					}
				}
			}
			break;
		case CatapultXMLParser::REPLY_UNKNOWN:
			m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(
				wxColour(174, 0, 0),
				wxNullColour,
				wxFont(10, wxMODERN, wxNORMAL, wxBOLD)));
			m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("Warning: Unknown reply received!\n"));
			break;
		}
		break;
	case CatapultXMLParser::TAG_LOG:
		switch (data->logLevel) {
		case CatapultXMLParser::LOG_WARNING:
			m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(
				wxColour(174, 0, 0),
				wxNullColour,
				wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)));
			break;
		case CatapultXMLParser::LOG_INFO:
		case CatapultXMLParser::LOG_UNKNOWN:
			m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(
				wxColour(0, 0, 0),
				wxNullColour,
				wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)));
			break;
		}
		m_appWindow->m_statusPage->m_outputtext->AppendText(data->contents);
		m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
		if (data->contents.Left(15) == wxT("Screen saved to")) {
			int inhibit;
			ConfigurationData::instance().GetParameter(ConfigurationData::CD_SCREENSHOTINFO, &inhibit);
			if (inhibit == 0) {
				ScreenshotDlg dlg(m_appWindow);
				dlg.ShowModal();
			}
		}
		break;
	default:
		break;
	}
	delete data;
}

void openMSXController::WriteCommand(wxString msg, TargetType target)
{
	if (!m_openMsxRunning) return;

	CommandEntry temp;
	temp.command = msg;
	temp.target = target;
	m_commands.push_back(temp);

	xmlChar* buffer = xmlEncodeEntitiesReentrant(nullptr, (const xmlChar*)(const char*)(wxConvUTF8.cWX2MB(msg)));
	if (!buffer) return;

	auto len = strlen(reinterpret_cast<const char*>(buffer));
	auto len2 = 9 + len + 11;
	VLA(char, cmd, len2);
	memcpy(cmd, "<command>", 9);
	memcpy(cmd + 9, buffer, len);
	memcpy(cmd + 9 + len, "</command>\n", 11);
	WriteMessage((xmlChar*)cmd, len2);

	xmlFree(buffer);
}

wxString openMSXController::GetPendingCommand()
{
//	assert(!m_commands.empty());
	if (m_commands.empty()) {  // TODO: why is assert(!m_commands.empty()) triggered ?
		// can only happen if a reply is received without a previously sent command
		return wxString();
	}
	wxString result = m_commands.front().command;
	m_commands.pop_front();
	return result;
}

wxString openMSXController::PeekPendingCommand()
{
	return m_commands.empty() ? wxString() : m_commands.front().command;
}

enum openMSXController::TargetType openMSXController::PeekPendingCommandTarget()
{
	return m_commands.empty() ? TARGET_INTERACTIVE
	                          : m_commands.front().target;
}

bool openMSXController::StartOpenMSX(wxString cmd, bool getversion)
{
	++m_openMSXID;
	bool retval = true;
	if (getversion) {
		m_appWindow->SetStatusText(wxT("Initializing..."));
		wxString versioninfo = GetOpenMSXVersionInfo(cmd);
		retval = SetupOpenMSXParameters(versioninfo);
		m_appWindow->SetStatusText(wxT("Ready"));
	} else {
		m_appWindow->SetStatusText(wxT("Running"));
		m_appWindow->EnableSaveSettings(true);
		Launch(cmd);
	}
	return retval;
}

void openMSXController::HandleNormalLaunchReply(wxCommandEvent& event)
{
	executeLaunch(&event);
}

int openMSXController::InitConnectors(wxString dummy, wxString connectors)
{
	m_connectors.Clear();
	m_connectorclasses.Clear();
	wxStringTokenizer tkz(connectors, wxT("\n"));
	while (tkz.HasMoreTokens()) {
		m_connectors.Add(tkz.GetNextToken());
	}
	return 0; // don't skip any lines in the startup script
}

void openMSXController::GetConnectors(wxArrayString& connectors)
{
	connectors.Clear();
	if (m_connectors.IsEmpty()) return;

	for (unsigned i = 0; i < m_connectors.GetCount(); ++i) {
		connectors.Add(m_connectors[i]);
	}
}

wxString openMSXController::GetConnectorClass(wxString name)
{
	if (m_connectorclasses.IsEmpty()) return wxString();

	for (unsigned i = 0; i < m_connectors.GetCount(); ++i) {
		if (m_connectors[i] == name) {
			return m_connectorclasses[i];
		}
	}
	assert(false); return wxString();
}

int openMSXController::InitPluggables(wxString dummy, wxString pluggables)
{
	m_pluggables.Clear();
	m_pluggabledescriptions.Clear();
	m_pluggableclasses.Clear();
	wxStringTokenizer tkz(pluggables, wxT("\n"));
	while (tkz.HasMoreTokens()) {
		m_pluggables.Add(tkz.GetNextToken());
	}
	return 0; // don't skip any lines in the startup script
}

void openMSXController::GetPluggables(wxArrayString& pluggables)
{
	pluggables = m_pluggables;
}

void openMSXController::GetPluggableDescriptions(wxArrayString& descriptions)
{
	descriptions = m_pluggabledescriptions;
}

void openMSXController::GetPluggableClasses(wxArrayString& classes)
{
	classes = m_pluggableclasses;
}

bool openMSXController::SetupOpenMSXParameters(wxString version)
{
	long ver = -1;
	long majver;
	long minver;
	long subver;
	wxString temp = version;
	if (version.StartsWith(wxT("openMSX "))) {
		int pos = version.Find('.');
		if (pos != wxNOT_FOUND) {
			temp.Mid(8, pos - 8).ToLong(&majver);
			temp = temp.Mid(pos + 1);
			pos = temp.Find('.');
			if (pos != wxNOT_FOUND) {
				temp.Mid(0, pos).ToLong(&minver);
				temp.Mid(pos + 1).ToLong(&subver);
				ver = (((majver * 100) + minver) * 100) + subver;
			}
		}
	}
	// printf ("Detected openMSX version: %d\n", ver);
	if (ver == -1) {
		wxMessageBox(
			wxT("Unable to determine openMSX version!\nPlease upgrade to 0.6.2 or higher.\n(Or contact the authors.)"),
			wxT("Error"));
		return false;
	}
	if (ver < 602) {
		wxMessageBox(
			wxT("The openMSX version you are using is too old!\nPlease upgrade to 0.6.2 or higher."),
			wxT("Error"));
		return false;
	}
	m_appWindow->m_miscControlPage->FillInitialJoystickPortValues();
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
}

void openMSXController::InitLaunchScript()
{
	AddLaunchInstruction(wxT("openmsx_update enable setting"), wxT(""), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("openmsx_update enable led"), wxT(""), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("set power on"), wxT("e"), wxT("power"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("unset renderer"), wxT("e"), wxT(""), nullptr, true);
	AddLaunchInstruction(wxT("@execute"), wxT(""), wxT(""), &openMSXController::EnableMainWindow, false);
	AddLaunchInstruction(wxT("^info renderer"), wxT(""), wxT("RendererSelector"), &openMSXController::FillComboBox, true);
	AddLaunchInstruction(wxT("^info scale_algorithm"), wxT(""), wxT("ScalerAlgoSelector"), &openMSXController::FillComboBox, true);
	AddLaunchInstruction(wxT("lindex [openmsx_info setting scale_factor] 2"), wxT(""), wxT("ScalerFactorSelector"), &openMSXController::FillRangeComboBox, true);
	AddLaunchInstruction(wxT("^info accuracy"), wxT(""), wxT("AccuracySelector"), &openMSXController::FillComboBox, false);
	AddLaunchInstruction(wxT("openmsx_update enable media"), wxT(""), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("info exist frontswitch"), wxT(""), wxT("#"), &openMSXController::EnableFirmware, false);
	AddLaunchInstruction(wxT("info exist firmwareswitch"), wxT(""), wxT("#"), &openMSXController::EnableFirmware, false);
	AddLaunchInstruction(wxT("set renderer"), wxT(""), wxT("renderer"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set scale_algorithm"), wxT(""), wxT("scale_algorithm"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set scale_factor"), wxT(""), wxT("scale_factor"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set accuracy"), wxT(""), wxT("accuracy"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set deinterlace"), wxT(""), wxT("deinterlace"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set limitsprites"), wxT(""), wxT("limitsprites"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set fullscreen"), wxT(""), wxT("fullscreen"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set blur"), wxT(""), wxT("blur"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set glow"), wxT(""), wxT("glow"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set gamma"), wxT(""), wxT("gamma"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set scanline"), wxT("0"), wxT("scanline"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("info exist renshaturbo"), wxT("1"), wxT("#"), &openMSXController::EnableRenShaTurbo, false);
	AddLaunchInstruction(wxT("set renshaturbo"), wxT("0"), wxT("renshaturbo"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set save_settings_on_exit"), wxT(""), wxT("save_settings_on_exit"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set printerlogfilename"), wxT(""), wxT("printerlogfilename"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("@execute"), wxT(""), wxT(""), &openMSXController::SetSliderDefaults, false);
	AddLaunchInstruction(wxT("set speed"), wxT(""), wxT("speed"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set maxframeskip"), wxT(""), wxT("maxframeskip"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set minframeskip"), wxT(""), wxT("minframeskip"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set throttle"), wxT(""), wxT("throttle"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set cmdtiming"), wxT(""), wxT("cmdtiming"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("!info pluggable"), wxT("13"), wxT(""), &openMSXController::InitPluggables, true);
	AddLaunchInstruction(wxT("!info_nostore pluggable *"), wxT(""), wxT("*"), &openMSXController::AddPluggableDescription, true);
	AddLaunchInstruction(wxT("!info_nostore connectionclass *"), wxT(""), wxT("*"), &openMSXController::AddPluggableClass, false);
	AddLaunchInstruction(wxT("!info connector"), wxT("10"), wxT(""), &openMSXController::InitConnectors, true);
	AddLaunchInstruction(wxT("!info_nostore connectionclass *"), wxT(""), wxT("*"), &openMSXController::AddConnectorClass, false);
	AddLaunchInstruction(wxT("@checkfor msx-midi-in"), wxT("1"), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("set midi-in-readfilename"), wxT(""), wxT("midi-in-readfilename"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("@checkfor msx-midi-out"), wxT("1"), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("set midi-out-logfilename"), wxT(""), wxT("midi-out-logfilename"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("@checkfor pcminput"), wxT("1"), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("set audio-inputfilename"), wxT(""), wxT("audio-inputfilename"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("@execute"), wxT(""), wxT(""), &openMSXController::InitConnectorPanel, false);
	m_relaunch = m_launchScript.size(); // !!HACK!!
	AddLaunchInstruction(wxT("@execute"), wxT(""), wxT(""), &openMSXController::InitAudioConnectorPanel, false);
//	AddLaunchInstruction(wxT("#info romtype"), wxT(""), wxT(""), &openMSXController::InitRomTypes, true);
//	AddLaunchInstruction(wxT("#info_nostore romtype *"), wxT(""), wxT("*"), &openMSXController::SetRomDescription, true);
	AddLaunchInstruction(wxT("!info sounddevice"), wxT("5"), wxT(""), &openMSXController::InitSoundDevices, true);
	AddLaunchInstruction(wxT("!info_nostore sounddevice *"), wxT(""), wxT("*"), &openMSXController::SetChannelType, true);
	AddLaunchInstruction(wxT("@execute"), wxT(""), wxT(""), &openMSXController::SetChannelTypeDone, false);
	AddLaunchInstruction(wxT("set master_volume"), wxT(""), wxT("master_volume"), &openMSXController::UpdateSetting, false);
	AddLaunchInstruction(wxT("set *_volume"), wxT(""), wxT("*_volume"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set *_balance"), wxT(""), wxT("*_balance"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("set mute"), wxT(""), wxT("mute"), &openMSXController::UpdateSetting, true);
	AddLaunchInstruction(wxT("plug cassetteport"), wxT(""), wxT("cassetteport"), &openMSXController::EnableCassettePort, false);
	AddLaunchInstruction(wxT("join [cassetteplayer] \\n"), wxT(""), wxT(""), &openMSXController::SetCassetteMode, false);
	AddLaunchInstruction(wxT("openmsx_update enable plug"), wxT(""), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("openmsx_update enable unplug"), wxT(""), wxT(""), nullptr, false);
	AddLaunchInstruction(wxT("openmsx_update enable status"), wxT(""), wxT(""), nullptr, false);

	AddSetting(wxT("renderer"), wxT("RendererSelector"), &openMSXController::UpdateCombo);
	AddSetting(wxT("scale_algorithm"), wxT("ScalerAlgoSelector"), &openMSXController::UpdateCombo);
	AddSetting(wxT("scale_factor"), wxT("ScalerFactorSelector"), &openMSXController::UpdateCombo);
	AddSetting(wxT("accuracy"), wxT("AccuracySelector"), &openMSXController::UpdateCombo);
	AddSetting(wxT("deinterlace"), wxT("DeInterlaceButton"), &openMSXController::UpdateToggle, S_CONVERT);
	AddSetting(wxT("limitsprites"), wxT("LimitSpriteButton"), &openMSXController::UpdateToggle, S_CONVERT);
	AddSetting(wxT("blur"), wxT("BlurIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("glow"), wxT("GlowIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("gamma"), wxT("GammaIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("scanline"), wxT("ScanlineIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("speed"), wxT("SpeedIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("minframeskip"), wxT("MinFrameSkipIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("maxframeskip"), wxT("MaxFrameSkipIndicator"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("throttle"), wxT("MaxSpeedButton"), &openMSXController::UpdateToggle, S_INVERT | S_EVENT);
	AddSetting(wxT("power"), wxT("PowerButton"), &openMSXController::UpdateToggle);
	AddSetting(wxT("pause"), wxT("PauseButton"), &openMSXController::UpdateToggle);
	AddSetting(wxT("frontswitch"), wxT("FirmwareButton"), &openMSXController::UpdateToggle);
	AddSetting(wxT("firmwareswitch"), wxT("FirmwareButton"), &openMSXController::UpdateToggle);
	AddSetting(wxT("mute"), wxT("MuteButton"), &openMSXController::UpdateToggle);
	AddSetting(wxT("midi-in-readfilename"), wxT("MidiInFileInput"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("midi-out-logfilename"), wxT("MidiOutFileInput"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("audio-inputfilename"), wxT("SampleFileInput"), &openMSXController::UpdateIndicator);
	AddSetting(wxT("*_volume"), wxT(""), &openMSXController::UpdateVolume);
	AddSetting(wxT("*_balance"), wxT(""), &openMSXController::UpdateBalance);
	AddSetting(wxT("msx-midi-in"), wxT("MidiInSelector"), &openMSXController::UpdateMidiPlug);
	AddSetting(wxT("msx-midi-out"), wxT("MidiOutSelector"), &openMSXController::UpdateMidiPlug);
	AddSetting(wxT("pcminput"), wxT("SampleInSelector"), &openMSXController::UpdatePluggable);
	AddSetting(wxT("joyporta"), wxT("Joyport1Selector"), &openMSXController::UpdatePluggable);
	AddSetting(wxT("joyportb"), wxT("Joyport2Selector"), &openMSXController::UpdatePluggable);
	AddSetting(wxT("printerport"), wxT("PrinterportSelector"), &openMSXController::UpdatePluggable);
	AddSetting(wxT("renshaturbo"), wxT("RenshaTurboSlider"), &openMSXController::UpdateSlider);
	AddSetting(wxT("diska"), wxT("DiskAContents"), &openMSXController::UpdateCombo);
	AddSetting(wxT("diskb"), wxT("DiskBContents"), &openMSXController::UpdateCombo);
	AddSetting(wxT("cassetteplayer"), wxT("CassetteContents"), &openMSXController::UpdateCombo);
	AddSetting(wxT("fullscreen"), wxT("FullScreenButton"), &openMSXController::UpdateToggle, S_CONVERT);
	AddSetting(wxT("save_settings_on_exit"), wxT("Save Settings On Exit"), &openMSXController::UpdateMenu);
	AddSetting(wxT("printerlogfilename"), wxT("PrinterLogFile"), &openMSXController::UpdateIndicator, S_CONVERT);
}

void openMSXController::AddLaunchInstruction(
	wxString cmd, wxString action, wxString parameter,
	int (openMSXController::*pfunction)(wxString, wxString),
	bool showError)
{
	LaunchInstruction instr;
	instr.command = cmd;
	instr.action = action;
	instr.parameter = parameter;
	instr.p_okfunction = pfunction;
	instr.showError = showError;
	m_launchScript.push_back(instr);
}

void openMSXController::executeLaunch(wxCommandEvent* event, int startLine)
{
	static bool wait;
	static int sendStep = 0;
	static int recvStep = 0;
	static int sendLoop = -1;
	static int recvLoop = -1;
	static wxString lastdata;

	if (event) {
		auto* data = (CatapultXMLParser::ParseResult*)event->GetClientData();
		// handle received command
		wxString command = GetPendingCommand();
		wxString instruction  = m_launchScript[recvStep].command;
		while (instruction.StartsWith(wxT("@"))) {
			++recvStep;
			instruction = m_launchScript[recvStep].command;
		}
		if ((recvLoop == -1) && instruction.Contains(wxT("*"))) {
			recvLoop = 0;
		}
		wxArrayString tokens;
		tokenize(instruction, wxT(" "), tokens);
		wxString cmd = translate(tokens, recvLoop, lastdata);
		if (command == cmd) {
			if (tokens[0] == wxT("#info")) {
				lastdata = data->contents;
			}
			if (tokens[0] == wxT("^info")) {
				lastdata = data->contents;
			}
			if (tokens[0] == wxT("!info")) {
				lastdata = data->contents;
			}
			HandleLaunchReply(cmd, event, m_launchScript[recvStep], &sendStep, recvLoop, lastdata);
			if ((data->replyState == CatapultXMLParser::REPLY_NOK) ||
			    ((cmd.StartsWith(wxT("info exist")) && (data->contents == wxT("0"))))) {
				long displace;
				m_launchScript[recvStep].action.ToLong(&displace);
				recvStep += displace;
			}
			if (cmd == wxT("!done")) {
				recvStep = -2; // it is gonna be increased in a moment
				recvLoop = -1;
			}

			if (recvLoop != -1) {
				wxArrayString lastvalues;
				if (recvLoop < int(tokenize(lastdata, wxT("\n"), lastvalues) - 1)) {
					++recvLoop;
				} else {
					recvLoop = -1;
					++recvStep;
				}
			} else {
				++recvStep;
			}
		}
	} else {
		// init chain of events
		sendStep = startLine;
		recvStep = startLine;
		sendLoop = -1;
		recvLoop = -1;
	}
	if (recvStep >= int(m_launchScript.size())) {
		recvStep = 0;
		FinishLaunch();
		return;
	}
	if (wait) {
		while (m_launchScript[recvStep].command.StartsWith(wxT("@"))) {
			++recvStep;
		}
		if (recvStep >= sendStep) {
			wait = false;
		}
	}

	while (!wait && (sendStep < int(m_launchScript.size()))) {
		wxString instruction = m_launchScript[sendStep].command;
		if ((sendLoop == -1) && instruction.Contains(wxT("*"))) {
			sendLoop = 0;
		}
		wxArrayString tokens;
		tokenize(instruction, wxT(" "), tokens);
		wxString cmd = translate(tokens, sendLoop, lastdata);
		if (cmd.StartsWith(wxT("@"))) {
			wxString result = wxT("0");
			if (tokens[0] == wxT("@checkfor")) {
				bool contains = lastdata.Contains(tokens[1]);
				if (contains) {
					result = wxT("1");
				}
			} else {
				// @execute
				result = wxT("1");
			}
			if (result == wxT("0")) {
				while (m_launchScript[recvStep].command.StartsWith(wxT("@"))) {
					++recvStep;
				}
				long displace;
				m_launchScript[sendStep].action.ToLong(&displace);
				recvStep += displace;
			}
			HandleLaunchReply(tokens[0] + result, nullptr, m_launchScript[sendStep], &sendStep, -1, wxT(""));
		} else {
			WriteCommand(cmd, TARGET_STARTUP);
		}
		wxString action = m_launchScript[sendStep].action;

		if (sendLoop != -1) {
			wxArrayString lastvalues;
			if (sendLoop < int(tokenize(lastdata, wxT("\n"), lastvalues) - 1)) {
				++sendLoop;
			} else {
				sendLoop = -1;
				++sendStep;
				wait = true;
			}
		} else {
			++sendStep;
		}

		if (!action.IsEmpty()) {
			if (recvStep < sendStep) {
				wait = true;
			}
		}
	}
}

void openMSXController::FinishLaunch()
{
	m_appWindow->m_sessionPage->AutoPlugCassette();
	m_appWindow->SetControlsOnLaunch();
	m_appWindow->m_sessionPage->SetCassetteControl();
}

size_t openMSXController::tokenize(
	const wxString& text, const wxString& seperator, wxArrayString& result)
{
	wxStringTokenizer tkz(text, seperator);
	while (tkz.HasMoreTokens()) {
		result.Add(tkz.GetNextToken());
	}
	return result.GetCount();
}

wxString openMSXController::translate(wxArrayString tokens, int loop, wxString lastdata)
{
	if (loop != -1) {
		for (auto& token : tokens) {
			if (!token.Contains(wxT("*"))) continue;
			wxArrayString lastvalues;
			tokenize(lastdata, wxT("\n"), lastvalues);
			if (loop < (int)lastvalues.GetCount()) {
				token.Replace(wxT("*"), lastvalues[loop]);
				token.Replace(wxT(" "), wxT("\\ "));
			}
		}
	}

	switch (tokens[0][0]) {
	case '#': {
		assert(tokens[0].StartsWith(wxT("#info")));
		wxString parameter;
		while (tokens.GetCount() > 1) {
			parameter << tokens[1] << wxT(" ");
			tokens.RemoveAt(1);
		}
		parameter.Trim(true);
		tokens[0] = wxT("join [openmsx_info ") + parameter + wxT("] \\n");
		break;
	}
	case '!': {
		assert(tokens[0].StartsWith(wxT("!info")));
		wxString parameter;
		while (tokens.GetCount() > 1) {
			parameter << tokens[1] << wxT(" ");
			tokens.RemoveAt(1);
		}
		parameter.Trim(true);
		tokens[0] = wxT("join [machine_info ") + parameter + wxT("] \\n");
		break;
	}
	case '^': {
		assert(tokens[0].StartsWith(wxT("^info")));
		wxString parameter;
		while (tokens.GetCount() > 1) {
			parameter << tokens[1] << wxT(" ");
			tokens.RemoveAt(1);
		}
		parameter.Trim(true);
		tokens[0] = wxT("join [lindex [openmsx_info setting ") + parameter + wxT("] 2] \\n");
		break;
	}
	default:
		break;
	}

	wxString result;
	for (auto& token : tokens) {
		result << token << wxT(" ");
	}
	result.Trim(true);
	return result;
}

void openMSXController::HandleLaunchReply(
	wxString cmd, wxCommandEvent* event,
	LaunchInstruction instruction, int* sendStep, int loopcount,
	wxString datalist)
{
	CatapultXMLParser::ParseResult* data = nullptr;
	if (event) {
		data = (CatapultXMLParser::ParseResult*)event->GetClientData();
	}
	bool ok = false;
	if (cmd.StartsWith(wxT("@"))) {
		if (cmd.Mid(cmd.Len() - 1) == wxT("1")) {
			ok = true;
		}
	} else {
		assert(data!=0);
		if(data==0)throw "ERR1: data==0";
		if (cmd.StartsWith(wxT("info exist"))) {
			if (data->contents == wxT("1")) {
				ok = true;
			}
		} else {
			if (data->replyState == CatapultXMLParser::REPLY_OK) {
				ok = true;
			}
		}
	}
	wxString actions = instruction.action;

	if (ok) {
		if (instruction.p_okfunction != nullptr) {
			wxString parameter = instruction.parameter;
			wxString contents;
			if (loopcount > -1) {
				wxArrayString temp;
				tokenize(datalist, wxT("\n"), temp);
				parameter.Replace(wxT("*"), temp[loopcount]);
				parameter.Replace(wxT(" "), wxT("\\ "));
			}
			if (parameter == wxT("#")) {
				parameter = cmd;
			}
			if (event) {
				assert(data!=0);
				if(data==0)throw "ERR2: data==0";
				contents = data->contents;
			}
			int result = (*this.*(instruction.p_okfunction))(parameter, contents);
			if (result > 0) {
				*sendStep += result;
			}
		}
	} else {
		if (instruction.showError) {
			m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("Error received on command: ") + cmd + wxT("\n"));
		}
		if (!actions.IsEmpty()) {
			if (actions == wxT("e")) {
				*sendStep = m_launchScript.size();
			} else {
				long displace;
				actions.ToLong(&displace);
				*sendStep += displace;
			}
		}
	}
}

int openMSXController::UpdateSetting(wxString setting, wxString data)
{
	UpdateSetting2(setting, data);
	return 0; // don't skip any lines in the startup script
}
void openMSXController::UpdateSetting2(const wxString& name_, const wxString& data)
{
	wxString name = name_; // TODO HACK: need a proper Tcl parser
	name.Replace(wxT("\\"), wxT(""));

	for (auto& elem : m_settingTable) {
		if (name.Matches(elem.setting.c_str())) {
			(*this.*(elem.pfunction))(
				name, data, elem.control, elem.flags);
			break;
		}
	}
}

void openMSXController::AddSetting(
	const wxString& setting, const wxString& control,
	void (openMSXController::*pfunction)(const wxString&, const wxString&, const wxString&, int),
	int flags)
{
	SettingElement elem;
	elem.setting   = setting;
	elem.control   = control;
	elem.pfunction = pfunction;
	elem.flags     = flags;
	m_settingTable.push_back(elem);
}

static bool isTclTrue(const wxString& str)
{
	return (str == wxT("on"))   ||
	       (str == wxT("true")) ||
	       (str == wxT("1"))    ||
	       (str == wxT("yes"));
}

void openMSXController::UpdateToggle(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* button = (wxToggleButton*)wxWindow::FindWindowByName(control)) {
		bool active = !(flags & S_INVERT);
		bool sendvalue = isTclTrue(data) ? active : !active;
		button->SetValue(sendvalue);

		if (flags & S_EVENT) {
			wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, button->GetId());
			event.SetInt(active);
			event.SetEventObject(button);
			button->ProcessEvent(event);
		}

		if (flags & S_CONVERT) {
			button->SetLabel(isTclTrue(data) ? wxT("On") : wxT("Off"));
		}
	}
}

void openMSXController::UpdateCombo(const wxString& setting, const wxString& data, const wxString& control, int flags)
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

void openMSXController::UpdateIndicator(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* indicator = (wxTextCtrl*)wxWindow::FindWindowByName(control)) {
		wxString tempData = data;
		if (flags & S_CONVERT) {
			tempData = CatapultPage::ConvertPath(data, true, true);
		}
		if (indicator->GetValue() != tempData) {
			indicator->SetValue(tempData);
		}
	}
}

void openMSXController::UpdateSlider(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* slider = (wxSlider*)wxWindow::FindWindowByName(control)) {
		long value;
		data.ToLong(&value, 10);
		if (slider->GetValue() != value) {
			slider->SetValue(value);
		}
	}
}

void openMSXController::UpdateMenu(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	int menusetting = m_appWindow->GetMenuBar()->FindMenuItem(wxT("&Settings"), wxT("Save openMSX Settings On &Exit"));
	if (menusetting != wxNOT_FOUND) {
		m_appWindow->GetMenuBar()->Check(menusetting, isTclTrue(data));
	}
}

void openMSXController::UpdateVolume(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	wxString channel = setting.Mid(0, setting.Length() - 7); // remove "_volume"
	m_appWindow->m_audioControlPage->SetChannelVolume(channel, data);
}

void openMSXController::UpdateBalance(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	wxString channel = setting.Mid(0, setting.Length() - 8); // remove "_balance"
	m_appWindow->m_audioControlPage->SetChannelMode(channel, data);
}

void openMSXController::UpdateMidiPlug(const wxString& connector, const wxString& data, const wxString& control, int flags)
{
	m_appWindow->m_audioControlPage->UpdateMidiPlug(control, data);
}

void openMSXController::UpdatePluggable(const wxString& connector, const wxString& data, const wxString& control, int flags)
{
	if (auto* box = (wxComboBox*)wxWindow::FindWindowByName(control)) {
		box->SetValue(data.IsEmpty() ? wxT("--empty--") : data);
	}
}

int openMSXController::FillComboBox(wxString control, wxString data)
{
	auto* box = (wxComboBox*)wxWindow::FindWindowByName(control);
	box->Clear();
	wxStringTokenizer tkz(data, wxT("\n"));
	while (tkz.HasMoreTokens()) {
		box->Append(tkz.GetNextToken());
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::FillRangeComboBox(wxString setting, wxString data)
{
	long min;
	long max;
	wxString range;
	int pos = data.Find(' ');
	if (pos != wxNOT_FOUND) {
		data.Left(pos).ToLong(&min);
		data.Mid(pos + 1).ToLong(&max);
		for (long index = min; index <= max; ++index) {
			range << index << wxT("\n");
		}
	}
	FillComboBox(setting, range);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableFirmware(wxString cmd, wxString data)
{
	if ((data != wxT("0")) || cmd.StartsWith(wxT("set "))) {
		if (cmd.Contains(wxT("frontswitch"))) {
			m_appWindow->m_miscControlPage->EnableFirmware(wxT("frontswitch"));
		} else {
			m_appWindow->m_miscControlPage->EnableFirmware(wxT("firmwareswitch"));
		}
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableRenShaTurbo(wxString cmd, wxString data)
{
	if ((data != wxT("0")) || cmd.StartsWith(wxT("set "))) {
		m_appWindow->m_miscControlPage->EnableRenShaTurbo();
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableMainWindow(wxString dummy1, wxString dummy2)
{
	m_appWindow->EnableMainWindow();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitRomTypes(wxString dummy, wxString data)
{
	wxArrayString types;
	tokenize(data, wxT("\n"), types);
	for (auto& t : types) {
		m_appWindow->m_sessionPage->AddRomType(t);
	}
	return 0;
}

int openMSXController::SetRomDescription(wxString name, wxString data)
{
	m_appWindow->m_sessionPage->SetRomTypeFullName(name, data);
	return 0;
}

int openMSXController::InitSoundDevices(wxString dummy, wxString data)
{
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_appWindow->m_audioControlPage->InitAudioChannels();
	return 0;
}
int openMSXController::SetChannelType(wxString name, wxString data)
{
	m_appWindow->m_audioControlPage->AddChannelType(name, data);
	return 0;
}
int openMSXController::SetChannelTypeDone(wxString name, wxString data)
{
	m_appWindow->m_audioControlPage->SetupAudioMixer();
	return 0;
}

int openMSXController::AddPluggableDescription(wxString name, wxString data)
{
	m_pluggabledescriptions.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddPluggableClass(wxString name, wxString data)
{
	m_pluggableclasses.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddConnectorClass(wxString name, wxString data)
{
	m_connectorclasses.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::SetSliderDefaults(wxString dummy1, wxString dummy2)
{
	m_appWindow->m_videoControlPage->SetSliderDefaults();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitAudioConnectorPanel(wxString dummy1, wxString dummy2)
{
	m_appWindow->m_audioControlPage->InitAudioIO();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitConnectorPanel(wxString dummy1, wxString dummy2)
{
	m_appWindow->m_miscControlPage->InitConnectorPanel();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableCassettePort(wxString dummy, wxString data)
{
	m_appWindow->m_sessionPage->EnableCassettePort(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::SetCassetteMode(wxString dummy, wxString data)
{
	wxArrayString arrayData;
	int args = tokenize(data, wxT("\n"), arrayData);
	m_appWindow->m_sessionPage->SetCassetteMode(arrayData[args - 1]);
	return 0;
}

void openMSXController::UpdateMixer()
{
	executeLaunch(nullptr, m_relaunch);
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

void openMSXController::WriteMessage(xmlChar* msg, size_t length)
{
	if (!m_openMsxRunning) return;
#ifdef __WXMSW__
	unsigned long BytesWritten;
	::WriteFile(m_outputHandle, msg, length, &BytesWritten, nullptr);
	// ignore return value, BytesWritten
#else
	ssize_t r = write(m_openMSXstdin, msg, length);
	(void)r; // We really should check this return value, but for now
	         // just silence the warning.
#endif
}

bool openMSXController::Launch(wxString cmdline)
{
#ifdef __WXMSW__
	m_catapultWindow = GetActiveWindow();
	PreLaunch();
	bool useNamedPipes = DetermenNamedPipeUsage();
	cmdline += CreateControlParameter(useNamedPipes);
	HANDLE hInputRead, hOutputWrite, hErrorWrite, hErrorRead, hOutputRead;
	CreatePipes(useNamedPipes, &hInputRead, &hOutputWrite, &hErrorWrite, &hOutputRead, &hErrorRead);

	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
	WORD wStartupWnd = SW_HIDE;

	STARTUPINFO si;
	DWORD dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = dwFlags;
	si.hStdInput = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError  = hErrorWrite;
	si.wShowWindow = wStartupWnd;

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

	if (useNamedPipes) {
		if (!m_pipeActive) {
			m_pipeActive = true;
			m_connectThread->SetHandle(m_namedPipeHandle);
			m_connectThread->Run();
		}
		m_outputHandle = m_namedPipeHandle;

	} else {
		m_openMsxRunning = true;
		PostLaunch();
		m_appWindow->m_launch_AbortButton->Enable(true);
	}
	m_openMsxRunning = true;
	CloseHandles(useNamedPipes, m_openmsxProcInfo.hThread, hInputRead, hOutputWrite, hErrorWrite);

	return true;
#else
	PreLaunch();
	cmdline += wxT(" -control stdio");
	if (!execute((const char*)(wxConvUTF8.cWX2MB((cmdline))),
	             m_openMSXstdin, m_openMSXstdout, m_openMSXstderr)) {
		return false;
	}
	m_stdOutThread = new PipeReadThread(m_appWindow, MSGID_STDOUT, wxTHREAD_JOINABLE);
	if (m_stdOutThread->Create() == wxTHREAD_NO_ERROR) {
		m_stdOutThread->SetFileDescriptor(m_openMSXstdout);
		m_stdOutThread->Run();
	}
	m_stdErrThread = new PipeReadThread(m_appWindow, MSGID_STDERR, wxTHREAD_JOINABLE);
	if (m_stdErrThread->Create() == wxTHREAD_NO_ERROR) {
		m_stdErrThread->SetFileDescriptor(m_openMSXstderr);
		m_stdErrThread->Run();
	}

	m_openMsxRunning = true;
	PostLaunch();
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
#endif
}

void openMSXController::HandleNativeEndProcess()
{
#ifdef __WXMSW__
	// nothing
#else
	close(m_openMSXstdin);
#endif
}

wxString openMSXController::GetOpenMSXVersionInfo(wxString openmsxCmd)
{
	wxString version;
#ifdef __WXMSW__
	wxArrayString output;
	int code = wxExecute(openmsxCmd + wxT(" -v"), output);
	if ((code != -1) && !output.IsEmpty()) {
		version = output[0];
	}
#else
	if (system((const char*)(wxConvUTF8.cWX2MB((openmsxCmd + wxT(" -v > /tmp/catapult.tmp"))))) == 0) {
		wxTextFile tempfile(wxT("/tmp/catapult.tmp"));
		if (tempfile.Open()) {
			version = tempfile.GetFirstLine();
			tempfile.Close();
		}
	}
#endif
	return version;
}

// windows or linus specific stuff
#ifdef __WXMSW__

bool openMSXController::DetermenNamedPipeUsage()
{
	bool useNamedPipes = false;
	if (!FORCE_UNNAMED_PIPES) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx(&info)) {
			wxMessageBox(wxString::Format(
				wxT("Error getting system info: %ld "), GetLastError()));
		} else {
			if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
				useNamedPipes = true; // nt-based only and only if the user wants it
			}
		}
	}
	return useNamedPipes;
}

wxString openMSXController::CreateControlParameter(bool useNamedPipes)
{
	wxString parameter = wxT(" -control");

	if (useNamedPipes) {
		if (m_connectThread == nullptr) {
			m_launchCounter++;
		}
		auto pipeName = wxString::Format(
			wxT("\\\\.\\pipe\\Catapult-%u-%lu"), _getpid(), m_launchCounter);
		parameter += wxT(" pipe:") + pipeName.Mid(9);
		if (m_connectThread == nullptr) {
			m_connectThread = new PipeConnectThread(m_appWindow);
			m_connectThread->Create();
			m_namedPipeHandle = CreateNamedPipe(pipeName, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 10000, 0, 100, nullptr);
			if (m_namedPipeHandle == INVALID_HANDLE_VALUE) {
				wxMessageBox(wxString::Format(
					wxT("Error creating pipe: %ld"), GetLastError()));
			}
		} else {
			m_namedPipeHandle = m_outputHandle;
		}
	} else {
		parameter += wxT(" stdio:");
	}
	return parameter;
}

bool openMSXController::CreatePipes(
	bool useNamedPipes, HANDLE* input, HANDLE* output, HANDLE* error,
	HANDLE* outputWrite, HANDLE* errorWrite)
{
	HANDLE hOutputReadTmp, hOutputWrite;
	HANDLE hErrorReadTmp, hErrorWrite;
	HANDLE hInputRead = 0, hInputWriteTmp, hInputWrite;
	HANDLE hInputHandle = GetStdHandle(STD_INPUT_HANDLE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	if (!useNamedPipes) {
		if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0)) {
			ShowError(wxT("Error creating pipe for stdin"));
			return false;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hInputWriteTmp,
		                     GetCurrentProcess(), &hInputWrite, 0,
		                     FALSE, DUPLICATE_SAME_ACCESS)) {
			ShowError(wxT("Error Duplicating InputWriteTmp Handle"));
			return false;
		}
		if (!CloseHandle(hInputWriteTmp)) {
			ShowError(wxT("Error Closing Input Temp Handle"));
			return false;
		}
		m_outputHandle = hInputWrite;
		hInputHandle = hInputRead;
	}

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

void openMSXController::ShowError(wxString msg)
{
	wxMessageBox(msg + wxString::Format(wxT(": error %ld"), GetLastError()));
}

void openMSXController::CloseHandles(
	bool useNamedPipes, HANDLE hThread, HANDLE hInputRead,
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
	if (!useNamedPipes) {
		if (!CloseHandle(hInputRead)) {
			wxMessageBox(wxT("Unable to close Input Read"));
			return;
		}
	}
}

void openMSXController::HandlePipeCreated()
{
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_pipeActive = false;
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
