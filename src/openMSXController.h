#ifndef OPENMSXCONTROLLER_H
#define OPENMSXCONTROLLER_H

#ifdef __WXMSW__
#ifdef _MSC_VER // we don't want gcc to warn about pragma's
#pragma warning( disable : 4786) // we don't want 39 warnings about browse info names getting too long
#endif
#endif

#include "CatapultXMLParser.h"
#include <wx/arrstr.h>
#include <deque>
#include <functional>
#include <vector>
#ifdef __WXMSW__
// See: http://wiki.wxwidgets.org/WxMSW_Issues#The_windows.h_Header_File.2C_Macros_and_Compiling_Errors
#include <windows.h>
#include <wx/msw/winundef.h>
#else
#include <string>
#endif


class wxCatapultFrame;
class wxCommandEvent;
class CatapultXMLParser;
class PipeConnectThread;
class PipeReadThread;

class openMSXController
{
public:
	openMSXController(wxWindow* target);
	~openMSXController();

	void RaiseOpenMSX();
	void RestoreOpenMSX();

	void UpdateMixer();
	wxString GetConnectorClass(const wxString& name) const;
	const wxArrayString& GetConnectors() const { return m_connectors; }
	const wxArrayString& GetPluggables() const { return m_pluggables; }
	const wxArrayString& GetPluggableDescriptions() const { return m_pluggabledescriptions; }
	const wxArrayString& GetPluggableClasses() const { return m_pluggableclasses; }
	bool StartOpenMSX(wxString cmd, bool getversion = false);
	void WriteCommand(
		const wxString& command,
		std::function<void (const wxString&, const wxString&)> okCallback = nullptr,
		std::function<void (const wxString&, const wxString&)> errorCallback = nullptr);
	void HandleEndProcess(wxCommandEvent& event);
	bool HandleMessage(wxCommandEvent& event);

	bool IsOpenMSXRunning() const { return m_openMsxRunning; }

private:
	struct LaunchInstruction {
		wxString command;
		std::function<void (const wxString&, const wxString&)> callback;
	};

	void WriteMessage(const xmlChar* msg, size_t length);
	void commandError(const wxString& cmd, const wxString& result);
	bool Launch(wxString cmdline);
	void HandleNativeEndProcess();
	wxString GetOpenMSXVersionInfo(wxString openmsxCmd);

	void PostLaunch();
	void InitLaunchScript();
	void AddCommand(
		const wxString& cmd,
		std::function<void (const wxString&, const wxString&)> callback = nullptr);

	bool SetupOpenMSXParameters(wxString version);
	void HandleParsedOutput(wxCommandEvent& event);
	void HandleStdErr(wxCommandEvent& event);
	void HandleStdOut(wxCommandEvent& event);
	void ExecuteStart(int startLine = 0);
	void ExecuteLaunch(const wxString& command, const wxString& result, bool ok);
	void ExecuteNext();
	void FinishLaunch();
	void UpdateSetting2   (const wxString& name,    const wxString& result);
	void UpdateSetting    (const wxString& cmd,     const wxString& result);
	void EnableFirmware   (const wxString& cmd,     const wxString& result);
	void FillComboBox     (const wxString& control, const wxString& result);
	void FillRangeComboBox(const wxString& control, const wxString& result);
	void HandlePluggables  (const wxString& result);
	void HandleConnectors  (const wxString& result);
	void HandleSoundDevices(const wxString& result);

	void AddSetting(
		const wxString& settings,
		std::function<void (const wxString&, const wxString&)> callback);
	void UpdateToggle   (const wxString& v, const wxString& c, int f = 0);
	void UpdateCombo    (const wxString& v, const wxString& c, int f = 0);
	void UpdateIndicator(const wxString& v, const wxString& c, int f = 0);
	void UpdateSlider   (const wxString& v, const wxString& c);
	void UpdatePluggable(const wxString& v, const wxString& c);

	struct SettingElement {
		wxString setting;
		std::function<void (const wxString&, const wxString&)> callback;
	};
	std::vector<SettingElement> m_settingTable;

	std::vector<LaunchInstruction> m_launchScript;
	int sendStep;
	int m_relaunch;

	unsigned m_openMSXID;
	wxArrayString m_connectors;
	wxArrayString m_connectorclasses;
	wxArrayString m_pluggables;
	wxArrayString m_pluggabledescriptions;
	wxArrayString m_pluggableclasses;

	struct CommandEntry {
		wxString command;
		std::function<void (const wxString&, const wxString&)> okCallback;
		std::function<void (const wxString&, const wxString&)> errorCallback;
	};
	std::deque<CommandEntry> m_commands;

	wxCatapultFrame* m_appWindow;
	bool m_openMsxRunning;
	CatapultXMLParser* m_parser;

	// windows or linux specific stuff
#ifdef __WXMSW__
	struct FindOpenmsxInfo {
		LPPROCESS_INFORMATION ProcessInfo;
		HWND hWndFound;
	};

	void HandlePipeCreated();

	HWND FindOpenMSXWindow();
	static BOOL CALLBACK EnumWindowCallBack(HWND hwnd, LPARAM lParam);
	void CloseHandles(
		bool useNamedPipes, HANDLE hThread, HANDLE hInputRead,
		HANDLE hOutputWrite, HANDLE hErrorWrite);
	void ShowError(const wxString& msg);
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
#else
	bool execute(const std::string& command, int& fdIn, int& fdOut, int& fdErr);

	int m_openMSXstdin;
	int m_openMSXstdout;
	int m_openMSXstderr;
	PipeReadThread* m_stdErrThread;
	PipeReadThread* m_stdOutThread;
#endif
};

#endif
