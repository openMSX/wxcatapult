// $Id: wxCatapultApp.h,v 1.8 2004/05/09 14:25:51 manuelbi Exp $
#ifndef wxCatapultApp_H
#define wxCatapultApp_H

#define OPENMSX_DEMO_CD_VERSION	0 // set to 1 to make a special cd-version (oss 2004)
#define FORCE_UNNAMED_PIPES 0

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef SETUP_EXTERNALS
#define SCOPE
#else
#define SCOPE extern
#endif

// Globals

SCOPE int EVT_CONTROLLER;

#define MSGID_STDIN 0
#define MSGID_STDOUT 1
#define MSGID_STDERR 2
#define MSGID_PARSED 3
#define MSGID_PIPECREATED 4
#define MSGID_ENDPROCESS 5
#define HISTORY_SIZE 25

// Define a new application type, each program should derive a class from wxApp
class wxCatapultApp : public wxApp
{
	public:
		wxCatapultApp();
		wxString GetResourceDir ();
		bool OnCmdLineHelp(wxCmdLineParser& parser) {return TRUE;};
		virtual ~wxCatapultApp();

		// override base class virtuals
		// ----------------------------

		// this one is called on application startup and is a good place for the app
		// initialization (doing it here and not in the ctor allows to have an error
		// return: if OnInit() returns false, the application terminates)
		virtual bool OnInit();
	private:
		bool LoadXRC (wxString XrcFile);
};

DECLARE_APP(wxCatapultApp)

#endif // wxCatapultApp_H
