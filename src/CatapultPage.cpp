// $Id: CatapultPage.cpp,v 1.2 2004/03/25 19:30:12 h_oudejans Exp $
// CatapultPage.cpp: implementation of the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CatapultPage.h"

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

}

wxString CatapultPage::ConvertPath(wxString path, bool ConvertSlash)
{
	path.Prepend(_("\""));
	path.Append(_("\""));
	if (ConvertSlash)
		path.Replace(_("\\"),_("/"),true);
	return path;
}

void CatapultPage::InitSettingsTable ()
{
	// this is an unshielded array, please add elements if the
	// number of settings increase
	m_settingsTable = new SettingTableElementType [21];
	AddSetting(0,"renderer","RendererSelector",&CatapultPage::UpdateComboSetting,false);
	AddSetting(1,"scaler","ScalerSelector",&CatapultPage::UpdateComboSetting,false);
	AddSetting(2,"accuracy","AccuracySelector",&CatapultPage::UpdateComboSetting,true);
	AddSetting(3,"deinterlace","DeInterlaceButton",&CatapultPage::UpdateToggleSetting,true);
	AddSetting(4,"limitsprites","LimitSpriteButton",&CatapultPage::UpdateToggleSetting,true);
	AddSetting(5,"blur","BlurIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(6,"glow","GlowIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(7,"gamma","GammaIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(8,"scanline","ScanlineIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(9,"speed","SpeedIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(10,"frameskip","FrameSkipIndicator",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(11,"throttle","ThrottleButton",&CatapultPage::UpdateToggleSetting,true);
	AddSetting(12,"cmdtiming","CmdTimingButton",&CatapultPage::UpdateToggleSetting,true);
	AddSetting(13,"power","PowerButton",&CatapultPage::UpdateToggleSetting,false);
	AddSetting(14,"pause","PauseButton",&CatapultPage::UpdateToggleSetting,false);
	AddSetting(15,"frontswitch","FirmwareButton",&CatapultPage::UpdateToggleSetting,false);
	AddSetting(16,"mute","MuteButton",&CatapultPage::UpdateToggleSetting,false);
	AddSetting(17,"midi-in-readfilename","MidiInFileInput",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(18,"midi-out-logfilename","MidiOutFileInput",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(19,"audio-inputfilename","SampleFileInput",&CatapultPage::UpdateIndicatorSetting,false);
	AddSetting(20,"","",NULL,NULL); // end of table
}

void CatapultPage::AddSetting (int number,wxString setting, wxString controlname,
	bool (CatapultPage::*pfunction)(wxString,wxString,wxString,bool),
	bool convert)
{
	m_settingsTable[number].setting = setting;
	m_settingsTable[number].controlname = controlname;
	m_settingsTable[number].pfunction = pfunction;
	m_settingsTable[number].convert = convert;
}

void CatapultPage::UpdateSetting(wxString name, wxString data)
{
	int index = 0;
	bool found = false;
	while ((!found) && (m_settingsTable[index].setting != "")){
		if (m_settingsTable[index].setting == name){
			found = true;
			if (m_settingsTable[index].pfunction != NULL){
				(*this.*(m_settingsTable[index].pfunction))(
					name, data, m_settingsTable[index].controlname,
					m_settingsTable[index].convert);
			}			
		}
	index ++;
	}
}

bool CatapultPage::UpdateToggleSetting(wxString setting, wxString data, wxString control, bool convert)
{
	wxString ButtonText;
	wxToggleButton * button = (wxToggleButton *)m_parent->FindWindow(control);
	if (button != NULL){
		if ((data == "on") || (data == "broken")){
			button->SetValue(true);
		}
		else{
			button->SetValue(false);
		}
		if (convert){
			ButtonText = data.Mid(0,1).Upper() + data.Mid(1).Lower();
			button->SetLabel(ButtonText);
		}
		return true;
	}
	return false;
}

bool CatapultPage::UpdateComboSetting(wxString setting, wxString data, wxString control, bool convert)
{
	wxString valuetext = data;
	if (convert){
		valuetext = data.Mid(0,1).Upper() + data.Mid(1).Lower();
	}
	wxComboBox * box = (wxComboBox *)m_parent->FindWindow(control);
	if (box != NULL){
		box->SetValue(valuetext);
		return true;
	}
	return false;
}

bool CatapultPage::UpdateIndicatorSetting(wxString setting, wxString data, wxString control, bool dummy)
{
	wxTextCtrl * indicator = (wxTextCtrl *)m_parent->FindWindow(control);
	if (indicator != NULL){
		if (indicator->GetValue() != data)
			indicator->SetValue(data);
			printf("test");
		return true;
	}
	return false;
}
