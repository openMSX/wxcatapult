#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <vector>

class CatapultPage : public wxPanel
{
public:
	CatapultPage(wxWindow* parent = nullptr);

	void UpdateSetting(const wxString& name, const wxString& data);
	void OnClickCombo(wxCommandEvent& event);

	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) = 0;

	static wxString ConvertPath(wxString path, bool convertSlash = false, bool backwards = false); // need to move somewhere else

private:
	void AddSetting(
		const wxString& settings, const wxString& control,
		void (CatapultPage::*pfunction)(const wxString&, const wxString&, const wxString&, int),
		int flags = 0);
	void UpdateToggle   (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateCombo    (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateIndicator(const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateSlider   (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateAudio    (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateMenu     (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateMidiPlug (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdatePluggable(const wxString& setting, const wxString& data, const wxString& control, int flags);

	struct SettingElement {
		wxString setting;
		wxString control;
		void (CatapultPage::*pfunction)(const wxString&, const wxString&, const wxString&, int);
		int flags;
	};

	wxWindow* m_parent;
	std::vector<SettingElement> m_settingTable;
};

#endif
