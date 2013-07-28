#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <wx/dialog.h>

class AboutDlg : public wxDialog
{
public:
	AboutDlg(wxWindow* parent);
	void OnClose(wxCommandEvent& event);

private:
	DECLARE_CLASS(AboutDlg)
	DECLARE_EVENT_TABLE()
};

#endif
