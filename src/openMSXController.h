// $Id: openMSXController.h,v 1.2 2004/02/04 22:01:15 manuelbi Exp $
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
	protected:

		bool PostLaunch ();
		bool PreLaunch();
		CatapultXMLParser * m_parser;
		wxCatapultFrame * m_appWindow;
	private:
		wxString m_infoCommand;
		enum LaunchMode{
			LAUNCH_NONE,
			LAUNCH_HIDDEN,
			LAUNCH_NORMAL
		};
		
		wxString GetPendingCommand();
		void HandleHiddenLaunchReply(wxCommandEvent &event);
		void HandleNormalLaunchReply(wxCommandEvent &event);
		list<wxString> m_commands;
		LaunchMode m_launchMode;
};

#endif // !defined(AFX_OPENMSXCONTROLLER_H__B093351A_1D73_4989_87F1_59EC5C871497__INCLUDED_)
