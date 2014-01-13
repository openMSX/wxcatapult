#ifndef WXCATAPULTAPP_H
#define WXCATAPULTAPP_H

#define wxUSE_CMDLINE_PARSER 1

#include <wx/app.h>
#include <wx/string.h>
#include <wx/wxprec.h>

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

private:
	virtual bool OnInit();
	virtual bool OnCmdLineHelp(wxCmdLineParser& parser);
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	void ShowVersion();
	bool LoadXRC(wxString XrcFile);
};

DECLARE_APP(wxCatapultApp)

#endif
