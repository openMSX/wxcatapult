// $Id$
// PipeReadThread.h: interface for the PipeReadThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPEREADTHREAD_H__9CF545E1_3449_11D8_B9CF_00104B4B187E__INCLUDED_)
#define AFX_PIPEREADTHREAD_H__9CF545E1_3449_11D8_B9CF_00104B4B187E__INCLUDED_

#include "wx/thread.h"
#include "wx/process.h"
#include "wx/window.h"
#include "wx/txtstrm.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PipeReadThread : public wxThread  
{
	public:
		PipeReadThread(wxWindow * target, int id, wxProcess * process, wxThreadKind kind=wxTHREAD_DETACHED);
#ifdef __WINDOWS__	
		void SetHandle (HANDLE hTarget) {m_hTarget=hTarget;}
#endif
		wxThread::ExitCode Entry();
		virtual ~PipeReadThread();

	private:
		int m_id;
		wxWindow * m_target;
		wxProcess * m_process;
		wxTextInputStream * m_inputstream;
#ifdef __WINDOWS__	
		HANDLE m_hTarget;
#endif
};

#endif // !defined(AFX_PIPEREADTHREAD_H__9CF545E1_3449_11D8_B9CF_00104B4B187E__INCLUDED_)
