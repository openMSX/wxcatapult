// $Id$
// openMSXController.h: interface for the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
#define AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __WXMSW__
#ifdef _MSC_VER // we don't want gcc to warn about pragma's
#pragma warning( disable : 4786) // we don't want 39 warnings about browse info names getting too long
#endif
#endif

#include <list>
#include "CatapultPage.h"
#include "CatapultXMLParser.h"
#include <wx/socket.h>

#define LAUNCHSCRIPT_MAXSIZE 100
#define OPENMSX_SOCKET 1

using std::list;

class wxCatapultFrame;
class CatapultXMLParser;
class openMSXController
{
	public:
		enum TargetType{
			TARGET_STARTUP,
			TARGET_INTERACTIVE
		};
		void UpdateMixer ();
		void GetConnectors (wxArrayString & connectors);
		wxString GetConnectorClass (wxString name);
		wxString GetConnectorContents (wxString name);
		void GetPluggables (wxArrayString & pluggables);
		void GetPluggableDescriptions (wxArrayString & descriptions);
		void GetPluggableClasses (wxArrayString & classes);
		bool StartOpenMSX (wxString cmd, bool getversion=false);
		bool WriteCommand(wxString msg, TargetType target = TARGET_INTERACTIVE);
		void HandleParsedOutput (wxCommandEvent &event);
		void HandleStdErr (wxCommandEvent & event);
		void HandleStdOut (wxCommandEvent &event);
		void HandleEndProcess (wxCommandEvent & event);
		void HandleSocketEvent (wxSocketEvent & event);
		bool SetupOpenMSXParameters (wxString version);
		virtual bool WriteMessage (xmlChar * msg,size_t length)=0;
		virtual bool Launch (wxString cmdline)=0;
		virtual void HandleNativeEndProcess ()=0;
		virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd)=0;
		virtual bool HandleMessage (wxCommandEvent &event);
		openMSXController(wxWindow * target);
		virtual ~openMSXController();

		bool IsOpenMSXRunning() {return m_openMsxRunning;};
		void InitLaunchScript ();
	protected:
		wxCatapultFrame * m_appWindow;
		bool m_openMsxRunning;
		bool PostLaunch ();
		bool PreLaunch();
		CatapultXMLParser * m_parser;
		wxSocketClient * m_socket;
	private:
		bool wait;
		int sendStep;
		int recvStep;
		int sendLoop;
		int recvLoop;
		wxString lastdata;
		struct LaunchInstructionType{
			wxString command;
			wxString scriptActions;
			wxString parameter;
			int (openMSXController::*p_okfunction)(wxString,wxString);
			bool showError;
		};
		struct CommandEntry{
			wxString command;
			TargetType target;
		};

		LaunchInstructionType * m_launchScript;
		int m_launchScriptSize;
		int m_relaunch;

		void AddLaunchInstruction (wxString cmd, wxString action, wxString parameter,
				int (openMSXController::*pfunction)(wxString,wxString),
				bool showError);
		unsigned int m_openMSXID;
		wxArrayString m_connectors;
		wxArrayString m_connectorclasses;
		wxArrayString m_connectorcontents;
		wxArrayString m_pluggables;
		wxArrayString m_pluggabledescriptions;
		wxArrayString m_pluggableclasses;
		wxString GetPendingCommand();
		wxString PeekPendingCommand();
		enum TargetType PeekPendingCommandTarget();

		void HandleNormalLaunchReply(wxCommandEvent &event);
		void newLaunchReply (wxCommandEvent & event);
		void executeLaunch (wxCommandEvent * event = NULL, int startLine = 0);
		void FinishLaunch ();
		int tokenize (wxString text, wxString seperator, wxArrayString & result);
		wxString translate(wxArrayString tokens, int loop, wxString lastdata);
		void HandleLaunchReply (wxString cmd, wxCommandEvent * event,
			LaunchInstructionType instruction,
			int * sendStep, int loopcount,
			wxString datalist);
		int UpdateSetting (wxString setting,wxString data);
		int FillComboBox (wxString setting,wxString data);
		int FillRangeComboBox (wxString setting, wxString data);
		int EnableFirmware (wxString cmd, wxString data);
		int EnableRenShaTurbo (wxString cmd, wxString data);
		int EnableMainWindow (wxString dummy1, wxString dummy2);
		int InitConnectors(wxString dummy,wxString connectors);
		int InitPluggables(wxString dummy,wxString pluggables);
		int InitSoundDevices (wxString dummy, wxString data);
		int InitRomTypes (wxString dummy, wxString data);
		int SetRomDescription (wxString name, wxString data);
		int SetChannelType (wxString name,wxString data);
		int AddPluggableDescription(wxString name,wxString data);
		int AddPluggableClass(wxString name,wxString data);
		int AddConnectorClass(wxString name,wxString data);
		int AddConnectorContents(wxString name,wxString data);
		int SetSliderDefaults (wxString dummy1, wxString dummy2);
		int InitAudioConnectorPanel (wxString dummy1, wxString dummy2);
		int InitConnectorPanel (wxString dummy1, wxString dummy2);
		int EnableCassettePort (wxString cmd, wxString data);
		int SetCassetteMode (wxString cmd, wxString data);
		bool connectSocket();

		list<CommandEntry> m_commands;

};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
