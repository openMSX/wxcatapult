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
	void OnTestConfigEvent (wxCommandEvent & event);
	void HandleUpdateStats(bool checkmachine, bool succes, int progress);
	void HandleSetCurrentObject(wxString object);
	virtual ~CheckConfigsDlg();
	int ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions);
	void EndModal(int retCode);
	void FinishCheck ();	
private:
	struct CheckConfigsData{
		bool m_checkmachine;
		bool m_succes;
		int  m_progress;
		wxString m_currentObject;
	};
	class CheckConfigsThread : public wxThread  
	{
	public:
		CheckConfigsThread(CheckConfigsDlg * target);
		virtual ~CheckConfigsThread();
		wxThread::ExitCode Entry();
		void SetParameters (wxString cmd, wxArrayString * machines, wxArrayString * extensions);
		bool m_running;
		
	private:
		bool doCheckConfigs (wxString cmd);
		void UpdateStats (bool checkmachine, bool succes, int progress);
		void SetCurrentObject (wxString object);
		
		CheckConfigsDlg * m_target;
		wxString m_cmd;
		wxArrayString * m_machines;
		wxArrayString * m_extensions;
		wxString m_workingmachine;
	};
	friend class CheckConfigsThread;
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
