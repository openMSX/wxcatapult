// $Id: wxCatapultApp.cpp,v 1.27 2005/10/14 08:53:04 h_oudejans Exp $
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#define SETUP_EXTERNALS
#include "wxCatapultApp.h"
#undef SETUP_EXTERNALS

#include "wx/xrc/xmlres.h"

#include "wxCatapultFrm.h"

#include "wx/image.h"
#include "wx/cmdline.h"
#include "ConfigurationData.h"
#include "CatapultConfigDlg.h"

#if !wxCHECK_VERSION(2,5,0)
#include "wxToggleButtonXmlHandler.h"
#define ADD_TOGGLEBUTTON_HANDLER 1
#endif

#ifdef __UNIX__
#include "config.h"
#endif

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. wxCatapultApp and
// not wxApp)
IMPLEMENT_APP(wxCatapultApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

wxCatapultApp::wxCatapultApp()
{
}

wxCatapultApp::~wxCatapultApp()
{
	// clean up the handlers
	wxXmlResource::Get()->ClearHandlers();
}

// 'Main program' equivalent: the program execution "starts" here
bool wxCatapultApp::OnInit()
{
	wxApp::OnInit();
	wxImage::AddHandler(new wxPNGHandler());
	wxXmlResource::Get()->InitAllHandlers();

#ifdef ADD_TOGGLEBUTTON_HANDLER
	wxXmlResource::Get()->AddHandler(new wxToggleButtonXmlHandler);
#endif

	EVT_CONTROLLER = wxNewEventType();
	EVT_TESTCONFIG = wxNewEventType();
	bool succes = true;
	succes &= LoadXRC (wxT("about.xrc"));
	succes &= LoadXRC (wxT("config.xrc"));
	succes &= LoadXRC (wxT("fullscreen.xrc"));
	succes &= LoadXRC (wxT("screenshot.xrc"));
	succes &= LoadXRC (wxT("catapult.xrc"));
	succes &= LoadXRC (wxT("session.xrc"));
	succes &= LoadXRC (wxT("status.xrc"));
	succes &= LoadXRC (wxT("misccontrols.xrc"));
	succes &= LoadXRC (wxT("videocontrols.xrc"));
	succes &= LoadXRC (wxT("audiocontrols.xrc"));
	succes &= LoadXRC (wxT("input.xrc"));
	succes &= LoadXRC (wxT("romtype.xrc"));
	succes &= LoadXRC (wxT("ipsselect.xrc"));
	succes &= LoadXRC (wxT("checkconfigs.xrc"));

	if (succes)
	{
		// We'll set the application and vendorname before the first call to 'Get'
		SetVendorName(wxT("openMSX team"));
		SetAppName(wxT("Catapult"));

		// create our custom frame with the all the event handlers
		wxCatapultFrame *frame = new wxCatapultFrame();

		// Show the frame.
		frame->Show(TRUE);
	}
	else {
		wxMessageBox (wxT("Unable to load resources!\nThere is something wrong with your installation.\n"),wxT("error"));
		return FALSE;
	}
	return TRUE;
}

bool wxCatapultApp::LoadXRC(wxString XrcFile)
{
	wxString resourceDir = GetResourceDir();
	if (::wxFileExists(resourceDir + wxT("/dialogs/") + XrcFile)) {
		wxXmlResource::Get()->Load(resourceDir + wxT("/dialogs/") + XrcFile);
		return true;
	}
	else{
		return false;
	}
}

wxString wxCatapultApp::GetResourceDir ()
{
#ifdef __WXMSW__
	wxString temp = ::wxPathOnly(argv[0]);
	temp.Replace(wxT("/"),wxT("\\"),true);
	while (temp.Last() == '\\') {
			temp = temp.Left(temp.Len()-1);
		}
		int pos = temp.Find('\\',true);
		if (pos != -1) {
			temp = temp.Left(pos+1) + wxT("resources");
		}
	return wxString(temp);
#else
	return RESOURCEDIR;
#endif
}
