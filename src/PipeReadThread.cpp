// $Id: PipeReadThread.cpp,v 1.2 2004/02/04 22:01:15 manuelbi Exp $
// PipeReadThread.cpp: implementation of the PipeReadThread class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxCatapultApp.h"
#include "PipeReadThread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

	PipeReadThread::PipeReadThread(wxWindow * target, int id, wxProcess * process, wxThreadKind kind)
:wxThread(kind)
{
	m_target = target;
	m_process = process;
	m_id = id;
}

PipeReadThread::~PipeReadThread()
{
}

wxThread::ExitCode PipeReadThread::Entry()
{
#ifdef __UNIX__
	// really runnning this
	if (m_id == MSGID_STDOUT)	
		m_inputstream = new wxTextInputStream (*m_process->GetInputStream());
	else
		m_inputstream = new wxTextInputStream (*m_process->GetErrorStream());


	while (m_process->IsInputOpened())
	{
		wxString * temp = new wxString;
		//		while ( !m_process->IsInputAvailable() && m_process->IsInputOpened() );		
		*temp = (m_inputstream->ReadLine());
		temp->Append(_("\n"));
		wxCommandEvent event(EVT_CONTROLLER);
		event.SetId(m_id);
		event.SetClientData((void *)temp);
		wxPostEvent(m_target, event);
	}

#else	

	DWORD dwBytesRead;
	BOOL bResult;
	char szBuffer [1010];
	do 	
	{
		bResult=ReadFile(m_hTarget,szBuffer,1000,&dwBytesRead,NULL);
		if (bResult) // the bytes could not be read
		{
			wxString temp2 = szBuffer;
			wxString * temp = new wxString;
			*temp = temp2.Left(dwBytesRead);
			wxCommandEvent event(EVT_CONTROLLER);
			event.SetId(m_id);
			event.SetClientData((void *)temp);
			wxPostEvent(m_target, event);

		}
	} while (bResult);
#endif	
	wxCommandEvent endEvent(EVT_CONTROLLER);
	endEvent.SetId(MSGID_ENDPROCESS);
	wxPostEvent (m_target, endEvent);
	delete m_inputstream;
	return 0;
}
