#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/wx.h>
#include "wxCatapultApp.h"
#include "PipeConnectThread.h"

PipeConnectThread::PipeConnectThread(wxWindow* target)
{
	m_Connected = false;
	m_target = target;
}

wxThread::ExitCode PipeConnectThread::Entry()
{
	if (!ConnectNamedPipe(m_pipeHandle, nullptr)) {
		wxString text;
		text.sprintf(wxT("Error connection pipe: %ld"), GetLastError());
		wxMessageBox(text);
	}
	wxCommandEvent endEvent(EVT_CONTROLLER);
	endEvent.SetId(MSGID_PIPECREATED);
	wxPostEvent(m_target, endEvent);
	return 0;
}

void PipeConnectThread::SetHandle(HANDLE pipeHandle)
{
	m_pipeHandle = pipeHandle;
}
