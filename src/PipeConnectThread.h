#ifndef PIPECONNECTTHREAD_H
#define PIPECONNECTTHREAD_H

#include "wx/thread.h"
#include "wx/process.h"
#include "wx/window.h"
#include "wx/txtstrm.h"
#include <windows.h>

class PipeConnectThread : public wxThread
{
public:
	PipeConnectThread(wxWindow* target);

	void SetHandle(HANDLE pipeHandle);
	wxThread::ExitCode Entry();

private:
	wxWindow* m_target;
	bool m_Connected;
	HANDLE m_pipeHandle;
};

#endif
