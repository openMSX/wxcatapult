#ifndef CATAPULTCONFIGDLG_H
#define CATAPULTCONFIGDLG_H

#include <wx/dialog.h>

class openMSXController;
class wxTextCtrl;
class wxCheckBox;

class CatapultConfigDlg : public wxDialog
{
public:
	CatapultConfigDlg(wxWindow* parent, openMSXController* controller);

private:
	void OnBrowseExec(wxCommandEvent& event);
	void OnBrowseShare(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);

	wxTextCtrl* m_ExecPath;
	wxTextCtrl* m_SharePath;
	wxCheckBox* m_ConfigCheck;
	wxWindow* m_parent;
	openMSXController* m_controller;

	DECLARE_CLASS(CatapultConfigDlg)
	DECLARE_EVENT_TABLE()
};

#endif
