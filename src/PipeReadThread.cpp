// $Id: PipeReadThread.cpp,v 1.3 2004/04/01 08:31:48 h_oudejans Exp $
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

	PipeReadThread::PipeReadThread(wxWindow * target, int id, wxThreadKind kind)
:wxThread(kind)
{
	m_target = target;
	m_id = id;
}

PipeReadThread::~PipeReadThread()
{
}

wxThread::ExitCode PipeReadThread::Entry()
{
#ifdef __UNIX__
	size_t bytesRead;
	char szBuffer[1001];
	do{
		bytesRead = read(m_descriptor,szBuffer,1000);
		if (bytesRead > 0){
			wxString temp2 = szBuffer;
			wxString * temp = new wxString;
			*temp = temp2.Left(bytesRead);
			wxCommandEvent event(EVT_CONTROLLER);
			event.SetId(m_id);
			event.SetClientData((void *)temp);
			wxPostEvent(m_target, event);	
		}
	}while (bytesRead > 0);

#else	

	DWORD dwBytesRead;
	BOOL bResult;
	char szBuffer [1001];
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
	return 0;
}
