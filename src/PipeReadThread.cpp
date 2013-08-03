#include "PipeReadThread.h"
#include "wxCatapultApp.h"
#include <wx/wxprec.h>
#ifdef __UNIX__
#include <unistd.h>
#endif

PipeReadThread::PipeReadThread(wxWindow* target, int id, wxThreadKind kind)
	: wxThread(kind)
{
	m_target = target;
	m_id = id;
}

wxThread::ExitCode PipeReadThread::Entry()
{
	while (true) {
		char buffer[1000];
#ifdef __UNIX__
		ssize_t bytesRead = read(m_descriptor, buffer, 1000);
		if (bytesRead <= 0) break;
#else
		DWORD bytesRead;
		if (!ReadFile(m_hTarget, buffer, 1000, &bytesRead, nullptr)) break;
#endif
		wxString* eventClientData = new wxString(buffer, wxConvUTF8, bytesRead);
		wxCommandEvent event(EVT_CONTROLLER);
		event.SetId(m_id);
		event.SetClientData(eventClientData);
		wxPostEvent(m_target, event);
	}
#ifdef __UNIX__
	close(m_descriptor);
#endif
	wxCommandEvent endEvent(EVT_CONTROLLER);
	endEvent.SetId(MSGID_ENDPROCESS);
	wxPostEvent(m_target, endEvent);
	return 0;
}
