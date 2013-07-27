#ifndef FULLSCREENDLG_H
#define FULLSCREENDLG_H

class FullScreenDlg : public wxDialog
{
public:
	FullScreenDlg(wxWindow * parent = NULL);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	virtual ~FullScreenDlg();

private:
	DECLARE_CLASS(FullScreenDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
