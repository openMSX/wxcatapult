// $Id: wxCatapultFrm.cpp,v 1.41 2004/10/07 18:22:03 manuelbi Exp $ 
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/image.h>
#include "wxCatapultApp.h"
#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "CatapultConfigDlg.h"
#include "PipeReadThread.h"
#include "CatapultXMLParser.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "AudioControlPage.h"
#include "MiscControlPage.h"
#include "InputPage.h"
#include "Version.h"
#ifdef __WINDOWS__
#include "openMSXWindowsController.h"
#else
#include "openMSXLinuxController.h"
#endif
#include "openMSXController.h"

#define unisprintf sprintf

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// menu items
	Catapult_Quit = 1,
	Catapult_About,
	Catapult_Edit_Config,
	Catapult_Load_OpenMSX_Settings,
	Catapult_Save_OpenMSX_Settings,
	Catapult_Save_OpenMSX_Settings_As
	
};

#define FPS_TIMER 1
#define FOCUS_TIMER 2

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
IMPLEMENT_CLASS(wxCatapultFrame, wxFrame)
BEGIN_EVENT_TABLE(wxCatapultFrame, wxFrame)
	EVT_MENU(Catapult_Quit,  wxCatapultFrame::OnMenuQuit)
	EVT_MENU(Catapult_About, wxCatapultFrame::OnMenuAbout)
	EVT_MENU(Catapult_Edit_Config, wxCatapultFrame::OnMenuEditConfig)
	EVT_MENU(Catapult_Save_OpenMSX_Settings, wxCatapultFrame::OnMenuSaveSettings)
	EVT_MENU(Catapult_Save_OpenMSX_Settings_As,wxCatapultFrame::OnMenuSaveSettingsAs)
	EVT_MENU(Catapult_Load_OpenMSX_Settings,wxCatapultFrame::OnMenuLoadSettings)
	EVT_COMMAND (-1, EVT_CONTROLLER, wxCatapultFrame::OnControllerEvent)
	EVT_BUTTON(XRCID("Launch_AbortButton"),wxCatapultFrame::OnLaunch)
	EVT_TIMER(FPS_TIMER, wxCatapultFrame::OnUpdateFPS)
	EVT_TIMER(FOCUS_TIMER, wxCatapultFrame::OnCheckFocus)
	EVT_NOTEBOOK_PAGE_CHANGED(XRCID("GlobalTabControl"), wxCatapultFrame::OnChangePage)
	EVT_ACTIVATE (wxCatapultFrame::OnDeselectCatapult)
	EVT_MENU_OPEN(wxCatapultFrame::OnMenuOpen)
	EVT_MENU_CLOSE(wxCatapultFrame::OnMenuClose)
	EVT_MENU_HIGHLIGHT_ALL(wxCatapultFrame::OnMenuHighlight)
END_EVENT_TABLE()

	// include icon for any non-unix version
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "catapult.xpm"
#endif

	// ------------------------------------------------------------------
	// main frame
	// ------------------------------------------------------------------

	// frame constructor
	wxCatapultFrame::wxCatapultFrame(wxWindow * parent)
{
	m_fpsTimer.SetOwner(this,FPS_TIMER);
	m_focusTimer.SetOwner(this,FOCUS_TIMER);

#ifdef __WINDOWS__	
	m_controller = new openMSXWindowsController(this);
#else
	m_controller = new openMSXLinuxController(this);
#endif

	wxXmlResource::Get()->LoadFrame(this, parent, wxT("CatapultFrame"));
	SetIcon (wxICON(catapult));

	// create menu bars

	wxMenu *fileMenu = new wxMenu("", wxMENU_TEAROFF);
	settingsMenu = new wxMenu("", wxMENU_TEAROFF);
	wxMenu *helpMenu = new wxMenu("", wxMENU_TEAROFF);

	fileMenu->Append(Catapult_Quit, wxT("&Quit\tCtrl-Q"), _("Quit this program"));
	settingsMenu->Append(Catapult_Edit_Config, wxT("&Edit Configuration\tCtrl-E"), _("Adjust Catapult Configuration"));
	settingsMenu->Append(Catapult_Load_OpenMSX_Settings, wxT("&Load openMSX Settings..."), _("Load specified settings into openMSX"));
	settingsMenu->Append(Catapult_Save_OpenMSX_Settings, wxT("&Save openMSX Settings"), _("Save All openMSX settings"));
	settingsMenu->Append(Catapult_Save_OpenMSX_Settings_As, wxT("Save openMSX Settings As..."), _("Save All openMSX settings to a specified file"));
	helpMenu->Append(Catapult_About, wxT("&About Catapult...\tCtrl-A"), _("Show about dialog"));

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(settingsMenu, wxT("&Settings"));
	EnableSaveSettings(false);
	menuBar->Append(helpMenu, wxT("&Help"));

	SetMenuBar(menuBar);

	CreateStatusBar(2);

	// Fill the membervariables with control pointer for easy access

	m_powerLed = (wxStaticBitmap *)FindWindow(wxT("PowerLed"));
	m_capsLed = (wxStaticBitmap *)FindWindow(wxT("CapsLockLed"));
	m_kanaLed = (wxStaticBitmap *)FindWindow(wxT("KanaLed"));
	m_pauseLed = (wxStaticBitmap *)FindWindow(wxT("PauseLed"));
	m_turboLed = (wxStaticBitmap *)FindWindow(wxT("TurboLed"));
	m_fddLed = (wxStaticBitmap *)FindWindow(wxT("FDDLed"));

	m_tabControl = (wxNotebook *)FindWindow(wxT("GlobalTabControl"));
	m_sessionPage = new SessionPage(m_tabControl,m_controller);
	m_statusPage = new StatusPage(m_tabControl);
	m_miscControlPage = new MiscControlPage(m_tabControl,m_controller);
	m_videoControlPage = new VideoControlPage(m_tabControl,m_controller);
	m_audioControlPage = new AudioControlPage(m_tabControl,m_controller);
	m_inputPage = new InputPage(m_tabControl,m_controller);
	
	m_tabControl->AddPage(m_sessionPage,wxT("Session"),true);
	m_tabControl->AddPage(m_miscControlPage,wxT("Misc Controls"),false);
	m_tabControl->AddPage(m_videoControlPage,wxT("Video Controls"),false);
	m_tabControl->AddPage(m_audioControlPage,wxT("Audio Controls"),false);
	m_tabControl->AddPage(m_inputPage,wxT("Input Text"),false);
	m_tabControl->AddPage(m_statusPage,wxT("Status Info"),false);
	
	wxWindow * tempwindow = FindWindow(wxT("MainWindowPanel"));
	wxSize size = tempwindow->GetSizer()->Fit(tempwindow);
	this->GetSizer()->SetMinSize(size);
	this->GetSizer()->Fit(this);
	this->GetSizer()->SetSizeHints(this);

	m_launch_AbortButton = (wxButton *)FindWindow(wxT("Launch_AbortButton"));

	SetControlsOnEnd();
	m_launch_AbortButton->Enable(false);
	wxString cmd;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	m_controller->StartOpenMSX(cmd,true);
	m_settingsfile = "";
}

// frame destructor
wxCatapultFrame::~wxCatapultFrame()
{
	delete m_controller;
}

// event handlers
void wxCatapultFrame::OnMenuQuit(wxCommandEvent& event)
{
	// TRUE is to force the frame to close
	Close(TRUE);
}

void wxCatapultFrame::OnMenuAbout(wxCommandEvent& event)
{
	// called when help - about is picked from the menu or toolbar
	wxString msg;
	msg.Printf(
		Version::FULL_VERSION + "\n\n" +
		_("Created by the openMSX team\n")
		);

	wxMessageBox(msg, _("About openMSX Catapult"),
		wxOK | wxICON_INFORMATION, this );
}

void wxCatapultFrame::OnMenuEditConfig(wxCommandEvent& event)
{
	wxString cmd;
	CatapultConfigDlg dlg;
	dlg.Center();
	if (dlg.ShowModal() == wxID_OK) {
		bool result = ConfigurationData::instance()->SaveData();
#if !OPENMSX_DEMO_CD_VERSION
		if (!result) {
			wxMessageBox ("Error saving configuration data!");
		}
#endif
		m_sessionPage->SetupHardware(false);
		ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);
		
		m_controller->StartOpenMSX(cmd,true);
	}
}

void wxCatapultFrame::OnMenuLoadSettings(wxCommandEvent &event)
{
	wxString settingsfile;
	wxString path;
#ifndef __MOTIF__
	path = wxT("Configuration Files(*.xml)|*.xml;*.XML|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,_("Select configuration file"), "", wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		settingsfile = filedlg.GetPath();
		if (m_controller->IsOpenMSXRunning()){
			m_controller->WriteCommand(wxString("load_settings ") + m_sessionPage->ConvertPath(settingsfile,true));
		}		
		else{
			m_settingsfile = settingsfile;
		}
	}
}

void wxCatapultFrame::OnMenuSaveSettings(wxCommandEvent & event)
{
	m_controller->WriteCommand("save_settings");
}

void wxCatapultFrame::OnMenuSaveSettingsAs (wxCommandEvent & event)
{
	wxString settingsfile;
	wxString path;
#ifndef __MOTIF__
	path = wxT("Configuration Files(*.xml)|*.xml;*.XML|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,_("Select file to save to"), "", wxT(""), path ,wxSAVE | wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK)
	{
		settingsfile = filedlg.GetPath();
		m_controller->WriteCommand(wxString("save_settings ") + m_sessionPage->ConvertPath(settingsfile,true));
	}	
}

void wxCatapultFrame::EnableSaveSettings(bool enabled)
{
	settingsMenu->Enable(Catapult_Save_OpenMSX_Settings,enabled);
	settingsMenu->Enable(Catapult_Save_OpenMSX_Settings_As,enabled);
}

void wxCatapultFrame::OnMenuOpen(wxMenuEvent &event)
{
	m_tempStatus = GetStatusBar()->GetStatusText(0);	
}

void wxCatapultFrame::OnMenuClose(wxMenuEvent &event)
{
	SetStatusText(m_tempStatus,0);	
}

void wxCatapultFrame::OnMenuHighlight(wxMenuEvent &event)
{
	if (event.GetMenuId() != -1){
		wxFrame::OnMenuHighlight(event);
	}
	else{
		SetStatusText(m_tempStatus,0);	
	}
}

void wxCatapultFrame::OnLaunch(wxCommandEvent& event)
{
	unsigned int i;
	if (m_launch_AbortButton->GetLabel().IsSameAs(wxT("Stop"))) {
		m_controller->WriteCommand("quit");
		return;
	}
	m_launch_AbortButton->SetLabel(wxT("Stop"));
	m_launch_AbortButton->Enable(false);
	wxArrayString hardware;
	wxArrayString media;
	m_sessionPage->getHardware(hardware);
	m_sessionPage->getMedia(media);
	wxString cmd;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	if (hardware[0] != wxT(" <default> "))
	{
		cmd += " -machine " + hardware[0];
	}
	if (m_settingsfile != ""){
		cmd += " -setting " + m_sessionPage->ConvertPath(m_settingsfile,true);
	}
	if (hardware.GetCount() > 1) {
		for (i=1;i<hardware.GetCount();i++) {
			cmd += " -ext " + hardware[i];
		}
	}

	wxString parmname [6]={wxT("diska"),wxT("diskb"),wxT("carta"),wxT("cartb"),wxT("cassetteplayer"),wxT("cas")};
	for (i=0;i<6;i++) {
		if (!media[i].IsEmpty()) {
			cmd += " -" + parmname[i] + " \"" + media[i] + wxT("\"");
		}
	}

	m_sessionPage->UpdateSessionData();
	m_statusPage->m_outputtext->Clear();
	m_controller->StartOpenMSX(cmd);
	SetControlsOnLaunch();
}

void wxCatapultFrame::SetControlsOnLaunch()
{
	m_miscControlPage->SetControlsOnLaunch();
	m_videoControlPage->SetControlsOnLaunch();
	m_sessionPage->SetControlsOnLaunch();
	m_inputPage->SetControlsOnLaunch();
}

void wxCatapultFrame::SetControlsOnEnd()
{
	m_miscControlPage->SetControlsOnEnd();
	m_videoControlPage->SetControlsOnEnd();
	m_sessionPage->SetControlsOnEnd();
	m_inputPage->SetControlsOnEnd();
}

void wxCatapultFrame::OnControllerEvent(wxCommandEvent &event)
{
	m_controller->HandleMessage(event);	
}

void wxCatapultFrame::StartTimers()
{
	m_fpsTimer.Start(1000);
	m_focusTimer.Start(250);
}

void wxCatapultFrame::StopTimers()
{
	m_fpsTimer.Stop();
	m_focusTimer.Stop();
	SetStatusText(wxT(""),1);
}

void wxCatapultFrame::SetFPSdisplay(wxString val)
{
	double valfl = strtod(val.c_str(),NULL);
	val.sprintf("%2.1f",valfl);	
	SetStatusText(val + " fps",1);
}

void wxCatapultFrame::OnUpdateFPS(wxTimerEvent& event)
{
	m_controller->WriteCommand(m_controller->GetInfoCommand("fps"));
}

void wxCatapultFrame::OnCheckFocus(wxTimerEvent& event)
{
	int selectedPage = m_tabControl->GetSelection();
	if (selectedPage != -1) {
		CatapultPage * page = (CatapultPage *)m_tabControl->GetPage(selectedPage);
		wxWindow * newfocus = page->FindFocus();
		if (m_currentFocus != newfocus) {
			page->HandleFocusChange(m_currentFocus,newfocus);
			m_currentFocus = newfocus;
		}
	}
}

void wxCatapultFrame::OnChangePage(wxNotebookEvent &event)
{
	CatapultPage * page;
	int oldPageNr = event.GetOldSelection();
	int newPageNr = event.GetSelection();
	if (oldPageNr != -1) {
		page = (CatapultPage *)m_tabControl->GetPage(oldPageNr);
		page->HandleFocusChange(m_currentFocus,NULL);	
	}
	if (newPageNr != -1) {
		page = (CatapultPage *)m_tabControl->GetPage(newPageNr);	
		wxWindow * newfocus = page->FindFocus();
		page->HandleFocusChange(NULL,newfocus);
		m_currentFocus = newfocus;
	}
}

void wxCatapultFrame::OnDeselectCatapult(wxActivateEvent & event)
{
	int selectedPage = m_tabControl->GetSelection();
	if (selectedPage != -1) {
		CatapultPage * page = (CatapultPage *)m_tabControl->GetPage(selectedPage);	
		page->HandleFocusChange(m_currentFocus,NULL);
	}
	m_currentFocus = NULL;
}

void wxCatapultFrame::UpdateLed(wxString ledname, wxString ledstate)
{
	wxString resourceDir = ((wxCatapultApp &)wxGetApp()).GetResourceDir();
	wxStaticBitmap * led = NULL;
	if (ledname == "power") led = m_powerLed;
	if (ledname == "caps") led = m_capsLed;
	if (ledname == "kana") led = m_kanaLed;
	if (ledname == "pause") led = m_pauseLed;
	if (ledname == "turbo") led = m_turboLed;
	if (ledname == "fdd") led = m_fddLed;

	if (ledstate == "off") led->SetBitmap(wxBitmap(resourceDir + wxT("/bitmaps/ledoff.bmp"),wxBITMAP_TYPE_BMP));
	if (ledstate == "on") led->SetBitmap(wxBitmap(resourceDir + wxT("/bitmaps/ledon.bmp"),wxBITMAP_TYPE_BMP));
}

void wxCatapultFrame::UpdateState (wxString statename, wxString state)
{
	wxString statustext;
	if (statename != "paused"){  // just one possible type atm, so ignore all else
		return;
	}
	if (state == "true"){
		statustext = "Paused";
	}
	else{
		statustext = "Running";
	}
	SetStatusText(statustext,0);
}
