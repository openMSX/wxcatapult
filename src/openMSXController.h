// $Id: openMSXController.h,v 1.12 2004/03/27 20:37:15 h_oudejans Exp $
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
		virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd)=0;
		virtual bool HandleMessage (wxCommandEvent &event);
		openMSXController(wxWindow * target);
		virtual ~openMSXController();
		bool m_openMsxRunning;
		wxString GetInfoCommand (wxString parameter);
		wxString GetExistCommand (wxString parameter);
		
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
		unsigned int m_openMSXID;
		wxArrayString m_connectors;
		wxArrayString m_pluggables;
		wxArrayString m_pluggabledescriptions;
		wxString m_infoCommand;
		wxString m_unsetCommand;
		wxString GetPendingCommand();
		wxString PeekPendingCommand();
		void InitConnectors(wxString connectors);
		void InitPluggables(wxString pluggables);		
		void HandleNormalLaunchReply(wxCommandEvent &event);
		list<wxString> m_commands;		
};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
