// AuditDlg.cpp: implementation of the RomTypeDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxCatapultApp.h"
#include "AuditDlg.h"

IMPLEMENT_CLASS(AuditDlg, wxDialog)
BEGIN_EVENT_TABLE(AuditDlg, wxDialog)
	EVT_BUTTON(XRCID("AuditUserButton"),AuditDlg::OnUserButton)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuditDlg::AuditDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("CheckHardware"));
	m_completemachines = (wxStaticText *)FindWindowByName(wxT("CompleteMachinesCount"));
	m_incompletemachines = (wxStaticText *)FindWindowByName(wxT("IncompleteMachinesCount"));
	m_workingextensions = (wxStaticText *)FindWindowByName(wxT("CompleteExtensionsCount"));
	m_nonworkingextensions = (wxStaticText *)FindWindowByName(wxT("InCompleteExtensionsCount"));
	m_currentconfig = (wxStaticText *)FindWindowByName(wxT("CheckingObject"));
	m_progressbar = (wxGauge *)FindWindowByName (wxT("ProgressGauge"));
	m_progressbar->SetRange(100);
	m_userbutton = (wxButton *)FindWindowByName (wxT("AuditUserButton"));
	m_log = (wxTextCtrl *)FindWindowByName (wxT("MissingObjects"));
	m_validmachinecount = 0;
	m_invalidmachinecount = 0;
	m_validextensioncount = 0;
	m_invalidextensioncount = 0;
}	

AuditDlg::~AuditDlg()
{

}

int AuditDlg::ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions)
{
	m_auditThread = new AuditDlg::AuditThread (this);
	m_auditThread->Create();
	m_auditThread->SetParameters(cmd,&machines, &extensions);
	m_auditThread->Run();
	return wxDialog::ShowModal();
}

void AuditDlg::OnUserButton(wxCommandEvent &event)
{
	if (m_userbutton->GetLabel() == wxT("Ok")){
		EndModal(wxID_OK);
	}
	else{
		m_auditThread->m_abort = true;
	}
}

void AuditDlg::UpdateStats(bool checkmachine, bool succes, int progress)
{
	wxString count;
	if (succes){
		if (checkmachine){
			count.sprintf("%d",++m_validmachinecount);
			m_completemachines->SetLabel(count);
		}
		else{
			count.sprintf("%d",++m_validextensioncount);
			m_workingextensions->SetLabel(count);	
		}
	}
	else{
		if (checkmachine){
			count.sprintf("%d",++m_invalidmachinecount);
			m_incompletemachines->SetLabel(count);
			m_log->AppendText("machine: ");			
		}
		else{
			count.sprintf("%d",++m_invalidextensioncount);
			m_nonworkingextensions->SetLabel(count);	
			m_log->AppendText("extension: ");					
		}
		m_log->AppendText(m_currentObject);
		m_log->AppendText("\n");
	}
	m_progressbar->SetValue(progress);
}

void AuditDlg::FinishCheck()
{
	if (m_auditThread->m_abort){
		EndModal(wxID_CANCEL);		
	}
	else{	
	m_userbutton->SetLabel (wxT("Ok"));
	m_currentconfig->SetLabel(wxT("Done"));
	}
}

void AuditDlg::SetCurrentObject(wxString object)
{
	m_currentObject = object;
	m_currentconfig->SetLabel(object);
}

AuditDlg::AuditThread::AuditThread (AuditDlg * target)
{
	m_target = target;
}

AuditDlg::AuditThread::~AuditThread()
{
}

wxThread::ExitCode AuditDlg::AuditThread::Entry()
{
	m_abort = false;
	m_workingmachine = "";
	wxString fullCommand;
	int progress;
	int numberOfMachines = m_machines->Count();
	int numberOfExtensions = m_extensions->Count();
	int machine = 0;
	while ((machine < numberOfMachines) && !m_abort){
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += m_machines->Item(machine);
		progress = (50*(machine+1))/numberOfMachines;
		m_target->SetCurrentObject(m_machines->Item(machine));
		if (doAudit (fullCommand, true, progress)){
			if (m_workingmachine == ""){
				m_workingmachine = m_machines->Item(machine);
			}
		}
		machine++;
	}
	int extension = 0;
	while ((extension < numberOfExtensions) && !m_abort){	
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += m_workingmachine;
		fullCommand += wxT(" -ext ");
		fullCommand += m_extensions->Item(extension);
		m_target->SetCurrentObject(m_extensions->Item(extension));
		progress = ((50*(extension+1))/numberOfExtensions)+50;
		doAudit (fullCommand, false, progress);
		extension++;
	}
	m_target->FinishCheck();
	return 0;
}

void AuditDlg::AuditThread::SetParameters(wxString cmd, wxArrayString * machines, wxArrayString * extensions)
{
	m_machines = machines;
	m_extensions = extensions;
	m_cmd = cmd;
}

bool AuditDlg::AuditThread::doAudit (wxString cmd, bool checkmachine, int progress)
{
	unsigned long result;
	char buffer[1000];
	strcpy (buffer,(const char *) (wxConvUTF8.cWX2MB((cmd))));

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
	bool succes;
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
	m_target->UpdateStats (checkmachine, succes, progress);
	return succes;
}
