#ifndef PIPEREADTHREAD_H
#define PIPEREADTHREAD_H

#include "wx/thread.h"
#include "wx/process.h"
#include "wx/window.h"
#include "wx/txtstrm.h"
#ifdef __WXMSW__
#include <windows.h>
#endif

class PipeReadThread : public wxThread
{
public:
	PipeReadThread(wxWindow * target, int id, wxThreadKind kind=wxTHREAD_DETACHED);
#ifdef __WXMSW__
	void SetHandle (HANDLE hTarget) {m_hTarget=hTarget;}
#else
	void SetFileDescriptor (int descriptor) {m_descriptor = descriptor;};
#endif
	wxThread::ExitCode Entry();
	virtual ~PipeReadThread();

private:
	int m_id;
	wxWindow * m_target;

#ifdef __WXMSW__
	HANDLE m_hTarget;
#else
	int m_descriptor;
#endif
};

#endif
