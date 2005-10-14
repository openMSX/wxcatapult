// AuditDlg.h: interface for the AuditDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AUDITDLG_H_INCLUDED_)
#define AUDITDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MSGID_TESTCONFIG_SUCCES 1
#define MSGID_TESTCONFIG_FAILED 2

class AuditDlg : public wxDialog  
{
public:
	AuditDlg(wxWindow * parent = NULL);
	void OnUserButton(wxCommandEvent& event);
	virtual ~AuditDlg();
	int ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions);
	void UpdateStats (bool checkmachine, bool succes, int progress);
	void FinishCheck ();
	void SetCurrentObject (wxString object);
private:
	class AuditThread : public wxThread  
	{
	public:
		AuditThread(AuditDlg * target);
		virtual ~AuditThread();
		wxThread::ExitCode Entry();
		void SetParameters (wxString cmd, wxArrayString * machines, wxArrayString * extensions);
		bool m_abort;
		
	private:
		bool doAudit (wxString cmd, bool checkmachine, int progress);
		
		AuditDlg * m_target;
		wxString m_cmd;
		wxArrayString * m_machines;
		wxArrayString * m_extensions;
		wxString m_workingmachine;
	};
	int m_validmachinecount;
	int m_invalidmachinecount;
	int m_validextensioncount;
	int m_invalidextensioncount;
	
	AuditThread * m_auditThread;
	wxStaticText * m_completemachines;
	wxStaticText * m_incompletemachines;
	wxStaticText * m_workingextensions;
	wxStaticText * m_nonworkingextensions;
	wxStaticText * m_currentconfig;
	wxGauge * m_progressbar;
	wxButton * m_userbutton;
	wxTextCtrl * m_log;
	wxString m_currentObject;

	DECLARE_CLASS(AuditDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // !defined(AUDITDLG_H_INCLUDED_)
