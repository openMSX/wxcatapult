#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include "CatapultPage.h"

class wxTextCtrl;

class StatusPage : public CatapultPage
{
public:
	StatusPage(wxWindow* parent);

	wxTextCtrl* m_outputtext;

private:
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) {};

	DECLARE_CLASS(StatusPage)
	DECLARE_EVENT_TABLE()
};

#endif
