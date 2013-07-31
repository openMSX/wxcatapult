#ifndef OPENMSXWINDOWSCONTROLLER_H
#define OPENMSXWINDOWSCONTROLLER_H

#include "openMSXController.h"
#include <windows.h>

class PipeConnectThread;

class openMSXWindowsController : public openMSXController
{
public:
	openMSXWindowsController(wxWindow* target);
	virtual ~openMSXWindowsController();

	void RaiseOpenMSX();
	void RestoreOpenMSX();
	virtual bool HandleMessage(wxCommandEvent& event);

private:
	struct FindOpenmsxInfo {
		LPPROCESS_INFORMATION ProcessInfo;
		HWND hWndFound;
	};

	virtual bool WriteMessage(xmlChar* msg, size_t length);
	virtual bool Launch(wxString cmdLine);
	virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd);
	virtual void HandleNativeEndProcess() {};
	void HandleEndProcess(wxCommandEvent& event);
	void HandlePipeCreated();

	HWND FindOpenMSXWindow();
	static BOOL CALLBACK EnumWindowCallBack(HWND hwnd, LPARAM lParam);
	void CloseHandles(
		bool useNamedPipes, HANDLE hThread, HANDLE hInputRead,
		HANDLE hOutputWrite, HANDLE hErrorWrite);
	void ShowError(wxString msg);
	bool CreatePipes(
		bool useNamedPipes, HANDLE* input, HANDLE* output, HANDLE* error,
		HANDLE* outputWrite, HANDLE* errorWrite);
	wxString CreateControlParameter(bool useNamedPipes);
	bool DetermenNamedPipeUsage();

	HANDLE m_outputHandle;
	HANDLE m_namedPipeHandle;
	HWND m_catapultWindow;
	PROCESS_INFORMATION m_openmsxProcInfo;
	bool m_pipeActive;
	unsigned long m_launchCounter;
	PipeConnectThread* m_connectThread;
};

#endif
