#ifndef SCREENSHOTDLG_H
#define SCREENSHOTDLG_H

#include <wx/dialog.h>

class ScreenshotDlg : public wxDialog
{
public:
	ScreenshotDlg(wxWindow* parent);

private:
	void OnOk(wxCommandEvent& event);

	DECLARE_CLASS(ScreenshotDlg)
	DECLARE_EVENT_TABLE()
};

#endif
