#ifndef SCREENSHOTDLG_H
#define SCREENSHOTDLG_H

class ScreenshotDlg : public wxDialog
{
public:
	ScreenshotDlg(wxWindow * parent = NULL);
	void OnOk(wxCommandEvent& event);
	virtual ~ScreenshotDlg();

private:
	DECLARE_CLASS(ScreenshotDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
