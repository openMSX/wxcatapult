#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#include <wx/panel.h>

class CatapultPage : public wxPanel
{
public:
	void OnClickCombo(wxCommandEvent& event);
	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) = 0;

	// Need to move to some utility class.
	static wxString ConvertPath(wxString path, bool convertSlash = false, bool backwards = false);
};

#endif
