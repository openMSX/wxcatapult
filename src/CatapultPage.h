// $Id: CatapultPage.h,v 1.3 2004/03/26 20:02:06 h_oudejans Exp $
// CatapultPage.h: interface for the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/tglbtn.h>
#define SETTINGTABLE_MAXSIZE 22

class CatapultPage : public wxPanel 
{
public:
	CatapultPage(wxWindow * parent=NULL);
	virtual ~CatapultPage();
	void UpdateSetting(wxString name, wxString data);
	void OnClickCombo (wxCommandEvent &event);
	virtual void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)=0;
protected:
	wxString ConvertPath (wxString path, bool ConvertSlash=false);

private:
	struct SettingTableElementType{
		wxString setting;
		wxString controlname;
		bool (CatapultPage::*pfunction)(wxString,wxString,wxString,bool);
		bool convert;
	};
	
	SettingTableElementType * m_settingTable;
	int m_settingTableSize;
	void InitSettingsTable ();
	void AddSetting (wxString settings, wxString controlname,
		bool (CatapultPage::*pfunction)(wxString,wxString,wxString,bool),
		bool convert);
	wxWindow * m_parent;
	bool UpdateToggleSetting(wxString setting, wxString data, wxString control, bool convert);
	bool UpdateComboSetting(wxString setting, wxString data, wxString control, bool convert);
	bool UpdateIndicatorSetting(wxString setting, wxString data, wxString control, bool dummy);
	bool UpdateAudioSetting (wxString setting, wxString data, wxString selection, bool dummy2);
	
};

#endif 
