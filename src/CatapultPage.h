#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#include <wx/panel.h>
#include <wx/tglbtn.h>

class CatapultPage : public wxPanel
{
public:
	CatapultPage(wxWindow* parent = nullptr);
	virtual ~CatapultPage();

	void UpdateSetting(wxString name, wxString data);
	void OnClickCombo (wxCommandEvent& event);
	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) = 0;
	wxString ConvertPath(wxString path, bool convertSlash = false, bool backwards = false); // need to move somewhere else

private:
	void InitSettingsTable();
	void AddSetting(
		wxString settings, wxString controlname,
		void (CatapultPage::*pfunction)(wxString, wxString, wxString, int),
		int flags);
	void UpdateToggleSetting   (wxString setting,   wxString data, wxString control,   int flags);
	void UpdateComboSetting    (wxString setting,   wxString data, wxString control,   int flags);
	void UpdateIndicatorSetting(wxString setting,   wxString data, wxString control,   int flags);
	void UpdateSliderSetting   (wxString setting,   wxString data, wxString control,   int flags);
	void UpdateAudioSetting    (wxString setting,   wxString data, wxString selection, int flags);
	void UpdateMenuSetting     (wxString setting,   wxString data, wxString selection, int flags);
	void UpdateMidiPlug        (wxString connector, wxString data, wxString control,   int flags);
	void UpdatePluggable       (wxString connector, wxString data, wxString control,   int flags);

	struct SettingTableElementType {
		wxString setting;
		wxString controlname;
		void (CatapultPage::*pfunction)(wxString, wxString, wxString, int);
		int flags;
	};

	wxWindow* m_parent;
	SettingTableElementType* m_settingTable;
	int m_settingTableSize;
};

#endif
