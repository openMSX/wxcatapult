#include "wx/wxprec.h"
#include "config.h"
#include "PipeReadThread.h"
#include "PipeConnectThread.h"
#include "openMSXWindowsController.h"
#include "StatusPage.h"
#include "wxCatapultFrm.h"
#include "wxCatapultApp.h"
#include <process.h>
#include <wx/textfile.h>

openMSXWindowsController::openMSXWindowsController(wxWindow * target)
	: openMSXController(target)
{
	m_launchCounter = 0;
	m_connectThread = nullptr;
	m_pipeActive = false;
	m_openMsxRunning = false;
	m_namedPipeHandle = INVALID_HANDLE_VALUE;
	m_socket = nullptr;
}

openMSXWindowsController::~openMSXWindowsController()
{
	if (m_openMsxRunning) {
		WriteCommand(wxT("exit"));
	}
}

bool openMSXWindowsController::HandleMessage(wxCommandEvent& event)
{
	switch (event.GetId()) {
	case MSGID_PIPECREATED:
		HandlePipeCreated();
		break;
	case MSGID_ENDPROCESS:
		HandleEndProcess(event);
		break;
	default:
		if (openMSXController::HandleMessage(event)) {
			return true;
		}
		return false; // invalid ID
	}
	return true;
}

bool openMSXWindowsController::Launch(wxString cmdLine)
{
	m_catapultWindow = GetActiveWindow();
	HANDLE hInputRead, hOutputWrite, hErrorWrite, hErrorRead, hOutputRead;
	PreLaunch();
	bool useNamedPipes = DetermenNamedPipeUsage();
	cmdLine += CreateControlParameter(useNamedPipes);
	CreatePipes(useNamedPipes, &hInputRead, &hOutputWrite, &hErrorWrite, &hOutputRead, &hErrorRead);

	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
	WORD wStartupWnd = SW_HIDE;

	STARTUPINFO si;
	DWORD dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = dwFlags;
	si.hStdInput = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError  = hErrorWrite;
	si.wShowWindow = wStartupWnd;

	LPTSTR szCmdLine = _tcsdup(cmdLine.c_str());
	if (szCmdLine) {
		CreateProcess(
			nullptr, szCmdLine, nullptr, nullptr, true,
			dwProcessFlags, nullptr, nullptr, &si,
			&m_openmsxProcInfo); //testing suspended
		free(szCmdLine);
	}

	auto* thread = new PipeReadThread(m_appWindow, MSGID_STDOUT);
	if (thread->Create() == wxTHREAD_NO_ERROR) {
		thread->SetHandle(hOutputRead);
		thread->Run();
	}
	auto* thread2 = new PipeReadThread (m_appWindow, MSGID_STDERR);
	if (thread2->Create() == wxTHREAD_NO_ERROR) {
		thread2->SetHandle(hErrorRead);
		thread2->Run();
	}

	::ResumeThread(m_openmsxProcInfo.hThread);

	if (useNamedPipes) {
		if (!m_pipeActive) {
			m_pipeActive = true;
			m_connectThread->SetHandle(m_namedPipeHandle);
			m_connectThread->Run();
		}
		m_outputHandle = m_namedPipeHandle;

	} else {
		m_openMsxRunning = true;
		PostLaunch();
		m_appWindow->m_launch_AbortButton->Enable(true);
	}
	m_openMsxRunning = true;
	CloseHandles(useNamedPipes, m_openmsxProcInfo.hThread, hInputRead, hOutputWrite, hErrorWrite);

	return true;
}

bool openMSXWindowsController::DetermenNamedPipeUsage()
{
	bool useNamedPipes = false;
	if (!FORCE_UNNAMED_PIPES) {
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx(&info)) {
			wxString msg;
			msg.sprintf(wxT("Error getting system info: %ld "), GetLastError());
			wxMessageBox(msg);
		} else {
			if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
				useNamedPipes = true; // nt-based only and only if the user wants it
			}
		}
	}
	return useNamedPipes;
}

wxString openMSXWindowsController::CreateControlParameter(bool useNamedPipes)
{
	wxString parameter = wxT(" -control");

	if (useNamedPipes) {
		if (m_connectThread == nullptr) {
			m_launchCounter++;
		}
		wxString pipeName;
		pipeName.sprintf (wxT("\\\\.\\pipe\\Catapult-%u-%lu"), _getpid(), m_launchCounter);
		parameter += wxT(" pipe:") + (pipeName.Mid(9));
		if (m_connectThread == nullptr) {
			m_connectThread = new PipeConnectThread(m_appWindow);
			m_connectThread->Create();
			m_namedPipeHandle = CreateNamedPipe (pipeName,PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 10000, 0, 100, nullptr);
			if (m_namedPipeHandle == INVALID_HANDLE_VALUE) {
				wxString text;
				text.sprintf(wxT("Error creating pipe: %ld"),GetLastError());
				wxMessageBox(text);
			}
		} else {
			m_namedPipeHandle = m_outputHandle;
		}
	} else {
		parameter += wxT(" stdio:");
	}
	return wxString (parameter);
}

bool openMSXWindowsController::CreatePipes(
	bool useNamedPipes, HANDLE* input, HANDLE* output, HANDLE* error,
	HANDLE* outputWrite, HANDLE* errorWrite)
{
	HANDLE hOutputReadTmp, hOutputWrite;
	HANDLE hErrorReadTmp, hErrorWrite;
	HANDLE hInputRead = 0, hInputWriteTmp, hInputWrite;
	HANDLE hInputHandle = GetStdHandle(STD_INPUT_HANDLE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = TRUE;

	if (!useNamedPipes) {
		if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0)) {
			ShowError(wxT("Error creating pipe for stdin"));
			return false;
		}
		if (!DuplicateHandle(GetCurrentProcess(), hInputWriteTmp,
		                     GetCurrentProcess(), &hInputWrite, 0,
		                     FALSE, DUPLICATE_SAME_ACCESS)) {
			ShowError(wxT("Error Duplicating InputWriteTmp Handle"));
			return false;
		}
		if (!CloseHandle(hInputWriteTmp)) {
			ShowError(wxT("Error Closing Input Temp Handle"));
			return false;
		}
		m_outputHandle = hInputWrite;
		hInputHandle = hInputRead;
	}

	if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0)) {
		ShowError(wxT("Error creating pipe for stdout"));
		return false;
	}
	if (!CreatePipe(&hErrorReadTmp, &hErrorWrite, &sa, 0)) {
		ShowError(wxT("Error creating pipe for stderr"));
		return false;
	}
	*input = hInputHandle;
	*output = hOutputWrite;
	*error  = hErrorWrite;
	*outputWrite = hOutputReadTmp;
	*errorWrite = hErrorReadTmp;
	return true;
}

void openMSXWindowsController::ShowError(wxString msg)
{
	wxString error;
	error.sprintf(wxT("%ld"), GetLastError());
	wxMessageBox(msg + wxString(wxT(": error ")) + error);
}

void openMSXWindowsController::CloseHandles(
	bool useNamedPipes, HANDLE hThread, HANDLE hInputRead,
	HANDLE hOutputWrite, HANDLE hErrorWrite)
{
	if (!CloseHandle(hThread)) {
		wxMessageBox(wxT("Unable to close thread handle"));
		return;
	}
	if (!CloseHandle(hOutputWrite)) {
		wxMessageBox(wxT("Unable to close Output Write"));
		return;
	}
	if (!CloseHandle(hErrorWrite)) {
		wxMessageBox(wxT("Unable to close Error Write"));
		return;
	}
	if (!useNamedPipes) {
		if (!CloseHandle(hInputRead)) {
			wxMessageBox(wxT("Unable to close Input Read"));
			return;
		}
	}
}

void openMSXWindowsController::HandlePipeCreated()
{
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_pipeActive = false;
	PostLaunch();
}

bool openMSXWindowsController::WriteMessage(xmlChar* msg, size_t length)
{
	if (!m_openMsxRunning) return false;

	if (m_socket && (m_socket->IsConnected())) {
		m_socket->Write(msg, length);
		if (!m_socket->LastError()) {
			return true;
		}
		return false;
	}
	unsigned long BytesWritten;
	if (!::WriteFile(m_outputHandle, msg, length, &BytesWritten, nullptr)) {
		return false;
	}
	return true;
}

void openMSXWindowsController::HandleEndProcess(wxCommandEvent& event)
{
	openMSXController::HandleEndProcess(event);
	if (!m_pipeActive) {
		m_connectThread = nullptr;
	}
}

wxString openMSXWindowsController::GetOpenMSXVersionInfo(wxString openmsxCmd)
{
	wxString version = wxT("");
	wxArrayString output;
	int code = wxExecute(openmsxCmd + wxT(" -v"), output);
	if ((code != -1) && (output.GetCount() > 0)) {
		version = output[0];
	}
	return version;
}

void openMSXWindowsController::RaiseOpenMSX()
{
	HWND openmsxWindow = FindOpenMSXWindow();
	if (openmsxWindow != nullptr) {
		SetParent(openmsxWindow, m_catapultWindow);
		SetActiveWindow(openmsxWindow);
		SetForegroundWindow(openmsxWindow);
		SetParent(openmsxWindow, nullptr);
	}
}

void openMSXWindowsController::RestoreOpenMSX()
{
	HWND openmsxWindow = FindOpenMSXWindow();
	if (openmsxWindow != nullptr) {
		SetParent(openmsxWindow, m_catapultWindow);
		SetWindowPos(openmsxWindow, HWND_TOP, 0, 0, 640, 480, SWP_NOSIZE || SWP_SHOWWINDOW);
		SetParent(openmsxWindow, nullptr);
	}
}

HWND openMSXWindowsController::FindOpenMSXWindow()
{
	if (!m_openMsxRunning) return nullptr;

	FindOpenmsxInfo findInfo;
	findInfo.hWndFound = nullptr;
	findInfo.ProcessInfo = &m_openmsxProcInfo;

	WaitForInputIdle(m_openmsxProcInfo.hProcess, INFINITE);
	EnumWindows(EnumWindowCallBack, (LPARAM)&findInfo);
	return findInfo.hWndFound;
}

BOOL CALLBACK openMSXWindowsController::EnumWindowCallBack(HWND hwnd, LPARAM lParam)
{
	FindOpenmsxInfo* info = (FindOpenmsxInfo*)lParam;
	DWORD ProcessId;
	GetWindowThreadProcessId(hwnd, &ProcessId);
	TCHAR title[11];
	GetWindowText(hwnd, title, 10);
	if (ProcessId == info->ProcessInfo->dwProcessId && _tcslen(title) != 0) {
		info->hWndFound = hwnd;
		return false;
	} else {
		// Keep enumerating
		return true;
	}
}
