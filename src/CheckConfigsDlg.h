// CheckConfigsDlg.h: interface for the CheckConfigsDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AUDITDLG_H_INCLUDED_)
#define AUDITDLG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MSGID_TESTCONFIG_SUCCES 1
#define MSGID_TESTCONFIG_FAILED 2

class CheckConfigsDlg : public wxDialog  
{
public:
	CheckConfigsDlg(wxWindow * parent = NULL);
	void OnUserButton(wxCommandEvent& event);
	virtual ~CheckConfigsDlg();
	int ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions);
	void UpdateStats (bool checkmachine, bool succes, int progress);
	void FinishCheck ();
	void SetCurrentObject (wxString object);
private:
	class CheckConfigsThread : public wxThread  
	{
	public:
		CheckConfigsThread(CheckConfigsDlg * target);
		virtual ~CheckConfigsThread();
		wxThread::ExitCode Entry();
		void SetParameters (wxString cmd, wxArrayString * machines, wxArrayString * extensions);
		bool m_abort;
		
	private:
		bool doCheckConfigs (wxString cmd);
		
		CheckConfigsDlg * m_target;
		wxString m_cmd;
		wxArrayString * m_machines;
		wxArrayString * m_extensions;
		wxString m_workingmachine;
	};
	int m_validmachinecount;
	int m_invalidmachinecount;
	int m_validextensioncount;
	int m_invalidextensioncount;
	
	CheckConfigsThread * m_auditThread;
	wxStaticText * m_completemachines;
	wxStaticText * m_incompletemachines;
	wxStaticText * m_workingextensions;
	wxStaticText * m_nonworkingextensions;
	wxStaticText * m_currentconfig;
	wxGauge * m_progressbar;
	wxButton * m_userbutton;
	wxTextCtrl * m_log;
	wxString m_currentObject;

	DECLARE_CLASS(CheckConfigsDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // !defined(AUDITDLG_H_INCLUDED_)
