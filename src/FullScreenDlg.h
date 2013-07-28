#ifndef FULLSCREENDLG_H
#define FULLSCREENDLG_H

#include <wx/dialog.h>

class FullScreenDlg : public wxDialog
{
public:
	FullScreenDlg(wxWindow* parent = nullptr);

	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);

private:
	DECLARE_CLASS(FullScreenDlg)
	DECLARE_EVENT_TABLE()
};

#endif
