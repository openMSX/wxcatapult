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
#ifdef __UNIX__
	ssize_t bytesRead;
	char szBuffer[1001];
	do {
		bytesRead = read(m_descriptor, szBuffer, 1000);
		if (bytesRead > 0) {
			szBuffer[bytesRead] = 0;
			wxString* eventClientData = new wxString(szBuffer, wxConvUTF8, bytesRead);
			wxCommandEvent event(EVT_CONTROLLER);
			event.SetId(m_id);
			event.SetClientData(eventClientData);
			wxPostEvent(m_target, event);
		}
	} while (bytesRead > 0);
	close (m_descriptor);
#else
	DWORD dwBytesRead;
	BOOL bResult;
	char szBuffer [1001];
	do {
		bResult = ReadFile(m_hTarget, szBuffer, 1000, &dwBytesRead, nullptr);
		if (bResult) { // the bytes could not be read
			wxString* eventClientData = new wxString(szBuffer, wxConvUTF8, dwBytesRead);
			wxCommandEvent event(EVT_CONTROLLER);
			event.SetId(m_id);
			event.SetClientData(eventClientData);
			wxPostEvent(m_target, event);
		}
	} while (bResult);

#endif
	wxCommandEvent endEvent(EVT_CONTROLLER);
	endEvent.SetId(MSGID_ENDPROCESS);
	wxPostEvent(m_target, endEvent);
	return 0;
}
