#ifndef SCREENSHOTDLG_H
#define SCREENSHOTDLG_H

#include <wx/dialog.h>

class ScreenshotDlg : public wxDialog
{
public:
	ScreenshotDlg(wxWindow* parent = nullptr);

	void OnOk(wxCommandEvent& event);

private:
	DECLARE_CLASS(ScreenshotDlg)
	DECLARE_EVENT_TABLE()
};

#endif
