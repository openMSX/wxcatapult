// openMSXLinuxController.cpp: implementation of the openMSXLinuxController class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/process.h>
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

}

openMSXLinuxController::~openMSXLinuxController()
{

}

bool openMSXLinuxController::Launch(wxString cmdline)
{
	PreLaunch();
	cmdline += _(" -control stdio:");
	wxProcess * process = wxProcess::Open(cmdline,wxEXEC_ASYNC);
	m_outputstream = new wxTextOutputStream(*process->GetOutputStream());
	PipeReadThread * thread = new PipeReadThread(m_appWindow, MSGID_STDOUT, process);	
	if (thread->Create() == wxTHREAD_NO_ERROR)
	{
		thread->Run();
	}
	PipeReadThread * thread2 = new PipeReadThread(m_appWindow, MSGID_STDERR, process);	
	if (thread2->Create() == wxTHREAD_NO_ERROR)
	{
		thread2->Run();
	}
	m_openMsxRunning = true;
	PostLaunch();
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
}

bool openMSXLinuxController::WriteMessage(wxString msg)
{
	if (!m_openMsxRunning) 
		return false;
	m_outputstream->WriteString(msg);
	return true;
}
