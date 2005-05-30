// FullScreenDlg.h: interface for the FullScreenDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTDLG_H_INCLUDED_)
#define AFX_ABOUTDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_ABOUTDLG_H_INCLUDED_)
