#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include "CatapultPage.h"

class wxTextCtrl;

class StatusPage : public CatapultPage
{
public:
	StatusPage(wxWindow* parent);
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) {};

	wxTextCtrl* m_outputtext;

private:
	DECLARE_CLASS(StatusPage)
	DECLARE_EVENT_TABLE()
};

#endif
