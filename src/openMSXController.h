// $Id: openMSXController.h,v 1.28 2004/11/11 17:15:00 h_oudejans Exp $
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

#define LAUNCHSCRIPT_MAXSIZE 100

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
		void StartOpenMSX (wxString cmd, bool getversion=false);
		bool WriteCommand(wxString msg, TargetType target = TARGET_INTERACTIVE);
		void HandleParsedOutput (wxCommandEvent &event);
		void HandleStdErr (wxCommandEvent & event);
		void HandleStdOut (wxCommandEvent &event);
		void HandleEndProcess (wxCommandEvent & event);
		bool SetupOpenMSXParameters (wxString version);
		virtual bool WriteMessage (wxString msg)=0;
		virtual bool Launch (wxString cmdline)=0;
		virtual void HandleNativeEndProcess ()=0;
		virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd)=0;
		virtual bool HandleMessage (wxCommandEvent &event);
		openMSXController(wxWindow * target);
		virtual ~openMSXController();
		
		wxString GetInfoCommand (wxString parameter);
		bool IsOpenMSXRunning() {return m_openMsxRunning;};
		void InitLaunchScript ();
	protected:
		wxCatapultFrame * m_appWindow;
		bool m_openMsxRunning;
		bool PostLaunch ();
		bool PreLaunch();
		CatapultXMLParser * m_parser;
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
		void openMSXController::HandleLaunchReply (wxString cmd, wxCommandEvent * event,
			LaunchInstructionType instruction,
			int * sendStep, int loopcount,
			wxString datalist);
		int UpdateSetting (wxString data,wxString setting);
		int FillComboBox (wxString data,wxString setting);
		int EnableFirmware (wxString dummy, wxString cmd);
		int EnableRenShaTurbo (wxString data, wxString cmd);
		int InitConnectors(wxString connectors,wxString dummy);
		int InitPluggables(wxString pluggables,wxString dummy);
		int InitSoundDevices (wxString data, wxString dummy);
		int SetChannelType (wxString data,wxString name);
		int AddPluggableDescription(wxString data,wxString name);
		int AddPluggableClass(wxString data,wxString name);
		int AddConnectorClass(wxString data,wxString name);
		int AddConnectorContents(wxString data,wxString name);
		int SetSliderDefaults (wxString dummy1, wxString dummy2);
		int InitAudioConnectorPanel (wxString dummy1, wxString dummy2);
		int InitConnectorPanel (wxString dummy1, wxString dummy2);
		int EnableCassettePort (wxString data, wxString cmd);

		list<CommandEntry> m_commands;
};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
