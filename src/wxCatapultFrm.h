#ifndef WXCATAPULTFRAME_H
#define WXCATAPULTFRAME_H

#include <wx/tglbtn.h>
#include <wx/timer.h>
#include <wx/socket.h>
#include <wx/frame.h>

class CatapultXMLParser;
class SessionPage;
class StatusPage;
class InputPage;
class VideoControlPage;
class AudioControlPage;
class MiscControlPage;
class openMSXController;
class wxNotebook;
class wxButton;
class wxNotebookEvent;
class wxStaticBitmap;

#define FOREACH(x,array) for (x=0;x<sizeof(array)/sizeof(array[0]);x++)

// Define a new frame type: this is going to be our main frame
class wxCatapultFrame : public wxFrame
{
public:
	wxCatapultFrame(wxWindow* parent = nullptr);
	virtual ~wxCatapultFrame();

	void SetControlsOnEnd();
	void SetControlsOnLaunch();
	void StartTimers();
	void StopTimers();
	void EnableMainWindow();
	void SetFPSdisplay(wxString val);
	void UpdateLed(wxString ledname, wxString ledstate);
	void UpdateState(wxString statename, wxString state);
	void EnableSaveSettings(bool enabled);
	void CheckConfigs();

	SessionPage* m_sessionPage;
	StatusPage* m_statusPage;
	VideoControlPage* m_videoControlPage;
	MiscControlPage* m_miscControlPage;
	AudioControlPage* m_audioControlPage;
	InputPage* m_inputPage;

	wxNotebook* m_tabControl;
	wxButton* m_launch_AbortButton;

private:
	// event handlers (these functions should _not_ be virtual)
	void OnControllerEvent(wxCommandEvent& event);
	void OnMenuQuit(wxCommandEvent& event);
	void OnMenuCheckConfigs(wxCommandEvent& event);
	void OnMenuAbout(wxCommandEvent& event);
	void OnMenuEditConfig(wxCommandEvent& event);
	void OnMenuLoadSettings(wxCommandEvent& event);
	void OnMenuSaveSettings(wxCommandEvent& event);
	void OnMenuSaveSettingsAs(wxCommandEvent& event);
	void OnMenuSaveOnExit(wxCommandEvent& event);
	void OnMenuDisplayBroken (wxCommandEvent& event);
	void OnMenuClose (wxMenuEvent& event);
	void OnMenuOpen (wxMenuEvent& event);
	void OnMenuHighlight(wxMenuEvent& event);
	void OnLaunch(wxCommandEvent& event);
	void OnUpdateFPS(wxTimerEvent& event);
	void OnEnableMainWindow(wxTimerEvent& event);
	void OnCheckFocus(wxTimerEvent& event);
	void OnChangePage(wxNotebookEvent& event);
	void OnDeselectCatapult(wxActivateEvent& event);
	bool EditConfig(bool fatalIfFailed = false);

	openMSXController* m_controller;

	wxStaticBitmap* m_powerLed;
	wxStaticBitmap* m_capsLed;
	wxStaticBitmap* m_kanaLed;
	wxStaticBitmap* m_pauseLed;
	wxStaticBitmap* m_turboLed;
	wxStaticBitmap* m_fddLed;
	wxMenu* settingsMenu;
	wxMenu* viewMenu;

	wxTimer m_fpsTimer;
	wxTimer m_focusTimer;
	wxTimer m_safetyTimer;
	wxWindow* m_currentFocus;
	wxString m_tempStatus;
	wxString m_settingsfile;

	DECLARE_CLASS(wxCatapultFrame)
	DECLARE_EVENT_TABLE()
};

#endif
