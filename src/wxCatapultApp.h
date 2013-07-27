#ifndef WXCATAPULTAPP_H
#define WXCATAPULTAPP_H

#define wxUSE_CMDLINE_PARSER 1
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
SCOPE int EVT_TESTCONFIG;

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
	virtual ~wxCatapultApp();

	wxString GetResourceDir();
	virtual bool OnCmdLineHelp(wxCmdLineParser& parser);
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	void ShowVersion();

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

#endif
