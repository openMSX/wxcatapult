// $Id: PipeConnectThread.cpp,v 1.4 2004/11/14 18:29:47 h_oudejans Exp $
// PipeConnectThread.cpp: implementation of the PipeConnectThread class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"
#include "wxCatapultApp.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "PipeConnectThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PipeConnectThread::PipeConnectThread(wxWindow * target)
{
	m_Connected = false;
	m_target = target;
}

PipeConnectThread::~PipeConnectThread()
{
}

wxThread::ExitCode PipeConnectThread::Entry()
{
	if (!ConnectNamedPipe (m_pipeHandle, NULL))
	{
		wxString text;
		text.sprintf(wxT("Error connection pipe: %ld"),GetLastError());
		wxMessageBox (text);

	};
	wxCommandEvent endEvent(EVT_CONTROLLER);
	endEvent.SetId(MSGID_PIPECREATED);
	wxPostEvent (m_target, endEvent);
	return 0;
}

void PipeConnectThread::SetHandle(HANDLE pipeHandle)
{
	m_pipeHandle = pipeHandle;
}
