// openMSXWindowsController.cpp: implementation of the openMSXWindowsController class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "PipeReadThread.h"
#include "PipeConnectThread.h"
#include "openMSXWindowsController.h"
#include "wxCatapultFrm.h"
#include "wxCatapultApp.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	openMSXWindowsController::openMSXWindowsController(wxWindow * target)
:openMSXController(target)
{
	m_launchCounter = 0;
	m_connectThread = NULL;
	m_pipeActive = false;
	m_openMsxRunning = false;
	m_namedPipeHandle = INVALID_HANDLE_VALUE;	
}

openMSXWindowsController::~openMSXWindowsController()
{

}

bool openMSXWindowsController::HandleMessage(wxCommandEvent &event)
{
	int id = event.GetId();
	switch (id)
	{
		case MSGID_PIPECREATED:
			HandlePipeCreated();
			break;
		case MSGID_ENDPROCESS:
			HandleEndProcess(event);
			break;
		default:
			if (openMSXController::HandleMessage(event)){
				return true;
			}
			return false; // invalid ID
	}
	return true;		
}

bool openMSXWindowsController::Launch(wxString cmdLine)
{
	HANDLE hInputRead, hOutputWrite, hErrorWrite, hErrorRead, hOutputRead;
	PreLaunch();
	bool useNamedPipes = DetermenNamedPipeUsage();
	cmdLine += CreateControlParameter(useNamedPipes);
	CreatePipes(useNamedPipes, &hInputRead, &hOutputWrite, &hErrorWrite, &hOutputRead, &hErrorRead);

	PROCESS_INFORMATION pi;

	DWORD dwProcessFlags = CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
	WORD wStartupWnd = SW_HIDE;

	STARTUPINFO si;
	DWORD dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;	
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = dwFlags;	
	si.hStdInput = hInputRead;	
	si.hStdOutput = hOutputWrite;
	si.hStdError  = hErrorWrite;
	si.wShowWindow = wStartupWnd;

	CreateProcess (NULL,(char *)cmdLine.c_str(),
			NULL,NULL,true, dwProcessFlags ,NULL,NULL,&si,&pi); //testing suspended

	PipeReadThread * thread = new PipeReadThread(m_appWindow, MSGID_STDOUT, NULL);	
	if (thread->Create() == wxTHREAD_NO_ERROR)
	{
		thread->SetHandle(hOutputRead);
		thread->Run();
	}
	PipeReadThread * thread2 = new PipeReadThread (m_appWindow, MSGID_STDERR, NULL);
	if (thread2->Create() == wxTHREAD_NO_ERROR)
	{
		thread2->SetHandle(hErrorRead);
		thread2->Run();
	}

	::ResumeThread(pi.hThread);

	if (useNamedPipes)
	{
		if (!m_pipeActive)
		{
			m_pipeActive = true;
			m_connectThread->SetHandle(m_namedPipeHandle);
			m_connectThread->Run();
		}
		m_outputHandle = m_namedPipeHandle;

	}
	else{
		m_openMsxRunning = true;
		PostLaunch();
		m_appWindow->m_launch_AbortButton->Enable(true);
	}
	m_openMsxRunning = true;
	CloseHandles (useNamedPipes,pi.hThread, hInputRead, hOutputWrite, hErrorWrite );
	return true;
}

bool openMSXWindowsController::DetermenNamedPipeUsage()
{
	bool useNamedPipes = false;
	if (!FORCE_UNNAMED_PIPES){
		OSVERSIONINFO info;
		info.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);	
		if (!GetVersionEx (&info))
		{
			wxString msg;
			msg.sprintf ("Error getting system info: %d ", GetLastError());
			wxMessageBox (msg);
		}
		else{
			if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
				useNamedPipes = true; // nt-based only and only if the user wants it
		}

	}
	return useNamedPipes;
}

wxString openMSXWindowsController::CreateControlParameter(bool useNamedPipes)
{
	wxString parameter = _(" -control");

	if (useNamedPipes)
	{
		if (m_connectThread == NULL)
			m_launchCounter++;
		wxString pipeName;
		pipeName.sprintf ("\\\\.\\pipe\\Catapult-%u-%u", _getpid(), m_launchCounter);
		parameter += _(" pipe:") + (pipeName.Mid(9));
		if (m_connectThread == NULL)
		{
			m_connectThread = new PipeConnectThread (m_appWindow);
			m_connectThread->Create();
			m_namedPipeHandle = CreateNamedPipe (pipeName,PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE, 1, 10000, 0, 100, NULL);
			if (m_namedPipeHandle == INVALID_HANDLE_VALUE)
			{
				wxString text;
				text.sprintf("Error creating pipe: %d",GetLastError());
				wxMessageBox (text);
			}
		}
		else
			m_namedPipeHandle = m_outputHandle;
	}
	else 
		parameter += _(" stdio:");
	return wxString (parameter);
}

bool openMSXWindowsController::CreatePipes(bool useNamedPipes,HANDLE * input,
		HANDLE * output, HANDLE * error, 
		HANDLE * outputWrite, HANDLE * errorWrite)
{
	HANDLE hOutputReadTmp, hOutputWrite;
	HANDLE hErrorReadTmp, hErrorWrite;
	HANDLE hInputRead=0,hInputWriteTmp,hInputWrite;
	HANDLE hInputHandle = GetStdHandle(STD_INPUT_HANDLE);

	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!useNamedPipes)
	{
		if (!CreatePipe(&hInputRead, &hInputWriteTmp,&sa,0))
		{
			ShowError(_("Error creating pipe for stdin"));
			return false;
		}

		if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
					GetCurrentProcess(), &hInputWrite, 0,FALSE, DUPLICATE_SAME_ACCESS))
		{
			ShowError(_("Error Duplicating InputWriteTmp Handle"));
			return false;

		}

		if (!CloseHandle(hInputWriteTmp))
		{
			ShowError(_("Error Closing Input Temp Handle"));
			return false;
		}
		m_outputHandle = hInputWrite;
		hInputHandle  = hInputRead;
	}

	if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
	{
		ShowError(_("Error creating pipe for stdout"));
		return false;
	}

	if (!CreatePipe(&hErrorReadTmp,&hErrorWrite,&sa,0))
	{
		ShowError(_("Error creating pipe for stderr"));
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
	error.sprintf ("%d",GetLastError());
	wxMessageBox (msg + wxString(_(": error ")) + error);	
}

void openMSXWindowsController::CloseHandles(bool useNamedPipes, HANDLE hThread,
		HANDLE hInputRead, HANDLE hOutputWrite,
		HANDLE hErrorWrite)
{
	if (!CloseHandle(hThread)) 
	{
		wxMessageBox (_T("Unable to close thread handle"));
		return;
	}

	if (!CloseHandle(hOutputWrite))
	{
		wxMessageBox (_T("Unable to close Output Write"));
		return;	  
	};
	if (!CloseHandle(hErrorWrite))
	{
		wxMessageBox (_T("Unable to close Error Write"));
		return;	  
	};

	if (!useNamedPipes)
	{
		if (!CloseHandle(hInputRead))
		{
			wxMessageBox (_T("Unable to close Input Read"));
			return;	  
		};
	}	
}

void openMSXWindowsController::HandlePipeCreated()
{
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_pipeActive = false;
	PostLaunch();
}

bool openMSXWindowsController::WriteMessage(wxString msg)
{
	if (!m_openMsxRunning) 
		return false;
	unsigned long BytesWritten;
	if (!::WriteFile(m_outputHandle,msg.c_str(),msg.Len(),&BytesWritten,NULL))
		return false;
	return true;
}

void openMSXWindowsController::HandleEndProcess(wxCommandEvent &event)
{
	openMSXController::HandleEndProcess(event);
	if (!m_pipeActive)
	{
		m_connectThread = NULL;
	}
}
