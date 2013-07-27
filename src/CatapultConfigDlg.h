#ifndef CATAPULTCONFIGDLG_H
#define CATAPULTCONFIGDLG_H

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

#endif
