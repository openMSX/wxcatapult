#ifndef MISCCONTROLPAGE_H
#define MISCCONTROLPAGE_H

#include "CatapultPage.h"
#include <wx/slider.h>
#include <wx/timer.h>

class openMSXController;
class wxBitmapButton;
class wxButton;
class wxComboBox;
class wxStaticText;
class wxTextCtrl;
class wxToggleButton;

class MiscControlPage : public CatapultPage
{
public:
	MiscControlPage(wxWindow* parent, openMSXController& controller);

	void SetControlsOnEnd();
	void SetControlsOnLaunch();
	void EnableFirmware(wxString setting);
	void EnableRenShaTurbo();
	void InitConnectorPanel();
	void FillInitialJoystickPortValues();

private:
	void InvalidPrinterLogFilename();
	void InitJoystickPort(wxString connector, wxString control, wxString connectorClass);
	void OnSetThrottle(wxCommandEvent& event);
	void OnSetCmdTiming(wxCommandEvent& event);
	void OnSetNormalSpeed(wxCommandEvent& event);
	void OnSetMaxSpeed(wxCommandEvent& event);
	void OnSpeedChange(wxScrollEvent& event);
	void OnSetDefaultMinFrameSkip(wxCommandEvent& event);
	void OnSetDefaultMaxFrameSkip(wxCommandEvent& event);
	void OnMinFrameSkipChange(wxScrollEvent& event);
	void OnMaxFrameSkipChange(wxScrollEvent& event);
	void OnFirmware(wxCommandEvent& event);
	void OnPower(wxCommandEvent& event);
	void OnPause(wxCommandEvent& event);
	void OnReset(wxCommandEvent& event);
	void OnInputMinFrameskip(wxCommandEvent& event);
	void OnInputMaxFrameskip(wxCommandEvent& event);
	void OnChangeJoystick(wxCommandEvent& event);
	void OnChangePrinterPort(wxCommandEvent& event);
	void OnChangePrinterLogFile(wxCommandEvent& event);
	void OnBrowsePrinterLogFile(wxCommandEvent& event);
	void OnChangeRenShaTurbo(wxScrollEvent& event);
	void OnJoystickChangedTimer(wxTimerEvent& event);
	void OnInputSpeed(wxCommandEvent& event);
	void OnPrinterportChanged(bool save);
	void OnJoystickChanged();

	openMSXController& m_controller;
	wxBitmapButton* m_browsePrinterLog;

	wxButton* m_resetButton;
	wxButton* m_speedNormalButton;
	wxToggleButton* m_speedMaxButton;
	wxButton* m_defaultMinFrameSkipButton;
	wxButton* m_defaultMaxFrameSkipButton;

	wxStaticText* m_emulationSpeedLabel;
	wxStaticText* m_frameskipLabel;
	wxStaticText* m_frameskipMinLabel;
	wxStaticText* m_frameskipMaxLabel;
	wxStaticText* m_printerportLabel;
	wxStaticText* m_renshaLabel;
	wxStaticText* m_printerportFileLabel;

	wxToggleButton* m_powerButton;
	wxToggleButton* m_pauseButton;
	wxToggleButton* m_firmwareButton;
	wxSlider* m_speedSlider;
	wxSlider* m_maxFrameSkipSlider;
	wxSlider* m_minFrameSkipSlider;
	wxSlider* m_renshaTurboSlider;
	wxTextCtrl* m_speedIndicator;
	wxTextCtrl* m_minFrameSkipIndicator;
	wxTextCtrl* m_maxFrameSkipIndicator;
	wxTextCtrl* m_printerLogFile;
	wxString m_oldJoy1;
	wxString m_oldJoy2;
	wxString m_firmwareSetting;
	wxComboBox* m_lastUpdatedCombo;
	wxComboBox* m_printerportSelector;

#ifdef __UNIX__
	wxTimer m_joystick_update_timer; // bad solution for a worse bug in (wx?)gtk
#endif
	DECLARE_CLASS(MiscControlPage)
	DECLARE_EVENT_TABLE()
};

#endif
