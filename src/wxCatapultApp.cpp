// $Id: wxCatapultApp.cpp,v 1.10 2004/04/25 15:27:50 h_oudejans Exp $ 
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
#include "wxToggleButtonXmlHandler.h"

#include "config.h"

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
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->AddHandler(new wxToggleButtonXmlHandler);

	EVT_CONTROLLER = wxNewEventType();

	bool succes = true;
	succes &= LoadXRC (wxT("config.xrc"));
	succes &= LoadXRC (wxT("fullscreen.xrc"));
	succes &= LoadXRC (wxT("catapult.xrc"));
	succes &= LoadXRC (wxT("session.xrc"));
	succes &= LoadXRC (wxT("status.xrc"));
	succes &= LoadXRC (wxT("misccontrols.xrc"));
	succes &= LoadXRC (wxT("videocontrols.xrc"));
	succes &= LoadXRC (wxT("audiocontrols.xrc"));

	if (succes)
	{	
		// We'll set the appication and vendorname before the first call to 'Get'

		SetVendorName(wxT("openMSX team"));
		SetAppName(wxT("Catapult"));

		// Now, let's find out if we have a path to openMSX
		ConfigurationData * config = ConfigurationData::instance();
		if (!config->HaveRequiredSettings())
		{
			CatapultConfigDlg dlg;
			dlg.Center();
			if (dlg.ShowModal() != wxID_OK)
			{	
				return FALSE;
			}
		}

		// create our custom frame with the all the event handlers
		wxCatapultFrame *frame = new wxCatapultFrame();

		// Show the frame.
		frame->Show(TRUE);
	}
	else{
		wxMessageBox ("Unable to load resources\n","Error");
		return FALSE;
	}
	return TRUE;
}

bool wxCatapultApp::LoadXRC(wxString XrcFile)
{
	wxString resourceDir = GetResourceDir();
	if (::wxFileExists(resourceDir + wxT("/dialogs/") + XrcFile)){
		wxXmlResource::Get()->Load(resourceDir + wxT("/dialogs/") + XrcFile);
		return true;
	}
	else{
		return false;
	}
}

wxString wxCatapultApp::GetResourceDir ()
{
#ifdef RESOURCEDIR
	return wxString(RESOURCEDIR);
#else
	wxString temp = ::wxPathOnly(argv[0]);
	temp.Replace ("/","\\",true);
	while (temp.Last() == '\\'){
			temp = temp.Left(temp.Len()-1);
		}
		int pos = temp.Find('\\',true);
		if (pos !=-1){
			temp = temp.Left(pos+1) + wxT("resources");
		}
	return wxString(temp);	
#endif
}

