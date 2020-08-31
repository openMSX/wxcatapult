#include "CheckConfigsDlg.h"
#include "wxCatapultApp.h"
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <vector>
#include <algorithm>

#define MSGID_SETCURRENTOBJECT 0
#define MSGID_UPDATESTATS 1

// this is for an ugly hack: these extensions only work on certain machines and
// typically it's not the machine that Catapult is using to test the extension
// with. However, the goal of the testing was almost exclusively to find out
// about missing/uninstalled system ROMs. As these extensions do not have
// system ROMs, we treat them specially (see below where this vector is
// used)... TODO: implement a better solution.
static const std::vector<std::string> extensionsWithoutROMs = { "advram", "Casio_KB-7", "Casio_KB-10"  };

IMPLEMENT_CLASS(CheckConfigsDlg, wxDialog)
BEGIN_EVENT_TABLE(CheckConfigsDlg, wxDialog)
	EVT_BUTTON(XRCID("CheckConfigsUserButton"), CheckConfigsDlg::OnUserButton)
	EVT_COMMAND (-1, EVT_TESTCONFIG, CheckConfigsDlg::OnTestConfigEvent)
END_EVENT_TABLE()

CheckConfigsDlg::CheckConfigsDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("CheckConfigs"));
	m_completemachines     = (wxStaticText*)FindWindowByName(wxT("CompleteMachinesCount"));
	m_incompletemachines   = (wxStaticText*)FindWindowByName(wxT("IncompleteMachinesCount"));
	m_workingextensions    = (wxStaticText*)FindWindowByName(wxT("CompleteExtensionsCount"));
	m_nonworkingextensions = (wxStaticText*)FindWindowByName(wxT("InCompleteExtensionsCount"));
	m_currentconfig        = (wxStaticText*)FindWindowByName(wxT("CheckingObject"));
	m_progressbar = (wxGauge*)FindWindowByName(wxT("ProgressGauge"));
	m_progressbar->SetRange(100);
	m_userbutton = (wxButton*)FindWindowByName(wxT("CheckConfigsUserButton"));
	m_log = (wxTextCtrl*)FindWindowByName(wxT("MissingObjects"));
	m_validmachinecount = 0;
	m_invalidmachinecount = 0;
	m_validextensioncount = 0;
	m_invalidextensioncount = 0;
}

int CheckConfigsDlg::Present(wxString cmd, wxArrayString& machines, wxArrayString& extensions)
{
	m_auditThread.reset(new CheckConfigsDlg::CheckConfigsThread(this));
	m_auditThread->Create();
	m_auditThread->SetParameters(cmd, &machines, &extensions);
	m_auditThread->Run();
	return ShowModal();
}

void CheckConfigsDlg::EndModal(int retCode) // TODO: check if this is also called for posix systems
{
	m_auditThread->m_running = false;
	m_auditThread->Wait();
	wxDialog::EndModal(retCode);
}

void CheckConfigsDlg::OnUserButton(wxCommandEvent& event)
{
	if (!m_auditThread->m_running) {
		EndModal(wxID_OK);
	} else {
		m_auditThread->m_running = false;
		EndModal(wxID_CANCEL);
	}
}

void CheckConfigsDlg::OnTestConfigEvent(wxCommandEvent& event)
{
	std::unique_ptr<CheckConfigsData> data(
		(CheckConfigsData*)event.GetClientData());
	switch (event.GetId()) {
	case MSGID_SETCURRENTOBJECT:
		HandleSetCurrentObject(data->m_currentObject);
		break;
	case MSGID_UPDATESTATS:
		HandleUpdateStats(data->m_checkmachine, data->m_succes, data->m_progress);
		break;
	}
}

void CheckConfigsDlg::HandleUpdateStats(bool checkmachine, bool succes, int progress)
{
	if (succes) {
		if (checkmachine) {
			m_completemachines->SetLabel(wxString::Format(
				wxT("%d"), ++m_validmachinecount));
		} else {
			m_workingextensions->SetLabel(wxString::Format(
				wxT("%d"), ++m_validextensioncount));
		}
	} else {
		if (checkmachine) {
			m_incompletemachines->SetLabel(wxString::Format(
				wxT("%d"), ++m_invalidmachinecount));
			m_log->AppendText(wxT("machine: "));
		} else {
			m_nonworkingextensions->SetLabel(wxString::Format(
				wxT("%d"), ++m_invalidextensioncount));
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
	tmp.Replace(wxT("_"), wxT(" "));
	m_currentconfig->SetLabel(tmp);
}

void CheckConfigsDlg::FinishCheck()
{
	m_userbutton->SetLabel(wxT("Done"));
	m_currentconfig->SetLabel(wxT("Done"));
}

CheckConfigsDlg::CheckConfigsThread::CheckConfigsThread(CheckConfigsDlg* target)
	: wxThread(wxTHREAD_JOINABLE)
{
	m_target = target;
}

wxThread::ExitCode CheckConfigsDlg::CheckConfigsThread::Entry()
{
	m_running = true;
	std::vector<wxString> workingMachines;
	int numberOfMachines = m_machines->Count();
	int numberOfExtensions = m_extensions->Count();
	int config = 0;
	size_t machineIndex = 0;
	while ((machineIndex < m_machines->Count()) && m_running) {
		wxString machine(m_machines->Item(machineIndex));
		wxString fullCommand;
		fullCommand << m_cmd << wxT(" -testconfig -machine \"")
		            << machine << wxT("\"");
		SetCurrentObject(machine);
		int progress = (50 * (config + 1)) / numberOfMachines;
		bool success = doCheckConfigs(fullCommand);
		if (success) {
			workingMachines.push_back(machine);
			++machineIndex;
		} else {
			m_machines->RemoveAt(machineIndex);
		}
		++config;
		if (m_running) {
			UpdateStats(true, success, progress);
		}

	}
	config = 0;
	if (!workingMachines.empty()) {
		wxString workingMachine(workingMachines[0]);
		// check if this machine is working. If so, prefer that.
		const wxString preferredMachine(wxT("C-BIOS_MSX2+"));
		if (std::find(workingMachines.begin(), workingMachines.end(), preferredMachine) != workingMachines.end()) {
			workingMachine = preferredMachine;
		}
		// note that all of this won't work for non-MSX extensions...
		size_t extIndex = 0;
		while ((extIndex < m_extensions->Count()) && m_running) {
			wxString fullCommand;
			wxString extension = m_extensions->Item(extIndex);
			fullCommand << m_cmd << wxT(" -testconfig -machine \"")
				    << workingMachine << wxT("\" -ext \"")
				    << extension << wxT("\"");
			SetCurrentObject(extension);
			int progress = ((50 * (config + 1)) / numberOfExtensions) + 50;
			bool success;
			if (std::find(extensionsWithoutROMs.begin(), extensionsWithoutROMs.end(), std::string(extension.mb_str())) == extensionsWithoutROMs.end()) {
				success = doCheckConfigs(fullCommand);
			} else {
				success = true;
			}
			if (!success) {
				m_extensions->RemoveAt(extIndex);
			} else {
				++extIndex;
			}
			++config;
			if (m_running) {
				UpdateStats(false, success, progress);
			}
		}
	}
	if (m_running) {
		m_target->FinishCheck();
	}
	m_running = false;
	return 0;
}

void CheckConfigsDlg::CheckConfigsThread::UpdateStats(bool checkmachine, bool succes, int progress)
{
	wxCommandEvent checkConfigEvent(EVT_TESTCONFIG);
	CheckConfigsData* data = new CheckConfigsData;
	data->m_checkmachine = checkmachine;
	data->m_succes = succes;
	data->m_progress = progress;
	checkConfigEvent.SetClientData(data);
	checkConfigEvent.SetId(MSGID_UPDATESTATS);
	wxPostEvent(m_target, checkConfigEvent);
}


void CheckConfigsDlg::CheckConfigsThread::SetCurrentObject(wxString object)
{
	wxCommandEvent checkConfigEvent(EVT_TESTCONFIG);
	CheckConfigsData* data = new CheckConfigsData;
	data->m_currentObject = object;
	checkConfigEvent.SetClientData(data);
	checkConfigEvent.SetId(MSGID_SETCURRENTOBJECT);
	wxPostEvent(m_target, checkConfigEvent);
}

void CheckConfigsDlg::CheckConfigsThread::SetParameters(
	wxString cmd, wxArrayString* machines, wxArrayString* extensions)
{
	m_machines = machines;
	m_extensions = extensions;
	m_cmd = cmd;
}

bool CheckConfigsDlg::CheckConfigsThread::doCheckConfigs(wxString cmd)
{
	unsigned long result = 1;
#ifndef __WXMSW__
	char buffer[1000];
	cmd += wxT(" > /dev/null 2>&1");
	strcpy(buffer, (const char*)(cmd.mb_str(wxConvUTF8)));
	result = system(buffer);
#else
	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	if (LPTSTR szCmdLine = _tcsdup(cmd.c_str())) {
		BOOL created = CreateProcess(
			nullptr, szCmdLine, nullptr, nullptr, false,
			dwProcessFlags, nullptr, nullptr, &si, &pi);
		if (created) {
			WaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeProcess(pi.hProcess, &result);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
#endif
	return result == 0;
}
