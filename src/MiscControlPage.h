// $Id: MiscControlPage.h,v 1.16 2004/06/06 18:25:42 h_oudejans Exp $
// MiscControlPage.h: interface for the MiscControlPage class.
//
//////////////////////////////////////////////////////////////////////
#ifndef MISCCONTROLPAGE_H
#define MISCCONTROLPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/slider.h>

class wxCatapultFrame;
class openMSXController;
class MiscControlPage : public CatapultPage  
{
	public:
		void SetControlsOnEnd();
		void SetControlsOnLaunch();
		void EnableFirmware();
		MiscControlPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller=NULL);
		virtual ~MiscControlPage();
		void OnSetThrottle (wxCommandEvent &event);
		void OnSetCmdTiming (wxCommandEvent &event);	
		void OnSetNormalSpeed (wxCommandEvent & event);
		void OnSetMaxSpeed (wxCommandEvent & event);
		void OnSpeedChange (wxScrollEvent & event);
		void OnSetDefaultMinFrameSkip (wxCommandEvent & event);
		void OnSetDefaultMaxFrameSkip (wxCommandEvent & event);
		void OnMinFrameSkipChange (wxScrollEvent & event);
		void OnMaxFrameSkipChange (wxScrollEvent & event);
		void OnFirmware (wxCommandEvent & event);
		void OnPower (wxCommandEvent &event);
		void OnPause (wxCommandEvent &event);
		void OnReset (wxCommandEvent &event);
		void OnInputMinFrameskip (wxCommandEvent & event);
		void OnInputMaxFrameskip (wxCommandEvent & event);
		void OnChangeJoystick (wxCommandEvent & event);
		void OnJoystickChanged ();
		void OnInputSpeed (wxCommandEvent & event);
		void SetSpeed (wxString value);
		void SetMinFrameskip (wxString value);
		void SetMaxFrameskip (wxString value);
		void SetThrottle (wxString value);
		void SetCmdTiming (wxString value);		
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus) {};
		void InitConnectorPanel ();
		wxToggleButton * m_powerButton;
		
	private:
		void InitJoystickPort (wxString connector, wxString control, wxString connectorClass);

		openMSXController * m_controller;
		wxButton * m_resetButton;
		wxButton * m_speedNormalButton;
		wxButton * m_speedMaxButton;
		wxButton * m_defaultMinFrameSkipButton;
		wxButton * m_defaultMaxFrameSkipButton;

		wxToggleButton * m_pauseButton;
		wxToggleButton * m_firmwareButton;
		wxToggleButton * m_throttleButton;
		wxToggleButton * m_cmdTimingButton;
		wxSlider * m_speedSlider;
		wxSlider * m_maxFrameSkipSlider;
		wxSlider * m_minFrameSkipSlider;
		wxTextCtrl * m_speedIndicator;
		wxTextCtrl * m_minFrameSkipIndicator;
		wxTextCtrl * m_maxFrameSkipIndicator;
		wxString m_oldJoy1;
		wxString m_oldJoy2;
		wxComboBox * m_lastUpdatedCombo;

#ifdef __UNIX__		
		wxTimer m_joystick_update_timer; // bad solution for a worse bug in (wx?)gtk
#endif
		DECLARE_CLASS(MiscControlPage)
		// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif

