// $Id: wxCatapultFrm.h,v 1.5 2004/03/20 14:30:55 h_oudejans Exp $ 
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
		void OnApplyChanges (wxCommandEvent & event);

		// ctor(s)
		wxCatapultFrame(wxWindow * parent=(wxWindow *)NULL);
		virtual ~wxCatapultFrame();

		// event handlers (these functions should _not_ be virtual)
		void OnMenuQuit(wxCommandEvent& event);
		void OnMenuAbout(wxCommandEvent& event);
		void OnMenuEditConfig(wxCommandEvent& event);
		void OnLaunch(wxCommandEvent& event);
		void DisableControls();
		void EnableControls();
		void StartTimer();
		void StopTimer();
		void OnTimer(wxTimerEvent& event);
		void SetFPSdisplay(wxString val);

		SessionPage * m_sessionPage;
		StatusPage * m_statusPage;
		VideoControlPage * m_videoControlPage;
		MiscControlPage * m_miscControlPage;
		AudioControlPage * m_audioControlPage;

		wxString m_lastDiskA;
		wxString m_lastDiskB;
		wxString m_lastTape1;
		wxString m_lastTape2;
		wxButton * m_applyButton;
		openMSXController * m_controller;
		wxNotebook * m_tabControl;
		wxButton * m_launch_AbortButton;
	private:
		
		wxString ConvertPath (wxString path, bool ConvertSlash=false);

		
		CatapultXMLParser * m_parser;

		int m_InsertedMedia;
		wxString m_usedMachine;
		wxString m_usedExtensions;
		void SaveHistory();
		void RestoreHistory();
		void AddHistory (wxComboBox * media);

		bool WriteMessage (wxString msg);
		
		wxTimer m_timer;
		
		DECLARE_CLASS(wxCatapultFrame)
			// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // wxCatapultFrame_H
