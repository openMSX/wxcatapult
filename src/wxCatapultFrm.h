// $Id: wxCatapultFrm.h,v 1.7 2004/03/21 13:50:14 manuelbi Exp $ 
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
		void StartTimers();
		void StopTimers();
		void OnUpdateFPS(wxTimerEvent& event);
		void OnCheckFocus(wxTimerEvent& event);
		void OnChangePage(wxNotebookEvent & event);
		void OnDeselectCatapult(wxActivateEvent & event);
		void SetFPSdisplay(wxString val);
		SessionPage * m_sessionPage;
		StatusPage * m_statusPage;
		VideoControlPage * m_videoControlPage;
		MiscControlPage * m_miscControlPage;
		AudioControlPage * m_audioControlPage;

		openMSXController * m_controller;
		wxNotebook * m_tabControl;
		wxButton * m_launch_AbortButton;
	private:
		CatapultXMLParser * m_parser;
		
		int m_InsertedMedia;
		wxString m_usedMachine;
		wxString m_usedExtensions;
		
		wxTimer m_fpsTimer;
		wxTimer m_focusTimer;
		wxWindow * m_currentFocus;

		void SaveHistory();
		void RestoreHistory();
		void AddHistory (wxComboBox * media);

		bool WriteMessage (wxString msg);

		DECLARE_CLASS(wxCatapultFrame)
		// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // wxCatapultFrame_H
