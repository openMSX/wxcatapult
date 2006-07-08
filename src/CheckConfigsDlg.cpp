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

#ifdef __WXMSW__
#include <windows.h>
#endif

#define MSGID_SETCURRENTOBJECT 0
#define MSGID_UPDATESTATS 1


IMPLEMENT_CLASS(CheckConfigsDlg, wxDialog)
BEGIN_EVENT_TABLE(CheckConfigsDlg, wxDialog)
	EVT_BUTTON(XRCID("CheckConfigsUserButton"),CheckConfigsDlg::OnUserButton)
	EVT_COMMAND (-1, EVT_TESTCONFIG, CheckConfigsDlg::OnTestConfigEvent)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CheckConfigsDlg::CheckConfigsDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("CheckConfigs"));
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
	delete m_auditThread;
}

int CheckConfigsDlg::ShowModal(wxString cmd, wxArrayString &machines, wxArrayString &extensions)
{
	m_auditThread = new CheckConfigsDlg::CheckConfigsThread (this);
	m_auditThread->Create();
	m_auditThread->SetParameters(cmd,&machines, &extensions);
	m_auditThread->Run();
	return wxDialog::ShowModal();
}

void CheckConfigsDlg::EndModal(int retCode)		// TODO: check if this is also called for posix systems
{
	m_auditThread->m_running = false;
	m_auditThread->Wait();
	wxDialog::EndModal(retCode);
}

void CheckConfigsDlg::OnUserButton(wxCommandEvent &event)
{
	if (!m_auditThread->m_running){
		EndModal(wxID_OK);
	}
	else{
		m_auditThread->m_running = false;
		EndModal(wxID_CANCEL);
	}
}

void CheckConfigsDlg::OnTestConfigEvent (wxCommandEvent & event)
{
	CheckConfigsData * data = (CheckConfigsData *)event.GetClientData();
	int id = event.GetId();
	switch (id)
	{
	case MSGID_SETCURRENTOBJECT:
		HandleSetCurrentObject (data->m_currentObject);
		break;
	case MSGID_UPDATESTATS:
		HandleUpdateStats(data->m_checkmachine, data->m_succes, data->m_progress);
		break;
	}
	delete data;
}

void CheckConfigsDlg::HandleUpdateStats(bool checkmachine, bool succes, int progress)
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

void CheckConfigsDlg::HandleSetCurrentObject(wxString object)
{
	m_currentObject = object;
	wxString tmp = m_currentObject;
	tmp.Replace(wxT("_"),wxT(" "),true);
	m_currentconfig->SetLabel(tmp);
}


void CheckConfigsDlg::FinishCheck()
{
	m_userbutton->SetLabel (wxT("Done"));
	m_currentconfig->SetLabel(wxT("Done"));
}

CheckConfigsDlg::CheckConfigsThread::CheckConfigsThread (CheckConfigsDlg * target):
	wxThread(wxTHREAD_JOINABLE)
{
	m_target = target;
}

CheckConfigsDlg::CheckConfigsThread::~CheckConfigsThread()
{
}

wxThread::ExitCode CheckConfigsDlg::CheckConfigsThread::Entry()
{
	m_running = true;
	m_workingmachine = wxT("");
	wxString fullCommand;
	int progress;
	int numberOfMachines = m_machines->Count();
	int numberOfExtensions = m_extensions->Count();
	int config = 0;
	int machine = 0;
	while ((machine < (int)m_machines->Count()) && m_running) {
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += wxT("\"");
		fullCommand += m_machines->Item(machine);
		fullCommand += wxT("\"");
		SetCurrentObject(m_machines->Item(machine));
		progress = (50*(config+1))/numberOfMachines;
		bool success = doCheckConfigs (fullCommand);
		if (success) {
			if (m_workingmachine == wxT("")) {
				m_workingmachine = m_machines->Item(machine);

			}
			machine++;
		}
		else{
			m_machines->RemoveAt(machine);
		}
		config ++;
		if (m_running){
			UpdateStats (true, success, progress);
		}

	}
	int extension = 0;
	config = 0;
	while ((extension < (int)m_extensions->Count()) && m_running) {
		fullCommand = m_cmd;
		fullCommand += wxT(" -testconfig");
		fullCommand += wxT(" -machine ");
		fullCommand += wxT("\"");
		fullCommand += m_workingmachine;
		fullCommand += wxT("\"");
		fullCommand += wxT(" -ext ");
		fullCommand += wxT("\"");
		fullCommand += m_extensions->Item(extension);
		fullCommand += wxT("\"");
		SetCurrentObject(m_extensions->Item(extension));
		progress = ((50*(config+1))/numberOfExtensions)+50;
		bool success = doCheckConfigs (fullCommand);
		if (!success){
			m_extensions->RemoveAt(extension);
		}
		else{
			extension++;
		}
		config++;
		if (m_running){
			UpdateStats (false, success, progress);
		}
	}
	if (m_running){
		m_target->FinishCheck();
	}
	m_running = false;
	return 0;
}

void CheckConfigsDlg::CheckConfigsThread::UpdateStats (bool checkmachine, bool succes, int progress)
{
	wxCommandEvent checkConfigEvent(EVT_TESTCONFIG);
	CheckConfigsData * data = new CheckConfigsData;
	data->m_checkmachine = checkmachine;
	data->m_succes = succes;
	data->m_progress = progress;

	checkConfigEvent.SetClientData ((void *)data);
	checkConfigEvent.SetId(MSGID_UPDATESTATS);
	wxPostEvent (m_target, checkConfigEvent);
}


void CheckConfigsDlg::CheckConfigsThread::SetCurrentObject (wxString object)
{
	wxCommandEvent checkConfigEvent(EVT_TESTCONFIG);
	CheckConfigsData * data = new CheckConfigsData;
	data->m_currentObject = object;
	checkConfigEvent.SetClientData ((void *)data);
	checkConfigEvent.SetId(MSGID_SETCURRENTOBJECT);
	wxPostEvent (m_target, checkConfigEvent);

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
#ifdef __WXMSW__
	cmd += wxT(" > NUL");
#else
	cmd += wxT(" > /dev/null 2>&1");
#endif
	strcpy (buffer,(const char *) (wxConvUTF8.cWX2MB((cmd))));

#ifdef __WXMSW__
	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE;
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	BOOL created = CreateProcessA(
		NULL, buffer, NULL, NULL, false, dwProcessFlags, NULL, NULL, &si, &pi
		);
	if (created) {
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &result);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	} else {
		result = 1;
	}
#else
	result = system (buffer);
#endif
	return (result == 0);
}
