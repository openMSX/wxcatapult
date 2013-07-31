#define SETUP_EXTERNALS
#include "wxCatapultApp.h"
#undef SETUP_EXTERNALS
#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "CatapultConfigDlg.h"
#include "Version.h"
#include "config.h"
#include <cstdio>
#include <wx/image.h>
#include <wx/cmdline.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. wxCatapultApp and
// not wxApp)
IMPLEMENT_APP(wxCatapultApp)

wxCatapultApp::~wxCatapultApp()
{
	// clean up the handlers
	wxXmlResource::Get()->ClearHandlers();
}

// 'Main program' equivalent: the program execution "starts" here
bool wxCatapultApp::OnInit()
{
	if (!wxApp::OnInit()) return false;
	wxImage::AddHandler(new wxPNGHandler());
	wxXmlResource::Get()->InitAllHandlers();

#ifdef ADD_TOGGLEBUTTON_HANDLER
	wxXmlResource::Get()->AddHandler(new wxToggleButtonXmlHandler);
#endif

	EVT_CONTROLLER = wxNewEventType();
	EVT_TESTCONFIG = wxNewEventType();
	bool succes = true;
	succes &= LoadXRC(wxT("about.xrc"));
	succes &= LoadXRC(wxT("config.xrc"));
	succes &= LoadXRC(wxT("fullscreen.xrc"));
	succes &= LoadXRC(wxT("screenshot.xrc"));
	succes &= LoadXRC(wxT("catapult.xrc"));
	succes &= LoadXRC(wxT("session.xrc"));
	succes &= LoadXRC(wxT("status.xrc"));
	succes &= LoadXRC(wxT("misccontrols.xrc"));
	succes &= LoadXRC(wxT("videocontrols.xrc"));
	succes &= LoadXRC(wxT("audiocontrols.xrc"));
	succes &= LoadXRC(wxT("input.xrc"));
	succes &= LoadXRC(wxT("romtype.xrc"));
	succes &= LoadXRC(wxT("ipsselect.xrc"));
	succes &= LoadXRC(wxT("checkconfigs.xrc"));

	if (succes) {
		// We'll set the application and vendorname before the first call to 'Get'
		SetVendorName(wxT("openMSX team"));
		SetAppName(wxT("Catapult"));

		try {
			// create our custom frame with the all the event handlers
			wxCatapultFrame *frame = new wxCatapultFrame();

			// Show the frame.
			frame->Show(true);
		} catch (...) {
			return false;
		}
	} else {
		wxMessageBox(wxT("Unable to load resources!\nThere is something wrong with your installation.\n"),
		             wxT("error"));
		return false;
	}
	return true;
}

static wxString ver    (wxT("V"));
static wxString ver_lng(wxT("version"));
static wxString ver_dsc(wxT("show version information"));

void wxCatapultApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);
	parser.AddSwitch(ver, ver_lng, ver_dsc);
}

bool wxCatapultApp::OnCmdLineHelp(wxCmdLineParser& parser)
{
	parser.Usage();
	puts("\nopenMSX Catapult is the GUI for openMSX ");
	puts("openMSX is the MSX emulator that aims for perfection");
	return false;
}

bool wxCatapultApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	bool res = true;
	if (parser.Found(ver_lng)) {
		if (res) ShowVersion();
		res = false;
	}
	return wxApp::OnCmdLineParsed(parser) && res;
}

void wxCatapultApp::ShowVersion()
{
	for (unsigned i = 0; i < Version::FULL_VERSION.Len(); ++i) {
		putchar(Version::FULL_VERSION.GetChar(i));
	}
	putchar('\n');
}

bool wxCatapultApp::LoadXRC(wxString XrcFile)
{
	wxString resourceDir = GetResourceDir();
	if (::wxFileExists(resourceDir + wxT("/dialogs/") + XrcFile)) {
		wxXmlResource::Get()->Load(resourceDir + wxT("/dialogs/") + XrcFile);
		return true;
	} else {
		return false;
	}
}

wxString wxCatapultApp::GetResourceDir()
{
#ifdef __WXMSW__
	wxString temp = ::wxPathOnly(argv[0]);
	temp.Replace(wxT("/"), wxT("\\"),true);
	while (temp.Last() == '\\') {
			temp = temp.Left(temp.Len() - 1);
		}
		int pos = temp.Find('\\', true);
		if (pos != -1) {
			temp = temp.Left(pos + 1) + wxT("resources");
		}
	return temp;
#else
	return RESOURCEDIR;
#endif
}
