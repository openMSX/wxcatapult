// CheckConfigsDlg.cpp: implementation of the CheckConfigsDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxCatapultApp.h"
#include "CheckConfigsDlg.h"

IMPLEMENT_CLASS(CheckConfigsDlg, wxDialog)
BEGIN_EVENT_TABLE(CheckConfigsDlg, wxDialog)
	EVT_BUTTON(XRCID("CheckConfigsUserButton"),CheckConfigsDlg::OnUserButton)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CheckConfigsDlg::CheckConfigsDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("CheckHardware"));
	m_completemachines = (wxStaticText *)FindWindowByName(wxT("CompleteMachinesCount"));
	m_incompletemachines = (wxStaticText *)FindWindowByName(wxT("IncompleteMachinesCount"));
	m_workingextensions = (wxStaticText *)FindWindowByName(wxT("CompleteExtensionsCount"));
	m_nonworkingextensions = (wxStaticText *)FindWindowByName(wxT("InCompleteExtensionsCount"));
	m_currentconfig = (wxStaticText *)FindWindowByName(wxT("CheckingObject"));
	m_progressbar = (wxGauge *)FindWindowByName (wxT("ProgressGauge"));
	m_progressbar->SetRange(100);
	m_userbutton = (wxButton *)FindWindowByName (wxT("CheckConfigsUserButton"));
	m_log = (wxTextCtrl *)FindWindowByName (wxT("MissingObjects"));
	m_validmachinecount = 0;
	m_invalidmachinecount = 0;
	m_validextensioncount = 0;
	m_invalidextensioncount = 0;
}	

CheckConfigsDlg::~CheckConfigsDlg()
{
}

int CheckConfigsDlg::ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions)
{
	m_auditThread = new CheckConfigsDlg::CheckConfigsThread (this);
	m_auditThread->Create();
	m_auditThread->SetParameters(cmd,&machines, &extensions);
	m_auditThread->Run();
	return wxDialog::ShowModal();
}

void CheckConfigsDlg::OnUserButton(wxCommandEvent &event)
{
	if (m_userbutton->GetLabel() == wxT("Ok")) { // FIXME: this is ugly!
		EndModal(wxID_OK);
	}
	else{
		m_auditThread->m_abort = true;
	}
}

void CheckConfigsDlg::UpdateStats(bool checkmachine, bool succes, int progress)
{
	wxString count;
	if (succes) {
		if (checkmachine) {
			count.sprintf(wxT("%d"),++m_validmachinecount);
			m_completemachines->SetLabel(count);
		}
		else{
			count.sprintf(wxT("%d"),++m_validextensioncount);
			m_workingextensions->SetLabel(count);	
		}
	}
	else{
		if (checkmachine) {
			count.sprintf(wxT("%d"),++m_invalidmachinecount);
			m_incompletemachines->SetLabel(count);
			m_log->AppendText(wxT("machine: "));			
		}
		else{
			count.sprintf(wxT("%d"),++m_invalidextensioncount);
			m_nonworkingextensions->SetLabel(count);	
			m_log->AppendText(wxT("extension: "));					
		}
		m_log->AppendText(m_currentObject);
		m_log->AppendText(wxT("\n"));
	}
	m_progressbar->SetValue(progress);
}

void CheckConfigsDlg::FinishCheck()
{
	if (m_auditThread->m_abort) {
		EndModal(wxID_CANCEL);		
	}
	else{	
		m_userbutton->SetLabel (wxT("Ok"));
		m_currentconfig->SetLabel(wxT("Done"));
	}
}

void CheckConfigsDlg::SetCurrentObject(wxString object)
{
	m_currentObject = object;
	m_currentconfig->SetLabel(object);
}

CheckConfigsDlg::CheckConfigsThread::CheckConfigsThread (CheckConfigsDlg * target)
{
	m_target = target;
}

CheckConfigsDlg::CheckConfigsThread::~CheckConfigsThread()
{
}

wxThread::ExitCode CheckConfigsDlg::CheckConfigsThread::Entry()
{
	m_abort = false;
	m_workingmachine = wxT("");
	wxString fullCommand;
	int progress;
	int numberOfMachines = m_machines->Count();
	int numberOfExtensions = m_extensions->Count();
	int machine = 0;
	while ((machine < numberOfMachines) && !m_abort) {
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += m_machines->Item(machine);
		m_target->SetCurrentObject(m_machines->Item(machine));
		progress = (50*(machine+1))/numberOfMachines;
		bool success = doCheckConfigs (fullCommand);
		if (success) {
			if (m_workingmachine == wxT("")) {
				m_workingmachine = m_machines->Item(machine);
			}
		}
		m_target->UpdateStats (true, success, progress);
		machine++;
	}
	int extension = 0;
	while ((extension < numberOfExtensions) && !m_abort) {	
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += m_workingmachine;
		fullCommand += wxT(" -ext ");
		fullCommand += m_extensions->Item(extension);
		m_target->SetCurrentObject(m_extensions->Item(extension));
		progress = ((50*(extension+1))/numberOfExtensions)+50;
		bool success = doCheckConfigs (fullCommand);
		m_target->UpdateStats (false, success, progress);
		extension++;
	}
	m_target->FinishCheck();
	return 0;
}

void CheckConfigsDlg::CheckConfigsThread::SetParameters(wxString cmd, wxArrayString * machines, wxArrayString * extensions)
{
	m_machines = machines;
	m_extensions = extensions;
	m_cmd = cmd;
}

bool CheckConfigsDlg::CheckConfigsThread::doCheckConfigs (wxString cmd)
{
	unsigned long result;
	char buffer[1000];
#ifndef __WXMSW__
	cmd += wxT(" > /dev/null"); // keep stderr displayed for now
#endif
	strcpy (buffer,(const char *) (wxConvUTF8.cWX2MB((cmd))));

	bool succes;
#ifdef __WXMSW__
	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE;
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	ZeroMemory(&si,sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESHOWWINDOW;	
	si.wShowWindow = SW_HIDE;
	CreateProcessA (NULL,buffer,
			NULL,NULL,false, dwProcessFlags ,NULL,NULL,&si,&pi);
	WaitForSingleObject (pi.hProcess,INFINITE);
	GetExitCodeProcess(pi.hProcess, &result);
#else
	result = system (buffer);
#endif	

	if (result == 0)	{
		succes = true;		
	}
	else{
		succes = false;
	}
	return succes;
}
