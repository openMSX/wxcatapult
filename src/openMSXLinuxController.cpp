// $Id: openMSXLinuxController.cpp,v 1.15 2004/12/03 18:38:20 h_oudejans Exp $
// openMSXLinuxController.cpp: implementation of the openMSXLinuxController class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <unistd.h>
#include <wx/process.h>
#include <wx/textfile.h>
#include "openMSXLinuxController.h"
#include "PipeReadThread.h"
#include "wxCatapultFrm.h"
#include "wxCatapultApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	openMSXLinuxController::openMSXLinuxController(wxWindow * target)
:openMSXController(target)
{
	m_openMSXstdin = m_openMSXstdout = m_openMSXstderr = -1;
}

openMSXLinuxController::~openMSXLinuxController()
{
	if (m_openMsxRunning) { 
		WriteCommand(wxT("quit"));
		m_stdOutThread->Wait();
		m_stdErrThread->Wait();
		delete m_stdOutThread;
		delete m_stdErrThread;
	}
}

bool openMSXLinuxController::Launch(wxString cmdline)
{
	PreLaunch();
	cmdline += wxT(" -control stdio");
	if (!execute((char *)cmdline.c_str(), m_openMSXstdin, m_openMSXstdout, m_openMSXstderr)) {
		return false;
	}
	m_stdOutThread = new PipeReadThread(m_appWindow, MSGID_STDOUT,wxTHREAD_JOINABLE);
	if (m_stdOutThread->Create() == wxTHREAD_NO_ERROR)
	{
		m_stdOutThread->SetFileDescriptor(m_openMSXstdout);
		m_stdOutThread->Run();
	}
	m_stdErrThread = new PipeReadThread(m_appWindow, MSGID_STDERR,wxTHREAD_JOINABLE);	
	if (m_stdErrThread->Create() == wxTHREAD_NO_ERROR)
	{
		m_stdErrThread->SetFileDescriptor(m_openMSXstderr);
		m_stdErrThread->Run();
	}
	
	m_openMsxRunning = true;
	PostLaunch();
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
}

bool openMSXLinuxController::execute(const string& command, int& fdIn, int& fdOut, int& fdErr)
{
	// create pipes
	const int PIPE_READ = 0;
	const int PIPE_WRITE = 1;
	int pipeStdin[2], pipeStdout[2], pipeStderr[2];
	if ((pipe(pipeStdin)  == -1) ||
	    (pipe(pipeStdout) == -1) ||
	    (pipe(pipeStderr) == -1)) {
		return false;
	}

	// create new thread
	int pid = fork();
	if (pid == -1) {
		return false;
	}
	if (pid == 0) {
		// child thread
		
		// redirect IO
		close(pipeStdin[PIPE_WRITE]);
		close(pipeStdout[PIPE_READ]);
		close(pipeStderr[PIPE_READ]);
		dup2(pipeStdin[PIPE_READ], STDIN_FILENO);
		dup2(pipeStdout[PIPE_WRITE], STDOUT_FILENO);
		dup2(pipeStderr[PIPE_WRITE], STDERR_FILENO);

		// prepare cmdline
		// HACK: use sh to handle quoting
		unsigned len = command.length();
		char* cmd = static_cast<char*>(
		                alloca((len + 1) * sizeof(char)));
		memcpy(cmd, (char *)command.c_str(), len + 1);
		char* argv[4];
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = cmd;
		argv[3] = 0;
		
		// really execute command
		execvp(argv[0], argv);

	} else {
		// parent thread
		close(pipeStdin[PIPE_READ]);
		close(pipeStdout[PIPE_WRITE]);
		close(pipeStderr[PIPE_WRITE]);

		fdIn  = pipeStdin[PIPE_WRITE];
		fdOut = pipeStdout[PIPE_READ];
		fdErr = pipeStderr[PIPE_READ];
	}
	return true;
}

wxString openMSXLinuxController::GetOpenMSXVersionInfo(wxString openmsxCmd)
{
	wxString version = wxT("");
	system ((const char*) (wxConvUTF8.cWX2MB((openmsxCmd +wxT(" -v > /tmp/catapult.tmp")))));
	wxTextFile tempfile (wxT("/tmp/catapult.tmp"));
	if (tempfile.Open()) {
		version = tempfile.GetFirstLine();
		tempfile.Close();
	}
	return wxString (version);
}

bool openMSXLinuxController::WriteMessage(xmlChar * msg,size_t length)
{
	if (!m_openMsxRunning) 
		return false;
	write(m_openMSXstdin, msg,length);
	return true;
}

void openMSXLinuxController::HandleNativeEndProcess ()
{
	close(m_openMSXstdin);
}
