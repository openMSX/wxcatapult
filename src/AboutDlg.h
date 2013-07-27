#ifndef ABOUTDLG_H
#define ABOUTDLG_H

class AboutDlg : public wxDialog
{
public:
	AboutDlg(wxWindow * parent = NULL);
	void OnClose(wxCommandEvent& event);
	virtual ~AboutDlg();

private:
	DECLARE_CLASS(AboutDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
