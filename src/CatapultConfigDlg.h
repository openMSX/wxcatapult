// $Id$
// CatapultConfigDlg.h: interface for the CatapultConfigDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATAPULTCONFIGDLG_H__4C970B22_3068_11D8_B9CD_00104B4B187E__INCLUDED_)
#define AFX_CATAPULTCONFIGDLG_H__4C970B22_3068_11D8_B9CD_00104B4B187E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class openMSXController;

class CatapultConfigDlg : public wxDialog
{
	public:
		void OnBrowseExec (wxCommandEvent& event);
		void OnBrowseShare (wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnOk(wxCommandEvent& event);
		CatapultConfigDlg(wxWindow * parent, openMSXController* controller);
		virtual ~CatapultConfigDlg();

	private:
		wxTextCtrl * m_ExecPath;
		wxTextCtrl * m_SharePath;
		wxCheckBox * m_ConfigCheck;
		wxWindow * m_parent;
                openMSXController * m_controller;

		DECLARE_CLASS(CatapultConfigDlg)
		// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_CATAPULTCONFIGDLG_H__4C970B22_3068_11D8_B9CD_00104B4B187E__INCLUDED_)
