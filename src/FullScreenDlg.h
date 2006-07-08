// FullScreenDlg.h: interface for the FullScreenDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCREENDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_)
#define AFX_FULLSCREENDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(AFX_FULLSCREENDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_)
