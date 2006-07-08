// $Id: CatapultPage.h,v 1.15 2005/05/14 11:17:12 h_oudejans Exp $
// CatapultPage.h: interface for the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/tglbtn.h>
#define SETTINGTABLE_MAXSIZE 37

#define S_CONVERT 1
#define S_EVENT 2
#define S_INVERT 4

class CatapultPage : public wxPanel
{
public:
	CatapultPage(wxWindow * parent=NULL);
	virtual ~CatapultPage();
	void UpdateSetting(wxString name, wxString data);
	void OnClickCombo (wxCommandEvent &event);
	virtual void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)=0;
	wxString ConvertPath (wxString path, bool ConvertSlash=false, bool Backwards=false); // need to move somewhere else
protected:


private:
	struct SettingTableElementType{
		wxString setting;
		wxString controlname;
		bool (CatapultPage::*pfunction)(wxString,wxString,wxString,int);
		int flags;
	};

	SettingTableElementType * m_settingTable;
	int m_settingTableSize;
	void InitSettingsTable ();
	void AddSetting (wxString settings, wxString controlname,
		bool (CatapultPage::*pfunction)(wxString,wxString,wxString,int),
		int flags);
	wxWindow * m_parent;
	bool UpdateToggleSetting(wxString setting, wxString data, wxString control, int flags);
	bool UpdateComboSetting(wxString setting, wxString data, wxString control, int flags);
	bool UpdateIndicatorSetting(wxString setting, wxString data, wxString control, int flags);
	bool UpdateSliderSetting (wxString setting, wxString data, wxString control, int flags);
	bool UpdateAudioSetting (wxString setting, wxString data, wxString selection, int flags);
	bool UpdateMenuSetting (wxString setting, wxString data, wxString selection, int flags);
	bool UpdateMidiPlug (wxString connector, wxString data, wxString control, int flags);
	bool UpdatePluggable (wxString connector, wxString data, wxString control, int flags);
};

#endif
