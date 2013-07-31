#ifndef OPENMSXCONTROLLER_H
#define OPENMSXCONTROLLER_H

#ifdef __WXMSW__
#ifdef _MSC_VER // we don't want gcc to warn about pragma's
#pragma warning( disable : 4786) // we don't want 39 warnings about browse info names getting too long
#endif
#endif

#include "CatapultPage.h"
#include "CatapultXMLParser.h"
#include <list>
#include <wx/socket.h>

#define OPENMSX_SOCKET 1

class wxCatapultFrame;
class CatapultXMLParser;
class openMSXController
{
public:
	enum TargetType {
		TARGET_STARTUP,
		TARGET_INTERACTIVE
	};

	openMSXController(wxWindow* target);
	virtual ~openMSXController();

	void UpdateMixer();
	void GetConnectors(wxArrayString& connectors);
	wxString GetConnectorClass(wxString name);
	wxString GetConnectorContents(wxString name);
	void GetPluggables(wxArrayString& pluggables);
	void GetPluggableDescriptions(wxArrayString& descriptions);
	void GetPluggableClasses(wxArrayString& classes);
	bool StartOpenMSX(wxString cmd, bool getversion = false);
	bool WriteCommand(wxString msg, TargetType target = TARGET_INTERACTIVE);
	void HandleParsedOutput(wxCommandEvent& event);
	void HandleStdErr(wxCommandEvent& event);
	void HandleStdOut(wxCommandEvent& event);
	void HandleEndProcess(wxCommandEvent& event);
	void HandleSocketEvent(wxSocketEvent& event);
	bool SetupOpenMSXParameters(wxString version);
	virtual bool WriteMessage(xmlChar* msg, size_t length) = 0;
	virtual bool Launch(wxString cmdline) = 0;
	virtual void HandleNativeEndProcess() = 0;
	virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd) = 0;
	virtual bool HandleMessage(wxCommandEvent& event);

	bool IsOpenMSXRunning() const { return m_openMsxRunning; }
	void InitLaunchScript();

protected:
	wxCatapultFrame* m_appWindow;
	bool m_openMsxRunning;
	bool PostLaunch ();
	bool PreLaunch();
	CatapultXMLParser* m_parser;
	wxSocketClient* m_socket;

private:
	struct LaunchInstructionType {
		wxString command;
		wxString scriptActions;
		wxString parameter;
		int (openMSXController::*p_okfunction)(wxString, wxString);
		bool showError;
	};
	struct CommandEntry {
		wxString command;
		TargetType target;
	};

	void AddLaunchInstruction(
		wxString cmd, wxString action, wxString parameter,
		int (openMSXController::*pfunction)(wxString, wxString),
		bool showError);

	wxString GetPendingCommand();
	wxString PeekPendingCommand();
	enum TargetType PeekPendingCommandTarget();

	void HandleNormalLaunchReply(wxCommandEvent& event);
	void newLaunchReply(wxCommandEvent& event);
	void executeLaunch(wxCommandEvent* event = nullptr, int startLine = 0);
	void FinishLaunch();
	size_t tokenize(const wxString& text, const wxString& seperator,
	                wxArrayString& result);
	wxString translate(wxArrayString tokens, int loop, wxString lastdata);
	void HandleLaunchReply(wxString cmd, wxCommandEvent* event,
		LaunchInstructionType instruction,
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
	int AddPluggableDescription(wxString name, wxString data);
	int AddPluggableClass(wxString name, wxString data);
	int AddConnectorClass(wxString name, wxString data);
	int AddConnectorContents(wxString name, wxString data);
	int SetSliderDefaults(wxString dummy1, wxString dummy2);
	int InitAudioConnectorPanel(wxString dummy1, wxString dummy2);
	int InitConnectorPanel(wxString dummy1, wxString dummy2);
	int EnableCassettePort(wxString cmd, wxString data);
	int SetCassetteMode(wxString cmd, wxString data);
	bool connectSocket();

	bool wait;
	int sendStep;
	int recvStep;
	int sendLoop;
	int recvLoop;
	wxString lastdata;

	LaunchInstructionType* m_launchScript;
	int m_launchScriptSize;
	int m_relaunch;

	unsigned m_openMSXID;
	wxArrayString m_connectors;
	wxArrayString m_connectorclasses;
	wxArrayString m_connectorcontents;
	wxArrayString m_pluggables;
	wxArrayString m_pluggabledescriptions;
	wxArrayString m_pluggableclasses;

	std::list<CommandEntry> m_commands;
};

#endif
