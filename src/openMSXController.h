// $Id: openMSXController.h,v 1.15 2004/04/04 19:47:15 h_oudejans Exp $
// openMSXController.h: interface for the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
#define AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "CatapultPage.h"

#define LAUNCHSCRIPT_MAXSIZE 100

using std::list;

class wxCatapultFrame;
class CatapultXMLParser;
class openMSXController  
{
	public:
		wxString FilterCurrentValue(wxString);
		void GetConnectors (wxArrayString & connectors);
		void GetPluggables (wxArrayString & pluggables);
		void GetPluggableDescriptions (wxArrayString & descriptions);
		void StartOpenMSX (wxString cmd, bool getversion=false);
		bool WriteCommand(wxString msg);
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
		bool m_openMsxRunning;
		wxString GetInfoCommand (wxString parameter);
		wxString GetExistCommand (wxString parameter);
		void InitLaunchScript ();
		
	protected:
		wxCatapultFrame * m_appWindow;
		enum LaunchMode{
			LAUNCH_NONE,
			LAUNCH_NORMAL
		};
		bool PostLaunch ();
		bool PreLaunch();
		CatapultXMLParser * m_parser;
		LaunchMode m_launchMode;
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
			bool (openMSXController::*p_okfunction)(wxString,wxString); 
			bool showError;
		};
		
		LaunchInstructionType * m_launchScript;
		int m_launchScriptSize;
	
		void AddLaunchInstruction (wxString cmd, wxString action, wxString parameter, 
				bool (openMSXController::*pfunction)(wxString,wxString),
				bool showError);
		unsigned int m_openMSXID;
		wxArrayString m_connectors;
		wxArrayString m_pluggables;
		wxArrayString m_pluggabledescriptions;
		wxString m_infoCommand;
		wxString m_unsetCommand;
		wxString GetPendingCommand();
		wxString PeekPendingCommand();
		
				
		void HandleNormalLaunchReply(wxCommandEvent &event);
		
		void newLaunchReply (wxCommandEvent & event);
		void executeLaunch (wxCommandEvent * event = NULL);
		void FinishLaunch ();
		int tokenize (wxString text, wxString seperator, wxArrayString & result);
		wxString translate(wxArrayString tokens, int loop, wxString lastdata);
		void openMSXController::HandleLaunchReply (wxString cmd, wxCommandEvent * event,
													LaunchInstructionType instruction,
													int * sendStep, int loopcount,
													wxString datalist);
		bool UpdateSetting (wxString data,wxString setting);
		bool FillComboBox (wxString data,wxString setting);
		bool EnableFirmware (wxString dummy, wxString cmd);
		bool InitConnectors(wxString connectors,wxString dummy);
		bool InitPluggables(wxString pluggables,wxString dummy);
		bool InitSoundDevices (wxString data, wxString dummy);
		bool SetChannelType (wxString data,wxString name);
		bool AddPluggableDescription(wxString data,wxString name);
		bool SetSliderDefaults (wxString dummy1, wxString dummy2);
		bool InitAudioConnectorPanel (wxString dummy1, wxString dummy2);
		bool EnableAutoFrameSkip (wxString data, wxString cmd);

		list<wxString> m_commands;		
};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
