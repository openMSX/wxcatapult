#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include "CatapultPage.h"

class wxTextCtrl;

class StatusPage : public CatapultPage
{
public:
	StatusPage(wxWindow* parent);

	void Clear();
	void Add(const wxColour& col, const wxString& str);

private:
	wxTextCtrl* m_outputtext;

	DECLARE_CLASS(StatusPage)
	DECLARE_EVENT_TABLE()
};

#endif
