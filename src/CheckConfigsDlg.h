#ifndef CHECKCONFIGSDLG_H
#define CHECKCONFIGSDLG_H

#include <memory>
#include <wx/dialog.h>
#include <wx/gauge.h> // required here for windows

class wxStaticText;
class wxButton;
class wxTextCtrl;

class CheckConfigsDlg : public wxDialog
{
public:
	CheckConfigsDlg(wxWindow* parent);

	int Present(wxString cmd, wxArrayString& machines, wxArrayString& extensions);

private:
	void HandleUpdateStats(bool checkmachine, bool succes, int progress);
	void HandleSetCurrentObject(wxString object);
	void EndModal(int retCode);
	void FinishCheck();
	void OnUserButton(wxCommandEvent& event);
	void OnTestConfigEvent(wxCommandEvent& event);

	struct CheckConfigsData {
		bool m_checkmachine;
		bool m_succes;
		int  m_progress;
		wxString m_currentObject;
	};
	class CheckConfigsThread : public wxThread {
	public:
		CheckConfigsThread(CheckConfigsDlg* target);
		wxThread::ExitCode Entry();
		void SetParameters(wxString cmd, wxArrayString* machines, wxArrayString* extensions);
		bool m_running;

	private:
		bool doCheckConfigs(wxString cmd);
		void UpdateStats(bool checkmachine, bool succes, int progress);
		void SetCurrentObject(wxString object);

		CheckConfigsDlg* m_target;
		wxString m_cmd;
		wxArrayString* m_machines;
		wxArrayString* m_extensions;
	};
	friend class CheckConfigsThread;

	int m_validmachinecount;
	int m_invalidmachinecount;
	int m_validextensioncount;
	int m_invalidextensioncount;

	std::unique_ptr<CheckConfigsThread> m_auditThread;
	wxStaticText* m_completemachines;
	wxStaticText* m_incompletemachines;
	wxStaticText* m_workingextensions;
	wxStaticText* m_nonworkingextensions;
	wxStaticText* m_currentconfig;
	wxGauge* m_progressbar;
	wxButton* m_userbutton;
	wxTextCtrl* m_log;
	wxString m_currentObject;

	DECLARE_CLASS(CheckConfigsDlg)
	DECLARE_EVENT_TABLE()
};

#endif
