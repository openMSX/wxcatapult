// $Id: PipeReadThread.h,v 1.4 2004/04/18 15:50:55 h_oudejans Exp $
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
		PipeReadThread(wxWindow * target, int id, wxThreadKind kind=wxTHREAD_DETACHED);
#ifdef __WINDOWS__	
		void SetHandle (HANDLE hTarget) {m_hTarget=hTarget;}
#else
		void SetFileDescriptor (int descriptor) {m_descriptor = descriptor;};
#endif
		wxThread::ExitCode Entry();
		virtual ~PipeReadThread();

	private:
		int m_id;
		wxWindow * m_target;

#ifdef __WINDOWS__	
		HANDLE m_hTarget;
#else 
		int m_descriptor;
#endif
};

#endif // !defined(AFX_PIPEREADTHREAD_H__9CF545E1_3449_11D8_B9CF_00104B4B187E__INCLUDED_)
