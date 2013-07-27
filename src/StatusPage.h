#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include "CatapultPage.h"

class StatusPage : public CatapultPage
{
public:
	StatusPage(wxWindow * parent = (wxWindow *)NULL);
	virtual ~StatusPage();
	wxTextCtrl * m_outputtext;
	void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus) {};

private:
	DECLARE_CLASS(StatusPage)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
