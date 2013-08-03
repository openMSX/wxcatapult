#ifndef OPENMSXCONTROLLER_H
#define OPENMSXCONTROLLER_H

#ifdef __WXMSW__
#ifdef _MSC_VER // we don't want gcc to warn about pragma's
#pragma warning( disable : 4786) // we don't want 39 warnings about browse info names getting too long
#endif
#endif

#include "CatapultXMLParser.h"
#include <wx/arrstr.h>
#include <list>
#include <vector>
#ifdef __WXMSW__
#include <windows.h>
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
	enum TargetType {
		TARGET_STARTUP,
		TARGET_INTERACTIVE
	};

	openMSXController(wxWindow* target);
	~openMSXController();

	void RaiseOpenMSX();
	void RestoreOpenMSX();

	void UpdateMixer();
	void GetConnectors(wxArrayString& connectors);
	wxString GetConnectorClass(wxString name);
	void GetPluggables(wxArrayString& pluggables);
	void GetPluggableDescriptions(wxArrayString& descriptions);
	void GetPluggableClasses(wxArrayString& classes);
	bool StartOpenMSX(wxString cmd, bool getversion = false);
	void WriteCommand(wxString msg, TargetType target = TARGET_INTERACTIVE);
	void HandleEndProcess(wxCommandEvent& event);
	bool HandleMessage(wxCommandEvent& event);

	bool IsOpenMSXRunning() const { return m_openMsxRunning; }

private:
	struct LaunchInstruction {
		wxString command;
		wxString action;
		wxString parameter;
		int (openMSXController::*p_okfunction)(wxString, wxString);
		bool showError;
	};
	struct CommandEntry {
		wxString command;
		TargetType target;
	};

	void WriteMessage(xmlChar* msg, size_t length);
	bool Launch(wxString cmdline);
	void HandleNativeEndProcess();
	wxString GetOpenMSXVersionInfo(wxString openmsxCmd);

	bool PostLaunch();
	bool PreLaunch();
	void InitLaunchScript();
	void AddLaunchInstruction(
		wxString cmd, wxString action, wxString parameter,
		int (openMSXController::*pfunction)(wxString, wxString),
		bool showError);

	wxString GetPendingCommand();
	wxString PeekPendingCommand();
	TargetType PeekPendingCommandTarget();

	bool SetupOpenMSXParameters(wxString version);
	void HandleParsedOutput(wxCommandEvent& event);
	void HandleStdErr(wxCommandEvent& event);
	void HandleStdOut(wxCommandEvent& event);
	void HandleNormalLaunchReply(wxCommandEvent& event);
	void executeLaunch(wxCommandEvent* event = nullptr, int startLine = 0);
	void FinishLaunch();
	size_t tokenize(const wxString& text, const wxString& seperator,
	                wxArrayString& result);
	wxString translate(wxArrayString tokens, int loop, wxString lastdata);
	void HandleLaunchReply(wxString cmd, wxCommandEvent* event,
		LaunchInstruction instruction,
		int* sendStep, int loopcount, wxString datalist);
	int UpdateSetting(wxString setting, wxString data);
	int FillComboBox(wxString setting, wxString data);
	int FillRangeComboBox(wxString setting, wxString data);
	int EnableFirmware(wxString cmd, wxString data);
	int EnableRenShaTurbo(wxString cmd, wxString data);
	int EnableMainWindow(wxString dummy1, wxString dummy2);
	int InitConnectors(wxString dummy, wxString connectors);
	int InitPluggables(wxString dummy, wxString pluggables);
	int InitSoundDevices(wxString dummy, wxString data);
	int InitRomTypes(wxString dummy, wxString data);
	int SetRomDescription(wxString name, wxString data);
	int SetChannelType(wxString name, wxString data);
	int SetChannelTypeDone(wxString name, wxString data);
	int AddPluggableDescription(wxString name, wxString data);
	int AddPluggableClass(wxString name, wxString data);
	int AddConnectorClass(wxString name, wxString data);
	int SetSliderDefaults(wxString dummy1, wxString dummy2);
	int InitAudioConnectorPanel(wxString dummy1, wxString dummy2);
	int InitConnectorPanel(wxString dummy1, wxString dummy2);
	int EnableCassettePort(wxString cmd, wxString data);
	int SetCassetteMode(wxString cmd, wxString data);

	void AddSetting(
		const wxString& settings, const wxString& control,
		void (openMSXController::*pfunction)(const wxString&, const wxString&, const wxString&, int),
		int flags = 0);
	void UpdateSetting2(const wxString& name, const wxString& data);
	void UpdateToggle   (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateCombo    (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateIndicator(const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateSlider   (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateVolume   (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateBalance  (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateMenu     (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdateMidiPlug (const wxString& setting, const wxString& data, const wxString& control, int flags);
	void UpdatePluggable(const wxString& setting, const wxString& data, const wxString& control, int flags);

	struct SettingElement {
		wxString setting;
		wxString control;
		void (openMSXController::*pfunction)(const wxString&, const wxString&, const wxString&, int);
		int flags;
	};
	std::vector<SettingElement> m_settingTable;

	bool wait;
	int sendStep;
	int recvStep;
	int sendLoop;
	int recvLoop;
	wxString lastdata;

	std::vector<LaunchInstruction> m_launchScript;
	int m_relaunch;

	unsigned m_openMSXID;
	wxArrayString m_connectors;
	wxArrayString m_connectorclasses;
	wxArrayString m_pluggables;
	wxArrayString m_pluggabledescriptions;
	wxArrayString m_pluggableclasses;

	std::list<CommandEntry> m_commands;

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
