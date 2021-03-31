#include "wxCatapultFrm.h"
#include "wxCatapultApp.h"
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
#include "AboutDlg.h"
#include "CheckConfigsDlg.h"
#include "openMSXController.h"
#include "utils.h"
#include <wx/button.h>
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <sstream>

// TODO: why is this define needed?
#define unisprintf sprintf

class NoOpenMSXBinaryException : public std::exception {};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum {
	// menu items and controls
	Catapult_Quit = 1,
	Catapult_CheckConfigs,
	Catapult_About,
	Catapult_Edit_Config,
	Catapult_Load_OpenMSX_Settings,
	Catapult_Save_OpenMSX_Settings,
	Catapult_Save_OpenMSX_Settings_As,
	Catapult_Save_On_Exit,
	Catapult_Display_Invalids
};

#define FPS_TIMER 1
#define FOCUS_TIMER 2
#define SAFETY_TIMER 3

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
IMPLEMENT_CLASS(wxCatapultFrame, wxFrame)
BEGIN_EVENT_TABLE(wxCatapultFrame, wxFrame)
	EVT_MENU(Catapult_Quit,  wxCatapultFrame::OnMenuQuit)
	EVT_MENU(Catapult_CheckConfigs, wxCatapultFrame::OnMenuCheckConfigs)
	EVT_MENU(Catapult_About, wxCatapultFrame::OnMenuAbout)
	EVT_MENU(Catapult_Edit_Config, wxCatapultFrame::OnMenuEditConfig)
	EVT_MENU(Catapult_Save_OpenMSX_Settings, wxCatapultFrame::OnMenuSaveSettings)
	EVT_MENU(Catapult_Save_OpenMSX_Settings_As, wxCatapultFrame::OnMenuSaveSettingsAs)
	EVT_MENU(Catapult_Load_OpenMSX_Settings, wxCatapultFrame::OnMenuLoadSettings)
	EVT_MENU(Catapult_Save_On_Exit, wxCatapultFrame::OnMenuSaveOnExit)
	EVT_MENU(Catapult_Display_Invalids, wxCatapultFrame::OnMenuDisplayBroken)
	EVT_COMMAND (-1, EVT_CONTROLLER, wxCatapultFrame::OnControllerEvent)
	EVT_BUTTON(XRCID("Launch_AbortButton"), wxCatapultFrame::OnLaunch)
	EVT_TIMER(FPS_TIMER, wxCatapultFrame::OnUpdateFPS)
	EVT_TIMER(FOCUS_TIMER, wxCatapultFrame::OnCheckFocus)
	EVT_TIMER(SAFETY_TIMER, wxCatapultFrame::OnEnableMainWindow)
	EVT_NOTEBOOK_PAGE_CHANGED(XRCID("GlobalTabControl"), wxCatapultFrame::OnChangePage)
	EVT_ACTIVATE (wxCatapultFrame::OnDeselectCatapult)
	EVT_MENU_OPEN(wxCatapultFrame::OnMenuOpen)
	EVT_MENU_CLOSE(wxCatapultFrame::OnMenuClose)
	EVT_MENU_HIGHLIGHT_ALL(wxCatapultFrame::OnMenuHighlight)
END_EVENT_TABLE()

	// include icon only if we're compiling for Windows (but not with
	// Visual Studio)
#if !defined(_MSC_VER) && defined(_WIN32)
#include "catapult.xpm"
#endif


wxCatapultFrame::wxCatapultFrame(wxWindow* parent)
{
	m_fpsTimer.   SetOwner(this, FPS_TIMER);
	m_focusTimer. SetOwner(this, FOCUS_TIMER);
	m_safetyTimer.SetOwner(this, SAFETY_TIMER);

	m_controller.reset(new openMSXController(this));

	wxXmlResource::Get()->LoadFrame(this, parent, wxT("CatapultFrame"));
	// use icon resources for MS Visual Studio,
	// and the PNG for other cases, except
	// for other Windows builds (e.g. MinGW) use XPM
#if defined (_MSC_VER)
	SetIcon(wxIcon(wxT("catapult")));
#elif !defined (_WIN32) // for Windows, a 32x32 MUST be supplied, the docs say
	SetIcon(wxIcon(((wxCatapultApp&)wxGetApp()).GetResourceDir() + wxT("/bitmaps/about.png")));
#else
	SetIcon(catapult_xpm);
#endif

	// create menu bars

	wxMenu* fileMenu = new wxMenu(wxT(""), 0);
	settingsMenu = new wxMenu(wxT(""), 0);
	viewMenu = new wxMenu(wxT(""), 0);
	wxMenu* helpMenu = new wxMenu(wxT(""), 0);

	fileMenu->Append(Catapult_CheckConfigs, wxT("&Test MSX Hardware\tCtrl-T"), wxT("Tests which machines and extensions installed are actually working and removes non-working hardware from the lists"));
	fileMenu->Append(Catapult_Quit, wxT("&Quit\tCtrl-Q"), wxT("Quit openMSX Catapult"));
	settingsMenu->Append(Catapult_Edit_Config, wxT("Edit &Configuration\tCtrl-E"), wxT("Adjust Catapult Configuration"));
	settingsMenu->Append(Catapult_Load_OpenMSX_Settings, wxT("&Load openMSX Settings...\tCtrl-L"), wxT("Load specified settings into openMSX"));
	settingsMenu->Append(Catapult_Save_OpenMSX_Settings, wxT("&Save openMSX Settings\tCtrl-S"), wxT("Save All openMSX settings"));
	settingsMenu->Append(Catapult_Save_OpenMSX_Settings_As, wxT("Save openMSX Settings &As..."), wxT("Save All openMSX settings to a specified file"));
	settingsMenu->AppendCheckItem(Catapult_Save_On_Exit, wxT("Save openMSX Settings On &Exit"), wxT("Save All openMSX settings as soon as openMSX is closed"));
	viewMenu->AppendCheckItem(Catapult_Display_Invalids, wxT("Display Broken Configurations"), wxT("Display all machines and extension even if they don't work"));
	helpMenu->Append(Catapult_About, wxT("About"), wxT("Show about dialog"));

	auto& config = ConfigurationData::instance();

	// now append the freshly created menu to the menu bar...
	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("&File"));
	menuBar->Append(settingsMenu, wxT("&Settings"));
	EnableSaveSettings(false);
	menuBar->Append(viewMenu, wxT("&View"));
	int viewFlags;
	config.GetParameter(ConfigurationData::CD_VIEWFLAGS, &viewFlags);
	viewMenu->Check(Catapult_Display_Invalids, (viewFlags & ConfigurationData::VF_BROKEN) != 0);

	menuBar->Append(helpMenu, wxT("&Help"));

	SetMenuBar(menuBar);

	CreateStatusBar(2);

	// Fill the membervariables with control pointer for easy access

	m_powerLed = (wxStaticBitmap*)FindWindowByName(wxT("PowerLed"));
	m_capsLed  = (wxStaticBitmap*)FindWindowByName(wxT("CapsLockLed"));
	m_kanaLed  = (wxStaticBitmap*)FindWindowByName(wxT("KanaLed"));
	m_pauseLed = (wxStaticBitmap*)FindWindowByName(wxT("PauseLed"));
	m_turboLed = (wxStaticBitmap*)FindWindowByName(wxT("TurboLed"));
	m_fddLed   = (wxStaticBitmap*)FindWindowByName(wxT("FDDLed"));

	m_tabControl = (wxNotebook*)FindWindowByName(wxT("GlobalTabControl"));
	m_sessionPage = new SessionPage(m_tabControl, *m_controller);
	m_statusPage = new StatusPage(m_tabControl);
	m_miscControlPage = new MiscControlPage(m_tabControl, *m_controller);
	m_videoControlPage = new VideoControlPage(m_tabControl, *m_controller);
	m_audioControlPage = new AudioControlPage(m_tabControl, *m_controller);
	m_inputPage = new InputPage(m_tabControl, *m_controller);

	m_tabControl->AddPage(m_sessionPage,      wxT("Session"),        true);
	m_tabControl->AddPage(m_miscControlPage,  wxT("Misc Controls"),  false);
	m_tabControl->AddPage(m_videoControlPage, wxT("Video Controls"), false);
	m_tabControl->AddPage(m_audioControlPage, wxT("Audio Controls"), false);
	m_tabControl->AddPage(m_inputPage,        wxT("Input Text"),     false);
	m_tabControl->AddPage(m_statusPage,       wxT("Status Info"),    false);

	m_launch_AbortButton = (wxButton*)FindWindowByName(wxT("Launch_AbortButton"));

	SetControlsOnEnd();
	m_launch_AbortButton->Enable(false);
	// Now, let's find out if we have a path to openMSX
	bool configOK = true;
	if (!config.HaveRequiredSettings()) {
		configOK = EditConfig(true);
	}
	if (configOK) {
		wxString cmd;
		config.GetParameter(ConfigurationData::CD_EXECPATH, cmd);
		if (!(m_controller->CheckVersion(cmd))) {
			configOK = EditConfig(true);
		}
	}
	if (!configOK) {
		throw NoOpenMSXBinaryException();
	}

	m_sessionPage->GetRomTypes();
	m_miscControlPage->FillInitialJoystickPortValues();
	m_launch_AbortButton->Enable(true);
	m_sessionPage->SetupHardware(true, viewMenu->IsChecked(Catapult_Display_Invalids));
	m_sessionPage->FixLayout();
	wxWindow* tempwindow = FindWindowByName(wxT("MainWindowPanel"));
	wxSize size = tempwindow->GetSizer()->Fit(tempwindow);
	this->GetSizer()->SetMinSize(size);
	this->GetSizer()->Fit(this);
	this->GetSizer()->SetSizeHints(this);
	m_sessionPage->RestoreHistory();
	m_sessionPage->SetupHardware(false, viewMenu->IsChecked(Catapult_Display_Invalids));
	SetStatusText(wxT("Ready"));
}

// event handlers
void wxCatapultFrame::OnMenuQuit(wxCommandEvent& event)
{
	// TRUE is to force the frame to close
	Close(TRUE);
}

void wxCatapultFrame::OnMenuCheckConfigs(wxCommandEvent& event)
{
	CheckConfigs();
}

void wxCatapultFrame::CheckConfigs()
{
	m_sessionPage->SetupHardware(false, true);
	wxString cmd;
	auto& config = ConfigurationData::instance();
	config.GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	wxArrayString machines   = m_sessionPage->GetDetectedMachines();
	wxArrayString extensions = m_sessionPage->GetDetectedExtensions();
	CheckConfigsDlg dlg(this);
	dlg.CenterOnParent();
	if (dlg.Present(cmd, machines, extensions) == wxID_OK) {
		wxString machineString;
		for (unsigned j = 0; j < machines.GetCount(); ++j) {
			machineString += machines[j];
			machineString += wxT("::");

		}

		wxString extensionString;
		for (unsigned j = 0; j < extensions.GetCount(); ++j) {
			extensionString += extensions[j];
			extensionString += wxT("::");
		}

		config.SetParameter(ConfigurationData::CD_MACHINES,   machineString);
		config.SetParameter(ConfigurationData::CD_EXTENSIONS, extensionString);
		if (!config.SaveData()) {
			wxMessageBox(wxT("Error saving configuration data"));
		}
	}
	m_sessionPage->SetupHardware(false, viewMenu->IsChecked(Catapult_Display_Invalids));
}

void wxCatapultFrame::OnMenuAbout(wxCommandEvent& event)
{
	// called when help - about is picked from the menu or toolbar
	AboutDlg dlg(this);
	auto* version     = (wxStaticText*)FindWindowByName(wxT("AboutProductNameLabel"));
	auto* description = (wxStaticText*)FindWindowByName(wxT("AboutProductDescriptionLabel"));
	auto* message     = (wxStaticText*)FindWindowByName(wxT("AboutMessageLabel"));
	version->SetLabel(wxString(Version::FULL_VERSION.c_str(), wxConvUTF8));
	description->SetLabel(wxT("The official GUI for openMSX"));
	message->SetLabel(wxT("\251 2003-2021 The openMSX Team\nhttp://openmsx.org/\n"));
	dlg.Fit();
	dlg.CenterOnParent();
	dlg.ShowModal();
}

void wxCatapultFrame::OnMenuEditConfig(wxCommandEvent& event)
{
	EditConfig(false);
}

bool wxCatapultFrame::EditConfig(bool fatalIfFails)
{
	bool retval = true;
	CatapultConfigDlg dlg(this, *m_controller);
	dlg.Center();
	if (dlg.ShowModal() == wxID_OK) {
		if (!ConfigurationData::instance().SaveData()) {
			wxMessageBox(wxT("Error saving configuration data!"));
		}
		m_sessionPage->SetupHardware(false, false);
	} else {
		retval = !fatalIfFails;
	}
	return retval;
}

void wxCatapultFrame::OnMenuLoadSettings(wxCommandEvent& event)
{
	wxString path = wxT("Configuration Files(*.xml)|*.xml;*.XML|All files|*.*||");
	wxFileDialog filedlg(this, wxT("Select configuration file"), wxT(""), wxT(""), path, wxFD_OPEN);
	if (filedlg.ShowModal() == wxID_OK) {
		wxString settingsfile = filedlg.GetPath();
		if (m_controller->IsOpenMSXRunning()){
			m_controller->WriteCommand(wxT("load_settings ") + utils::ConvertPathForCommand(settingsfile));
		} else {
			m_settingsfile = settingsfile;
		}
	}
}

void wxCatapultFrame::OnMenuSaveSettings(wxCommandEvent& event)
{
	m_controller->WriteCommand(wxT("save_settings"),
		[&](const wxString&, const wxString&) {
			wxMessageBox(wxT("openMSX settings saved successfully!"));
		},
		[&](const wxString&, const wxString& r) {
			wxMessageBox(wxT("Error saving openMSX settings\n") + r);
		});
}

void wxCatapultFrame::OnMenuSaveSettingsAs(wxCommandEvent& event)
{
	wxString path = wxT("Configuration Files(*.xml)|*.xml;*.XML|All files|*.*||");
	wxFileDialog filedlg(this, wxT("Select file to save to"), wxT(""), wxT(""), path, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK) {
		wxString settingsfile = filedlg.GetPath();
		m_controller->WriteCommand(wxT("save_settings ") + utils::ConvertPathForCommand(settingsfile));
	}
}

void wxCatapultFrame::OnMenuSaveOnExit(wxCommandEvent& event)
{
	if (GetMenuBar()->IsChecked(Catapult_Save_On_Exit)) {
		m_controller->WriteCommand(wxT("set save_settings_on_exit true"));
	} else {
		m_controller->WriteCommand(wxT("set save_settings_on_exit false"));
	}
}

void wxCatapultFrame::OnMenuDisplayBroken(wxCommandEvent& event)
{
	auto& config = ConfigurationData::instance();
	int viewFlags;
	config.GetParameter(ConfigurationData::CD_VIEWFLAGS, &viewFlags);
	m_sessionPage->SetupHardware(false, viewMenu->IsChecked(Catapult_Display_Invalids));
	if (viewMenu->IsChecked(Catapult_Display_Invalids)) {
		config.SetParameter(ConfigurationData::CD_VIEWFLAGS, (long)(viewFlags |  ConfigurationData::VF_BROKEN));
	} else {
		config.SetParameter(ConfigurationData::CD_VIEWFLAGS, (long)(viewFlags & ~ConfigurationData::VF_BROKEN));
	}
	config.SaveData();
}

void wxCatapultFrame::EnableSaveSettings(bool enabled)
{
	settingsMenu->Enable(Catapult_Save_OpenMSX_Settings,    enabled);
	settingsMenu->Enable(Catapult_Save_OpenMSX_Settings_As, enabled);
}

void wxCatapultFrame::OnMenuOpen(wxMenuEvent& event)
{
	m_tempStatus = GetStatusBar()->GetStatusText(0);
}

void wxCatapultFrame::OnMenuClose(wxMenuEvent& event)
{
	SetStatusText(m_tempStatus, 0);
}

void wxCatapultFrame::OnMenuHighlight(wxMenuEvent& event)
{
	if (event.GetMenuId() != -1) {
		wxFrame::OnMenuHighlight(event);
	} else {
		SetStatusText(m_tempStatus, 0);
	}
}

void wxCatapultFrame::OnLaunch(wxCommandEvent& event)
{
	if (m_launch_AbortButton->GetLabel().IsSameAs(wxT("Stop"))) {
		m_controller->WriteCommand(wxT("exit"));
		return;
	}
	// check again if openMSX can be found
	auto& config = ConfigurationData::instance();
	if (!config.HaveRequiredSettings()) {
		if (!EditConfig(true)) return;
	}

	m_launch_AbortButton->SetLabel(wxT("Stop"));
	m_launch_AbortButton->Enable(false);
	m_safetyTimer.Start(2000, true); // max 2 seconds disable (whatever happens)

	wxString cmd;
	config.GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	if (!m_settingsfile.IsEmpty()) {
		cmd += wxT(" -setting ") + utils::ConvertPathForCommand(m_settingsfile);
	}
	cmd += wxT(" ") + m_sessionPage->getStartupCommandLineOptions();

	m_statusPage->Clear();

	Enable(false); // Disable this frame only after getting the selections

	SetControlsOnLaunch();
	m_controller->StartOpenMSX(cmd);
}

void wxCatapultFrame::OpenMSXStarted()
{
	StartTimers();
	m_launch_AbortButton->Enable(true);
	EnableSaveSettings(true);
}

void wxCatapultFrame::OpenMSXStopped()
{
	wxString leds[] = {
		wxT("power"), wxT("caps"),  wxT("kana"),
		wxT("pause"), wxT("turbo"), wxT("FDD")
	};
	for (auto& l : leds) {
		UpdateLed(l, wxT("off"));
	}

	StopTimers();
	EnableSaveSettings(false);
	m_audioControlPage->DestroyAudioMixer();
	m_audioControlPage->DisableAudioPanel();
	m_launch_AbortButton->Enable(true);
	SetControlsOnEnd();
	m_launch_AbortButton->SetLabel(wxT("Start"));
}

void wxCatapultFrame::SetControlsOnLaunch()
{
	SetStatusText(wxT("Running"));
	if (m_miscControlPage)  m_miscControlPage ->SetControlsOnLaunch();
	if (m_videoControlPage) m_videoControlPage->SetControlsOnLaunch();
	if (m_sessionPage)      m_sessionPage     ->SetControlsOnLaunch();
	if (m_inputPage)        m_inputPage       ->SetControlsOnLaunch();
}

void wxCatapultFrame::SetControlsOnEnd()
{
	if (m_miscControlPage)  m_miscControlPage ->SetControlsOnEnd();
	if (m_videoControlPage) m_videoControlPage->SetControlsOnEnd();
	if (m_sessionPage)      m_sessionPage     ->SetControlsOnEnd();
	if (m_inputPage)        m_inputPage       ->SetControlsOnEnd();
	SetStatusText(wxT("Ready"));
}

void wxCatapultFrame::OnControllerEvent(wxCommandEvent& event)
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
	SetStatusText(wxT(""), 1);
}

void wxCatapultFrame::OnUpdateFPS(wxTimerEvent& event)
{
	m_controller->WriteCommand(wxT("openmsx_info fps"),
		[&](const wxString& c, const wxString& r) {
			// convert wxString -> double using C locale
			//  (so use '.' as decimal point)
			// The wxString::ToDouble() method uses the current
			// locale and for some reason wxWidgets changes the
			// default locale (or doesn't use the C locale like
			// most c/c++ programs).
			double val;
			std::stringstream ss(std::string(r.ToAscii()));
			ss >> val;
			//if (r.ToCDouble(&val)) { // requires wxWidgets-2.9
				SetStatusText(wxString::Format(wxT("%2.1f fps"), val), 1);
			//}
		});
}

void wxCatapultFrame::OnEnableMainWindow(wxTimerEvent& event)
{
	EnableMainWindow();
}

void wxCatapultFrame::EnableMainWindow()
{
	Enable(true);
	m_safetyTimer.Stop();
}

void wxCatapultFrame::OnCheckFocus(wxTimerEvent& event)
{
	int selectedPage = m_tabControl->GetSelection();
	if (selectedPage != -1) {
		auto* page = (CatapultPage*)m_tabControl->GetPage(selectedPage);
		wxWindow* newfocus = page->FindFocus();
		if (m_currentFocus != newfocus) {
			page->HandleFocusChange(m_currentFocus, newfocus);
			m_currentFocus = newfocus;
		}
	}
}

void wxCatapultFrame::OnChangePage(wxNotebookEvent& event)
{
	int oldPageNr = event.GetOldSelection();
	int newPageNr = event.GetSelection();
	if (oldPageNr != -1) {
		auto* page = (CatapultPage*)m_tabControl->GetPage(oldPageNr);
		page->HandleFocusChange(m_currentFocus, nullptr);
	}
	if (newPageNr != -1) {
		auto* page = (CatapultPage*)m_tabControl->GetPage(newPageNr);
		wxWindow* newfocus = page->FindFocus();
		page->HandleFocusChange(nullptr, newfocus);
		m_currentFocus = newfocus;
	}
}

void wxCatapultFrame::OnDeselectCatapult(wxActivateEvent& event)
{
	int selectedPage = m_tabControl->GetSelection();
	if (selectedPage != -1) {
		auto* page = (CatapultPage*)m_tabControl->GetPage(selectedPage);
		page->HandleFocusChange(m_currentFocus, nullptr);
	}
	m_currentFocus = nullptr;
}

void wxCatapultFrame::UpdateLed(const wxString& ledname, const wxString& ledstate)
{
	wxStaticBitmap* led;
	if      (ledname == wxT("power")) led = m_powerLed;
	else if (ledname == wxT("caps"))  led = m_capsLed;
	else if (ledname == wxT("kana"))  led = m_kanaLed;
	else if (ledname == wxT("pause")) led = m_pauseLed;
	else if (ledname == wxT("turbo")) led = m_turboLed;
	else if (ledname == wxT("FDD"))   led = m_fddLed;
	else { assert(false); led = nullptr; }

	wxString image = (ledstate == wxT("on"))
	               ? wxT("/bitmaps/ledon.png")
	               : wxT("/bitmaps/ledoff.png");
	wxString resourceDir = ((wxCatapultApp&)wxGetApp()).GetResourceDir();
	led->SetBitmap(wxBitmap(resourceDir + image, wxBITMAP_TYPE_PNG));
}

void wxCatapultFrame::UpdateState(const wxString& statename, const wxString& state)
{
	if (statename != wxT("paused")) {
		// just one possible type atm, so ignore all else
		return;
	}
	wxString statustext = (state == wxT("true"))
	                    ? wxT("Paused")
	                    : wxT("Running");
	SetStatusText(statustext, 0);
}
