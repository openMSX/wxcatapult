// $Id$
// ScreenshotDlg.h: interface for the FullScreenDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCREENSHOTDLG_H)
#define AFX_SCREENSHOTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
