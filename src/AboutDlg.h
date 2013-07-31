#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <wx/dialog.h>

class AboutDlg : public wxDialog
{
public:
	AboutDlg(wxWindow* parent);

private:
	void OnClose(wxCommandEvent& event);

	DECLARE_CLASS(AboutDlg)
	DECLARE_EVENT_TABLE()
};

#endif
