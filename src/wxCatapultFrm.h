// $Id: wxCatapultFrm.h,v 1.12 2004/08/26 14:29:46 h_oudejans Exp $ 
#ifndef wxCatapultFrame_H
#define wxCatapultFrame_H

#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/notebook.h>
#include <wx/timer.h>

class CatapultXMLParser;
class SessionPage;
class StatusPage;
class VideoControlPage;
class AudioControlPage;
class MiscControlPage;
class openMSXController;

// Define a new frame type: this is going to be our main frame
class wxCatapultFrame : public wxFrame
{
	public:
		void OnControllerEvent (wxCommandEvent & event);
		// ctor(s)
		wxCatapultFrame(wxWindow * parent=(wxWindow *)NULL);
		virtual ~wxCatapultFrame();

		// event handlers (these functions should _not_ be virtual)
		void OnMenuQuit(wxCommandEvent& event);
		void OnMenuAbout(wxCommandEvent& event);
		void OnMenuEditConfig(wxCommandEvent& event);
		void OnMenuClose (wxMenuEvent &event);
		void OnMenuOpen (wxMenuEvent &event);
		void OnLaunch(wxCommandEvent& event);
		void SetControlsOnEnd();
		void SetControlsOnLaunch();
		void StartTimers();
		void StopTimers();
		void OnUpdateFPS(wxTimerEvent& event);
		void OnCheckFocus(wxTimerEvent& event);
		void OnChangePage(wxNotebookEvent & event);
		void OnDeselectCatapult(wxActivateEvent & event);
		void SetFPSdisplay(wxString val);
		void UpdateLed(wxString ledname, wxString ledstate);
		void UpdateState (wxString statename, wxString state);
		SessionPage * m_sessionPage;
		StatusPage * m_statusPage;
		VideoControlPage * m_videoControlPage;
		MiscControlPage * m_miscControlPage;
		AudioControlPage * m_audioControlPage;

		openMSXController * m_controller;
		wxNotebook * m_tabControl;
		wxButton * m_launch_AbortButton;
	private:
		wxStaticBitmap * m_powerLed;
		wxStaticBitmap * m_capsLed;
		wxStaticBitmap * m_kanaLed;
		wxStaticBitmap * m_pauseLed;
		wxStaticBitmap * m_turboLed;
		wxStaticBitmap * m_fddLed;
		
		CatapultXMLParser * m_parser;
		
		wxTimer m_fpsTimer;
		wxTimer m_focusTimer;
		wxWindow * m_currentFocus;
		wxString m_tempStatus;
		bool WriteMessage (wxString msg);

		DECLARE_CLASS(wxCatapultFrame)
		// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // wxCatapultFrame_H
