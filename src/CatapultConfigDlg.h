#ifndef CATAPULTCONFIGDLG_H
#define CATAPULTCONFIGDLG_H

#include <wx/dialog.h>
#include <wx/textctrl.h>

class openMSXController;
class wxCheckBox;

class CatapultConfigDlg : public wxDialog
{
public:
	CatapultConfigDlg(wxWindow* parent, openMSXController& controller);

private:
	void OnBrowseExec(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);

	wxTextCtrl* m_ExecPath;
	wxCheckBox* m_ConfigCheck;
	wxWindow* m_parent;
	openMSXController& m_controller;

	DECLARE_CLASS(CatapultConfigDlg)
	DECLARE_EVENT_TABLE()
};

#endif
