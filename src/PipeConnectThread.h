// $Id$
// PipeConnectThread.h: interface for the PipeConnectThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPECONNECTTHREAD_H__96567092_B704_4993_9936_3F906199C89C__INCLUDED_)
#define AFX_PIPECONNECTTHREAD_H__96567092_B704_4993_9936_3F906199C89C__INCLUDED_

#include "wx/thread.h"
#include "wx/process.h"
#include "wx/window.h"
#include "wx/txtstrm.h"
#include <windows.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PipeConnectThread : public wxThread
{
	public:
		PipeConnectThread(wxWindow * target);
		void SetHandle(HANDLE pipeHandle);
		virtual ~PipeConnectThread();
		wxThread::ExitCode Entry();
	private:
		wxWindow * m_target;
		bool m_Connected;
		HANDLE m_pipeHandle;
};

#endif // !defined(AFX_PIPECONNECTTHREAD_H__96567092_B704_4993_9936_3F906199C89C__INCLUDED_)
