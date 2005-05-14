// $Id: CatapultPage.cpp,v 1.34 2005/01/06 16:27:21 h_oudejans Exp $
// CatapultPage.cpp: implementation of the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"
#include "wx/version.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "CatapultPage.h"
#include <wx/notebook.h>
#include "AudioControlPage.h"
#include "VideoControlPage.h"
#include "ConfigurationData.h"

#if wxCHECK_VERSION(2,5,0)
#define WX24SIGNCAST
#else
#define WX24SIGNCAST (unsigned int)
#endif

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

wxString CatapultPage::ConvertPath(wxString path, bool ConvertSlash, bool Backwards)
{
	if (Backwards){
		path.Replace(wxT("\""),wxT(""),true);
	}
	else{
		path.Prepend(wxT("\""));
		path.Append(wxT("\""));
	}
	if (ConvertSlash){
		if (Backwards){
			path.Replace(wxT("/"),wxT("\\"),true);
		}
		else{
			path.Replace(wxT("\\"),wxT("/"),true);
		}
	}
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
	AddSetting(wxT("renderer"),wxT("RendererSelector"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("scaler"),wxT("ScalerSelector"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("accuracy"),wxT("AccuracySelector"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("deinterlace"),wxT("DeInterlaceButton"),&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting(wxT("limitsprites"),wxT("LimitSpriteButton"),&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting(wxT("blur"),wxT("BlurIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("glow"),wxT("GlowIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("gamma"),wxT("GammaIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("scanline"),wxT("ScanlineIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("speed"),wxT("SpeedIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("minframeskip"),wxT("MinFrameSkipIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("maxframeskip"),wxT("MaxFrameSkipIndicator"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("throttle"),wxT("MaxSpeedButton"),&CatapultPage::UpdateToggleSetting,S_INVERT | S_EVENT);
	AddSetting(wxT("power"),wxT("PowerButton"),&CatapultPage::UpdateToggleSetting,0);
	AddSetting(wxT("pause"),wxT("PauseButton"),&CatapultPage::UpdateToggleSetting,0);
	AddSetting(wxT("frontswitch"),wxT("FirmwareButton"),&CatapultPage::UpdateToggleSetting,0);
	AddSetting(wxT("firmwareswitch"),wxT("FirmwareButton"),&CatapultPage::UpdateToggleSetting,0);
	AddSetting(wxT("mute"),wxT("MuteButton"),&CatapultPage::UpdateToggleSetting,0);
	AddSetting(wxT("midi-in-readfilename"),wxT("MidiInFileInput"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("midi-out-logfilename"),wxT("MidiOutFileInput"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("audio-inputfilename"),wxT("SampleFileInput"),&CatapultPage::UpdateIndicatorSetting,0);
	AddSetting(wxT("*_volume"),wxT("volume"),&CatapultPage::UpdateAudioSetting,0);
	AddSetting(wxT("*_mode"),wxT("mode"),&CatapultPage::UpdateAudioSetting,0);
	AddSetting(wxT("msx-midi-in"),wxT("MidiInSelector"),&CatapultPage::UpdateMidiPlug,0);
	AddSetting(wxT("msx-midi-out"),wxT("MidiOutSelector"),&CatapultPage::UpdateMidiPlug,0);
	AddSetting(wxT("pcminput"),wxT("SampleInSelector"),&CatapultPage::UpdatePluggable,0);
	AddSetting(wxT("joyporta"),wxT("Joyport1Selector"),&CatapultPage::UpdatePluggable,0);
	AddSetting(wxT("joyportb"),wxT("Joyport2Selector"),&CatapultPage::UpdatePluggable,0);
	AddSetting(wxT("printerport"),wxT("PrinterportSelector"),&CatapultPage::UpdatePluggable,0);
	AddSetting(wxT("renshaturbo"),wxT("RenshaTurboSlider"),&CatapultPage::UpdateSliderSetting,0);
	AddSetting(wxT("diska"),wxT("DiskAContents"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("diskb"),wxT("DiskBContents"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("cassette"),wxT("CassetteContents"),&CatapultPage::UpdateComboSetting,0);
	AddSetting(wxT("fullscreen"),wxT("FullScreenButton"),&CatapultPage::UpdateToggleSetting,S_CONVERT);
	AddSetting(wxT("save_settings_on_exit"),wxT("Save Settings On Exit"),&CatapultPage::UpdateMenuSetting,0);
	AddSetting(wxT("printerlogfilename"),wxT("PrinterLogFile"),&CatapultPage::UpdateIndicatorSetting,S_CONVERT);
}

void CatapultPage::AddSetting (wxString setting, wxString controlname,
	bool (CatapultPage::*pfunction)(wxString,wxString,wxString,int),
	int flags)
{	
	if (m_settingTableSize >= SETTINGTABLE_MAXSIZE) {
		wxMessageBox (wxT("Not enough space to store the setting table!\nPlease contact the authors."),wxT("Internal Catapult Error"));
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
	
	wxToggleButton * button = (wxToggleButton *)m_parent->FindWindowByName(control);
	if (button != NULL) {
		if ((data == wxT("on")) || (data == wxT("true")) || (data == wxT("1")) || (data== wxT("yes"))) {
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
			if ((data == wxT("false")) || (data == wxT("no")) || (data == wxT("0"))){
				data = wxT("off");
			}
			if ((data == wxT("true")) || (data == wxT("yes")) || (data == wxT("1"))){
				data = wxT("on");
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
	wxComboBox * box = (wxComboBox *)m_parent->FindWindowByName(control);
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
	wxString tempData = data;
	wxTextCtrl * indicator = (wxTextCtrl *)m_parent->FindWindowByName(control);
	if (indicator != NULL) {
		if (flags & S_CONVERT)
		{
			tempData = ConvertPath (data,true,true);
		}
		if (indicator->GetValue() != tempData)
			indicator->SetValue(tempData);
		return true;
	}
	return false;
}

bool CatapultPage::UpdateSliderSetting(wxString setting, wxString data, wxString control, int flags)
{
	long value;
	data.ToLong(&value,10);
	wxSlider * slider = (wxSlider *)m_parent->FindWindowByName(control);
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
	int menusetting = appwnd->GetMenuBar()->FindMenuItem(wxT("&Settings"),wxT("Save openMSX Settings On &Exit"));
	if (menusetting != wxNOT_FOUND) {
		if ((data == wxT("on")) || (data == wxT("true")) || (data == wxT("1")) || (data== wxT("yes"))) {
			sendvalue = true;
			savedvalue = 1;
		}
		appwnd->GetMenuBar()->Check(menusetting,sendvalue);
	}
	return false;
}

bool CatapultPage::UpdateAudioSetting (wxString setting, wxString data, wxString selection, int flags)
{
	unsigned int i;
	wxString slidertext;
	wxNotebook * notebook = (wxNotebook *) m_parent;
	AudioControlPage * audiopage = NULL;
	for (i=0;i< WX24SIGNCAST (notebook->GetPageCount());i++) {
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
	for (unsigned int i=0;i< WX24SIGNCAST notebook->GetPageCount();i++) {
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
	wxComboBox * box = (wxComboBox *)m_parent->FindWindowByName(control);
	if (box != NULL) {
		box->SetValue(valuetext);
	}
	return true;
}
