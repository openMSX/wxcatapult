// $Id: CatapultPage.cpp,v 1.25 2004/10/08 15:26:03 h_oudejans Exp $
// CatapultPage.cpp: implementation of the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "CatapultPage.h"
#include <wx/notebook.h>
#include "AudioControlPage.h"
#include "VideoControlPage.h"
#include "ConfigurationData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CatapultPage::CatapultPage(wxWindow * parent)
{
	m_parent = parent;
	InitSettingsTable();
}

CatapultPage::~CatapultPage()
{
	delete [] m_settingTable;
}

wxString CatapultPage::ConvertPath(wxString path, bool ConvertSlash)
{
	path.Prepend(wxT("\""));
	path.Append(wxT("\""));
	if (ConvertSlash)
		path.Replace(wxT("\\"),wxT("/"),true);
	return path;
}

void CatapultPage::OnClickCombo (wxCommandEvent &event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString sel = box->GetString(box->GetSelection());
	wxString cursel = box->GetValue();
	if (sel != cursel) {
		box->SetValue(sel);
	}
}

void CatapultPage::InitSettingsTable ()
{
	m_settingTableSize = 0;
	m_settingTable = new SettingTableElementType [SETTINGTABLE_MAXSIZE];
	AddSetting("renderer","RendererSelector",&CatapultPage::UpdateComboSetting,0);
	AddSetting("scaler","ScalerSelector",&CatapultPage::UpdateComboSetting,0);
	AddSetting("accuracy","AccuracySelector",&CatapultPage::UpdateComboSetting,0);
	AddSetting("deinterlace","DeInterlaceButton",&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting("limitsprites","LimitSpriteButton",&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting("blur","BlurIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("glow","GlowIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("gamma","GammaIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("scanline","ScanlineIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("speed","SpeedIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("minframeskip","MinFrameSkipIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("maxframeskip","MaxFrameSkipIndicator",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("throttle","MaxSpeedButton",&CatapultPage::UpdateToggleSetting,S_INVERT | S_EVENT);
	AddSetting("power","PowerButton",&CatapultPage::UpdateToggleSetting,0);
	AddSetting("pause","PauseButton",&CatapultPage::UpdateToggleSetting,0);
	AddSetting("frontswitch","FirmwareButton",&CatapultPage::UpdateToggleSetting,0);
	AddSetting("firmwareswitch","FirmwareButton",&CatapultPage::UpdateToggleSetting,0);
	AddSetting("mute","MuteButton",&CatapultPage::UpdateToggleSetting,0);
	AddSetting("midi-in-readfilename","MidiInFileInput",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("midi-out-logfilename","MidiOutFileInput",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("audio-inputfilename","SampleFileInput",&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting("*_volume","volume",&CatapultPage::UpdateAudioSetting,0);
	AddSetting("*_mode","mode",&CatapultPage::UpdateAudioSetting,0);
	AddSetting("msx-midi-in","MidiInSelector",&CatapultPage::UpdateMidiPlug,0);
	AddSetting("msx-midi-out","MidiOutSelector",&CatapultPage::UpdateMidiPlug,0);
	AddSetting("pcminput","SampleInSelector",&CatapultPage::UpdatePluggable,0);
	AddSetting("joyporta","Joyport1Selector",&CatapultPage::UpdatePluggable,0);
	AddSetting("joyportb","Joyport2Selector",&CatapultPage::UpdatePluggable,0);
	AddSetting("renshaturbo","RenshaTurboSlider",&CatapultPage::UpdateSliderSetting,0);
	AddSetting("diska","DiskAContents",&CatapultPage::UpdateComboSetting,0);
	AddSetting("diskb","DiskBContents",&CatapultPage::UpdateComboSetting,0);
	AddSetting("tape1","Tape1Contents",&CatapultPage::UpdateComboSetting,0);
	AddSetting("tape2","Tape2Contents",&CatapultPage::UpdateComboSetting,0);
	AddSetting("fullscreen","FullScreenButton",&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting("save_settings_on_exit","Save Settings On Exit",&CatapultPage::UpdateMenuSetting,0);
	AddSetting("printerlogfilename","PrinterLogFile",&CatapultPage::UpdateIndicatorSetting,0);
}

void CatapultPage::AddSetting (wxString setting, wxString controlname,
	bool (CatapultPage::*pfunction)(wxString,wxString,wxString,int),
	int flags)
{	
	if (m_settingTableSize >= SETTINGTABLE_MAXSIZE) {
		wxMessageBox ("Not enough space to store the setting table!\nPlease contact the authors.","Internal Catapult Error");
		return;
	}
	m_settingTable[m_settingTableSize].setting = setting;
	m_settingTable[m_settingTableSize].controlname = controlname;
	m_settingTable[m_settingTableSize].pfunction = pfunction;
	m_settingTable[m_settingTableSize].flags = flags;
	m_settingTableSize ++;
}

void CatapultPage::UpdateSetting(wxString name, wxString data)
{
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
	index ++;
	}
}

bool CatapultPage::UpdateToggleSetting(wxString setting, wxString data, wxString control, int flags)
{
	bool value[]={false,true};
	int active = 1;
	int sendvalue;


	wxString ButtonText;
	if (flags & S_INVERT){
		active = 0;
	}
	
	wxToggleButton * button = (wxToggleButton *)m_parent->FindWindow(control);
	if (button != NULL) {
		if ((data == "on") || (data == "true") || (data == "1") || (data== "yes")) {
			sendvalue = active;
		}
		else{
			sendvalue = 1-active;
			
		}
		button->SetValue(value[sendvalue]);	
		
		if (flags & S_EVENT){
			wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,button->GetId());
			event.SetInt(active);
			event.SetEventObject(button);
			button->ProcessEvent(event);
		}
					
		if (flags & S_CONVERT) {
			if ((data == "false") || (data == "no") || (data == "0")){
				data = "off";
			}
			if ((data == "true") || (data == "yes") || (data == "1")){
				data = "on";
			}
			ButtonText = data.Mid(0,1).Upper() + data.Mid(1).Lower();
			button->SetLabel(ButtonText);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateComboSetting(wxString setting, wxString data, wxString control, int flags)
{
	wxString valuetext = data;
	if (flags & S_CONVERT) {
		valuetext = data.Mid(0,1).Upper() + data.Mid(1).Lower();
	}
	wxComboBox * box = (wxComboBox *)m_parent->FindWindow(control);
	if (box != NULL) {
		if (box->GetWindowStyle() & wxCB_READONLY) {
			box->SetSelection(box->FindString(valuetext));
		}
		else{
			box->SetValue(valuetext);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateIndicatorSetting(wxString setting, wxString data, wxString control, int flags)
{
	wxTextCtrl * indicator = (wxTextCtrl *)m_parent->FindWindow(control);
	if (indicator != NULL) {
		if (indicator->GetValue() != data)
			indicator->SetValue(data);
		return true;
	}
	return false;
}

bool CatapultPage::UpdateSliderSetting(wxString setting, wxString data, wxString control, int flags)
{
	long value;
	data.ToLong(&value,10);
	wxSlider * slider = (wxSlider *)m_parent->FindWindow(control);
	if (slider != NULL) {
		if (slider->GetValue() != value)
			slider->SetValue(value);
		return true;
	}
	return false;
}

bool CatapultPage::UpdateMenuSetting (wxString setting, wxString data, wxString control, int flags)
{
	bool sendvalue = false;
	long savedvalue = 0;
	wxFrame * appwnd = (wxFrame *)GetParent()->GetGrandParent();
	int menusetting = appwnd->GetMenuBar()->FindMenuItem("&Settings","Save openMSX Settings On &Exit");
	if (menusetting != wxNOT_FOUND) {
		if ((data == "on") || (data == "true") || (data == "1") || (data== "yes")) {
			sendvalue = true;
			savedvalue = 1;
		}
		appwnd->GetMenuBar()->Check(menusetting,sendvalue);
	}
	return false;
}

bool CatapultPage::UpdateAudioSetting (wxString setting, wxString data, wxString selection, int flags)
{
	int i;
	wxString slidertext;
	wxNotebook * notebook = (wxNotebook *) m_parent;
	AudioControlPage * audiopage = NULL;
	for (i=0;i<notebook->GetPageCount();i++) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage *)notebook->GetPage(i);
		}
	}
	for (i=0;i<audiopage->GetNumberOfAudioChannels();i++) {
		if ((audiopage->GetAudioChannelName(i) + wxT("_") + selection) == setting) {
			if (selection == wxT("volume")) {
				audiopage->SetChannelVolume(i,data);
			}
			else{
				audiopage->SetChannelMode(i,data);
			}
			return true;
		}
	}
	return false;
}

bool CatapultPage::UpdateMidiPlug (wxString connector, wxString data, wxString control, int flags)
{
	wxNotebook * notebook = (wxNotebook *) m_parent;
	AudioControlPage * audiopage = NULL;
	for (int i=0;i<notebook->GetPageCount();i++) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage *)notebook->GetPage(i);
		}
	}
	audiopage->UpdateMidiPlug (control, data);
	return true;
}

bool CatapultPage::UpdatePluggable (wxString connector, wxString data, wxString control, int flags)
{
	wxString valuetext = data;
	if (data == wxT("")) {
		valuetext = wxT("--empty--");
	}
	wxComboBox * box = (wxComboBox *)m_parent->FindWindow(control);
	if (box != NULL) {
		box->SetValue(valuetext);
	}
	return true;
}


