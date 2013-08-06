#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#include <wx/panel.h>

class CatapultPage : public wxPanel
{
public:
	void OnClickCombo(wxCommandEvent& event);
	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) {}
};

#endif
