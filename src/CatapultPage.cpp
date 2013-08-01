#include "CatapultPage.h"
#include "AudioControlPage.h"
#include "VideoControlPage.h"
#include "ConfigurationData.h"
//#include "wx/version.h"
#include <wx/msgdlg.h>
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/slider.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

#define S_CONVERT 1
#define S_EVENT 2
#define S_INVERT 4
#define SETTINGTABLE_MAXSIZE 37

CatapultPage::CatapultPage(wxWindow* parent)
	: m_parent(parent)
{
	InitSettingsTable();
}

CatapultPage::~CatapultPage()
{
	delete[] m_settingTable;
}

wxString CatapultPage::ConvertPath(wxString path, bool convertSlash, bool backwards)
{
	if (backwards) {
		path.Replace(wxT("\""), wxT(""), true);
	} else {
		path.Prepend(wxT("\""));
		path.Append(wxT("\""));
	}
	if (convertSlash) {
		if (backwards){
			path.Replace(wxT("/"), wxT("\\"), true);
		} else {
			path.Replace(wxT("\\"), wxT("/"), true);
		}
	}
	return path;
}

void CatapultPage::OnClickCombo(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString sel = box->GetString(box->GetSelection());
	if (sel != box->GetValue()) {
		box->SetValue(sel);
	}
}

void CatapultPage::InitSettingsTable ()
{
	m_settingTableSize = 0;
	m_settingTable = new SettingTableElementType[SETTINGTABLE_MAXSIZE];
	AddSetting(wxT("renderer"), wxT("RendererSelector"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("scale_algorithm"), wxT("ScalerAlgoSelector"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("scale_factor"), wxT("ScalerFactorSelector"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("accuracy"), wxT("AccuracySelector"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("deinterlace"), wxT("DeInterlaceButton"), &CatapultPage::UpdateToggleSetting, S_CONVERT);
	AddSetting(wxT("limitsprites"), wxT("LimitSpriteButton"), &CatapultPage::UpdateToggleSetting, S_CONVERT);
	AddSetting(wxT("blur"), wxT("BlurIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("glow"), wxT("GlowIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("gamma"), wxT("GammaIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("scanline"), wxT("ScanlineIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("speed"), wxT("SpeedIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("minframeskip"), wxT("MinFrameSkipIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("maxframeskip"), wxT("MaxFrameSkipIndicator"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("throttle"), wxT("MaxSpeedButton"), &CatapultPage::UpdateToggleSetting, S_INVERT | S_EVENT);
	AddSetting(wxT("power"), wxT("PowerButton"), &CatapultPage::UpdateToggleSetting, 0);
	AddSetting(wxT("pause"), wxT("PauseButton"), &CatapultPage::UpdateToggleSetting, 0);
	AddSetting(wxT("frontswitch"), wxT("FirmwareButton"), &CatapultPage::UpdateToggleSetting, 0);
	AddSetting(wxT("firmwareswitch"), wxT("FirmwareButton"), &CatapultPage::UpdateToggleSetting, 0);
	AddSetting(wxT("mute"), wxT("MuteButton"), &CatapultPage::UpdateToggleSetting, 0);
	AddSetting(wxT("midi-in-readfilename"), wxT("MidiInFileInput"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("midi-out-logfilename"), wxT("MidiOutFileInput"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("audio-inputfilename"), wxT("SampleFileInput"), &CatapultPage::UpdateIndicatorSetting, 0);
	AddSetting(wxT("*_volume"), wxT("volume"), &CatapultPage::UpdateAudioSetting, 0);
	AddSetting(wxT("*_balance"), wxT("balance"), &CatapultPage::UpdateAudioSetting, 0);
	AddSetting(wxT("msx-midi-in"), wxT("MidiInSelector"), &CatapultPage::UpdateMidiPlug, 0);
	AddSetting(wxT("msx-midi-out"), wxT("MidiOutSelector"), &CatapultPage::UpdateMidiPlug, 0);
	AddSetting(wxT("pcminput"), wxT("SampleInSelector"), &CatapultPage::UpdatePluggable, 0);
	AddSetting(wxT("joyporta"), wxT("Joyport1Selector"), &CatapultPage::UpdatePluggable, 0);
	AddSetting(wxT("joyportb"), wxT("Joyport2Selector"), &CatapultPage::UpdatePluggable, 0);
	AddSetting(wxT("printerport"), wxT("PrinterportSelector"), &CatapultPage::UpdatePluggable, 0);
	AddSetting(wxT("renshaturbo"), wxT("RenshaTurboSlider"), &CatapultPage::UpdateSliderSetting, 0);
	AddSetting(wxT("diska"), wxT("DiskAContents"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("diskb"), wxT("DiskBContents"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("cassetteplayer"), wxT("CassetteContents"), &CatapultPage::UpdateComboSetting, 0);
	AddSetting(wxT("fullscreen"), wxT("FullScreenButton"), &CatapultPage::UpdateToggleSetting, S_CONVERT);
	AddSetting(wxT("save_settings_on_exit"), wxT("Save Settings On Exit"), &CatapultPage::UpdateMenuSetting, 0);
	AddSetting(wxT("printerlogfilename"), wxT("PrinterLogFile"),&CatapultPage::UpdateIndicatorSetting, S_CONVERT);
}

void CatapultPage::AddSetting(
	wxString setting, wxString controlname,
	bool (CatapultPage::*pfunction)(wxString, wxString, wxString, int),
	int flags)
{
	if (m_settingTableSize >= SETTINGTABLE_MAXSIZE) {
		wxMessageBox(wxT("Not enough space to store the setting table!\nPlease contact the authors."),
		             wxT("Internal Catapult Error"));
		return;
	}
	m_settingTable[m_settingTableSize].setting = setting;
	m_settingTable[m_settingTableSize].controlname = controlname;
	m_settingTable[m_settingTableSize].pfunction = pfunction;
	m_settingTable[m_settingTableSize].flags = flags;
	m_settingTableSize++;
}

static wxString removeBackslash(const wxString& s)
{
	wxString result;
	int len = s.Length();
	for (int i = 0; i < len; ++i) {
		if (s[i] != '\\') {
			result += s[i];
		}
	}
	return result;
}
void CatapultPage::UpdateSetting(wxString name, wxString data)
{
	name = removeBackslash(name); // !!! HACK !!!
	int index = 0;
	bool found = false;
	while ((!found) && (index < SETTINGTABLE_MAXSIZE)) {
		if (name.Matches(m_settingTable[index].setting.c_str())) {
			found = true;
			if (m_settingTable[index].pfunction != NULL) {
				(*this.*(m_settingTable[index].pfunction))(
					name, data, m_settingTable[index].controlname,
					m_settingTable[index].flags);
			}
		}
		++index;
	}
}

bool CatapultPage::UpdateToggleSetting(wxString setting, wxString data, wxString control, int flags)
{
	bool value[] = {false, true};
	int active = 1;
	int sendvalue;

	if (flags & S_INVERT) {
		active = 0;
	}

	if (auto* button = (wxToggleButton*)m_parent->FindWindowByName(control)) {
		if ((data == wxT("on")) || (data == wxT("true")) || (data == wxT("1")) || (data == wxT("yes"))) {
			sendvalue = active;
		} else {
			sendvalue = 1 - active;

		}
		button->SetValue(value[sendvalue]);

		if (flags & S_EVENT) {
			wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, button->GetId());
			event.SetInt(active);
			event.SetEventObject(button);
			button->ProcessEvent(event);
		}

		if (flags & S_CONVERT) {
			if ((data == wxT("false")) || (data == wxT("no")) || (data == wxT("0"))) {
				data = wxT("off");
			}
			if ((data == wxT("true")) || (data == wxT("yes")) || (data == wxT("1"))) {
				data = wxT("on");
			}
			button->SetLabel(data.Mid(0, 1).Upper() + data.Mid(1).Lower());
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateComboSetting(wxString setting, wxString data, wxString control, int flags)
{
	wxString valuetext = data;
	if (flags & S_CONVERT) {
		valuetext = data.Mid(0, 1).Upper() + data.Mid(1).Lower();
	}
	if (auto* box = (wxComboBox*)m_parent->FindWindowByName(control)) {
		if (box->GetWindowStyle() & wxCB_READONLY) {
			box->SetSelection(box->FindString(valuetext));
		} else {
			box->SetValue(valuetext);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateIndicatorSetting(wxString setting, wxString data, wxString control, int flags)
{
	if (auto* indicator = (wxTextCtrl*)m_parent->FindWindowByName(control)) {
		wxString tempData = data;
		if (flags & S_CONVERT) {
			tempData = ConvertPath(data, true, true);
		}
		if (indicator->GetValue() != tempData) {
			indicator->SetValue(tempData);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateSliderSetting(wxString setting, wxString data, wxString control, int flags)
{
	if (auto* slider = (wxSlider *)m_parent->FindWindowByName(control)) {
		long value;
		data.ToLong(&value, 10);
		if (slider->GetValue() != value) {
			slider->SetValue(value);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateMenuSetting(wxString setting, wxString data, wxString control, int flags)
{
	bool sendvalue = false;
	auto* appwnd = (wxFrame*)GetParent()->GetGrandParent();
	int menusetting = appwnd->GetMenuBar()->FindMenuItem(wxT("&Settings"), wxT("Save openMSX Settings On &Exit"));
	if (menusetting != wxNOT_FOUND) {
		if ((data == wxT("on")) || (data == wxT("true")) || (data == wxT("1")) || (data== wxT("yes"))) {
			sendvalue = true;
		}
		appwnd->GetMenuBar()->Check(menusetting, sendvalue);
	}
	return false;
}

bool CatapultPage::UpdateAudioSetting(wxString setting, wxString data, wxString selection, int flags)
{
	auto* notebook = (wxNotebook*)m_parent;
	AudioControlPage* audiopage = nullptr;
	for (unsigned i = 0; i < notebook->GetPageCount(); ++i) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage*)notebook->GetPage(i);
			break;
		}
	}

	wxString channel = setting.Mid(0, setting.Length() - selection.Length() - 1);
	if (selection == wxT("volume")) {
		audiopage->SetChannelVolume(channel, data);
	} else {
		assert(selection == wxT("balance"));
		audiopage->SetChannelMode(channel, data);
	}
	return true;
}

bool CatapultPage::UpdateMidiPlug(wxString connector, wxString data, wxString control, int flags)
{
	auto* notebook = (wxNotebook*)m_parent;
	AudioControlPage* audiopage = nullptr;
	for (unsigned int i = 0; i < notebook->GetPageCount(); ++i) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage*)notebook->GetPage(i);
			break;
		}
	}
	audiopage->UpdateMidiPlug(control, data);
	return true;
}

bool CatapultPage::UpdatePluggable(wxString connector, wxString data, wxString control, int flags)
{
	if (auto* box = (wxComboBox*)m_parent->FindWindowByName(control)) {
		box->SetValue(data.IsEmpty() ? wxT("--empty--") : data);
	}
	return true;
}
