// $Id: openMSXController.h,v 1.6 2004/03/03 18:06:27 h_oudejans Exp $
// openMSXController.h: interface for the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
#define AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

using std::list;

class wxCatapultFrame;
class CatapultXMLParser;
class openMSXController  
{
	public:
		wxString FilterCurrentValue(wxString);
		void StartOpenMSX (wxString cmd, bool hidden=false);
		bool WriteCommand(wxString msg);
		void HandleParsedOutput (wxCommandEvent &event);
		void HandleStdErr (wxCommandEvent & event);
		void HandleStdOut (wxCommandEvent &event);
		void HandleEndProcess (wxCommandEvent & event);
		virtual bool WriteMessage (wxString msg)=0;
		virtual bool Launch (wxString cmdline)=0;
		virtual bool HandleMessage (wxCommandEvent &event);
		openMSXController(wxWindow * target);
		virtual ~openMSXController();
		bool m_openMsxRunning;
		wxString GetInfoCommand (wxString parameter); 
	protected:
		enum LaunchMode{
			LAUNCH_NONE,
			LAUNCH_HIDDEN,
			LAUNCH_NORMAL
		};
		bool PostLaunch ();
		bool PreLaunch();
		CatapultXMLParser * m_parser;
		wxCatapultFrame * m_appWindow;
		LaunchMode m_launchMode;
	private:
		unsigned int m_openMSXID;
		wxString m_infoCommand;
		wxString GetPendingCommand();
		void HandleHiddenLaunchReply(wxCommandEvent &event);
		void HandleNormalLaunchReply(wxCommandEvent &event);
		list<wxString> m_commands;		
};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
