// $Id: MiscControlPage.cpp,v 1.24 2004/05/09 14:25:51 manuelbi Exp $
// MiscControlPage.cpp: implementation of the MiscControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "SessionPage.h"
#include "MiscControlPage.h"
#include "wxCatapultFrm.h"
#include "openMSXController.h"

#include <wx/joystick.h>

IMPLEMENT_CLASS(MiscControlPage, wxPanel)
BEGIN_EVENT_TABLE(MiscControlPage, wxPanel)
	EVT_BUTTON(XRCID("ResetButton"),MiscControlPage::OnReset)
	EVT_TOGGLEBUTTON (XRCID("PauseButton"),MiscControlPage::OnPause)
	EVT_TOGGLEBUTTON (XRCID("PowerButton"),MiscControlPage::OnPower)
	EVT_TOGGLEBUTTON (XRCID("FirmwareButton"),MiscControlPage::OnFirmware)
	EVT_COMMAND_SCROLL (XRCID("SpeedSlider"),MiscControlPage::OnSpeedChange)
	EVT_BUTTON(XRCID("NormalSpeedButton"),MiscControlPage::OnSetNormalSpeed)
	EVT_BUTTON(XRCID("MaxSpeedButton"),MiscControlPage::OnSetMaxSpeed)
	EVT_COMMAND_SCROLL (XRCID("FrameSkipSlider"),MiscControlPage::OnFrameSkipChange)
	EVT_BUTTON(XRCID("ZeroFrameSkipButton"),MiscControlPage::OnSetZeroFrameSkip)
	EVT_TOGGLEBUTTON(XRCID("AutoFrameSkipButton"),MiscControlPage::OnSetAutoFrameSkip)
	EVT_TOGGLEBUTTON(XRCID("ThrottleButton"),MiscControlPage::OnSetThrottle)
	EVT_TOGGLEBUTTON(XRCID("CmdTimingButton"),MiscControlPage::OnSetCmdTiming)
	EVT_TEXT(XRCID("FrameSkipIndicator"),MiscControlPage::OnInputFrameskip)
	EVT_TEXT(XRCID("SpeedIndicator"),MiscControlPage::OnInputSpeed)
	EVT_TEXT(XRCID("Joyport1Selector"),MiscControlPage::OnChangeJoystick)
	EVT_TEXT(XRCID("Joyport2Selector"),MiscControlPage::OnChangeJoystick)
#ifdef __UNIX__
	EVT_TIMER(-1,MiscControlPage::OnJoystickChanged)
#endif
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

MiscControlPage::MiscControlPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
#ifdef __UNIX__
	m_joystick_update_timer.SetOwner(this,-1);
#endif
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("MiscControlPage"));
	m_controller = controller;
	m_powerButton = (wxToggleButton *)FindWindow(wxT("PowerButton"));
	m_pauseButton = (wxToggleButton *)FindWindow(wxT("PauseButton"));	
	m_resetButton = (wxButton *)FindWindow(wxT("ResetButton"));
	m_firmwareButton = (wxToggleButton *)FindWindow(wxT("FirmwareButton"));
	m_speedIndicator = (wxTextCtrl *)FindWindow(wxT("SpeedIndicator"));
	m_speedSlider = (wxSlider *)FindWindow(wxT("SpeedSlider"));

	m_speedNormalButton = (wxButton *)FindWindow (wxT("NormalSpeedButton"));
	m_speedMaxButton = (wxButton *)FindWindow (wxT("MaxSpeedButton"));
	m_frameSkipIndicator = (wxTextCtrl *)FindWindow(wxT("FrameSkipIndicator"));
	m_maxFrameSkipSlider = (wxSlider *)FindWindow(wxT("MaxFrameSkipSlider"));
	m_minFrameSkipSlider = (wxSlider *)FindWindow(wxT("MinFrameSkipSlider"));

	m_frameSkipZeroButton = (wxButton *)FindWindow (wxT("ZeroFrameSkipButton"));
	m_throttleButton = (wxToggleButton *)FindWindow (wxT("ThrottleButton"));
	m_cmdTimingButton = (wxToggleButton *)FindWindow (wxT("CmdTimingButton"));

	m_speedSlider->SetTickFreq (25,1);
	m_maxFrameSkipSlider->SetTickFreq (5,1);
	m_minFrameSkipSlider->SetTickFreq (5,1);

// temporary hardcoded joystick port devices (not for BSD)

#ifndef __BSD__ 
	wxJoystick joy(wxJOYSTICK1);
	wxString temp;
#endif

	wxComboBox * box[2];
	box[0] = (wxComboBox *)FindWindow (wxT("Joyport1Selector"));
	box[1] = (wxComboBox *)FindWindow (wxT("Joyport2Selector"));
	for (int i=0;i<2;i++) {
		box[i]->Clear();
		box[i]->Append(wxT("--empty--"));
		box[i]->Append(wxT("mouse"));
		box[i]->Append(wxT("keyjoystick"));
#ifndef __BSD__
		for (int j=1;j<=joy.GetNumberJoysticks();j++) {
			temp.sprintf("joystick%d",j);
			box[i]->Append(temp);
		}
#endif
		box[i]->SetSelection(0);
	}
	m_oldJoy1 = wxT("--empty--");
	m_oldJoy2 = wxT("--empty--");
}

MiscControlPage::~MiscControlPage()
{
}

void MiscControlPage::OnReset(wxCommandEvent &event)
{
	m_controller->WriteCommand ("reset");
}

void MiscControlPage::OnPause(wxCommandEvent &event)
{
	m_controller->WriteCommand ("toggle pause");
}

void MiscControlPage::OnPower(wxCommandEvent &event)
{
	m_controller->WriteCommand ("toggle power");
	if (m_powerButton->GetValue())
	{
		m_controller->WriteCommand ("set pause off");
		m_pauseButton->SetValue(false);
	}
}

void MiscControlPage::OnFirmware(wxCommandEvent &event)
{
	m_controller->WriteCommand ("toggle frontswitch");
}

void MiscControlPage::OnSpeedChange(wxScrollEvent &event)
{
	wxString speedText;
	speedText.sprintf("%ld", event.GetInt());
	m_speedIndicator->SetValue(speedText);
	m_controller->WriteCommand (wxString("set speed ") + speedText);
	m_throttleButton->SetValue(true);
	m_throttleButton->SetLabel(wxT("On"));
	m_controller->WriteCommand ("set throttle on");
}

void MiscControlPage::OnSetNormalSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set speed 100");
	m_speedIndicator->SetValue(wxT("100"));
	m_speedSlider->SetValue (100);
	m_throttleButton->SetValue(true);
	m_throttleButton->SetLabel(wxT("On"));
	m_controller->WriteCommand ("set throttle on");
}

void MiscControlPage::OnSetMaxSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set speed 500");
	m_speedIndicator->SetValue(wxT("500"));
	m_speedSlider->SetValue (500);
	m_throttleButton->SetValue(false);
	m_throttleButton->SetLabel(wxT("Off"));
	m_controller->WriteCommand ("set throttle off");
}

void MiscControlPage::OnFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf("%ld", event.GetInt());
	m_frameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString("set maxframeskip " + skipText));
}

void MiscControlPage::OnSetZeroFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set maxframeskip 0");
	m_frameSkipIndicator->Enable();
	m_frameSkipIndicator->SetValue(wxT("0"));
	m_maxFrameSkipSlider->Enable();
	m_maxFrameSkipSlider->SetValue (0);
}

void MiscControlPage::OnSetAutoFrameSkip(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue()) {
		m_frameSkipIndicator->SetValue(wxT("auto"));
	}
	else{
		int val = m_maxFrameSkipSlider->GetValue();
		wxString strval;
		strval.sprintf ("%d",val);
		m_frameSkipIndicator->SetValue(strval);
	}
}

void MiscControlPage::OnSetThrottle(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand ("set throttle on");
		button->SetLabel("On");
	}
	else
	{
		m_controller->WriteCommand ("set throttle off");
		button->SetLabel("Off");
	}	
}

void MiscControlPage::OnSetCmdTiming(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand ("set cmdtiming broken");
		button->SetLabel("Broken");
	}
	else
	{
		m_controller->WriteCommand ("set cmdtiming real");
		button->SetLabel("Real");
	}
}

void MiscControlPage::SetControlsOnLaunch()
{
	m_speedSlider->Enable(true);
	m_speedIndicator->Enable(true);
	m_speedNormalButton->Enable(true);
	m_speedMaxButton->Enable(true);

	m_maxFrameSkipSlider->Enable(true);
	m_frameSkipIndicator->Enable(true);
	m_frameSkipZeroButton->Enable(true);

	m_powerButton->Enable(true);
	m_resetButton->Enable(true);
	m_pauseButton->Enable(true);

	m_throttleButton->Enable(true);
	m_cmdTimingButton->Enable(true);
	m_pauseButton->SetValue(false);
	m_firmwareButton->SetValue(false);
}

void MiscControlPage::EnableFirmware()
{
	m_firmwareButton->Enable(true);
}

void MiscControlPage::SetControlsOnEnd()
{
	m_speedSlider->Enable(false);
	m_speedIndicator->Enable(false);
	m_speedNormalButton->Enable(false);
	m_speedMaxButton->Enable(false);

	m_maxFrameSkipSlider->Enable(false);
	m_frameSkipIndicator->Enable(false);
	m_frameSkipZeroButton->Enable(false);

	m_powerButton->Enable(false);
	m_resetButton->Enable(false);
	m_firmwareButton->Enable(false);
	m_pauseButton->Enable(false);

	m_throttleButton->Enable(false);
	m_cmdTimingButton->Enable(false);
}

void MiscControlPage::OnInputSpeed(wxCommandEvent &event)
{
	wxString text = m_speedIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num >500) 
		{
			num = 500;
			text = wxT("500");
			m_speedIndicator->SetValue(text);
		}
		if (num >= 1)
		{
			m_controller->WriteCommand (wxString("set speed ") + text);
			m_speedSlider->SetValue(num);
		}
	}
}

void MiscControlPage::OnInputFrameskip(wxCommandEvent &event)
{
	wxString text = m_frameSkipIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100)
		{
			num = 100;
			text = wxT("100");
			m_frameSkipIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString("set maxframeskip " + text));
			m_maxFrameSkipSlider->SetValue(num);
		}
	}	
}

void MiscControlPage::SetSpeed (wxString value)
{
	m_speedIndicator->SetValue(value);
}

void MiscControlPage::SetFrameskip (wxString value)
{
	m_frameSkipIndicator->SetValue(value);
}

void MiscControlPage::SetThrottle (wxString value)
{
	if (value == wxT("on")) {
		m_throttleButton->SetValue(true);
		m_throttleButton->SetLabel(wxT("On"));
	}
	else {
		m_throttleButton->SetValue(false);
		m_throttleButton->SetLabel(wxT("Off"));
	}
}

void MiscControlPage::SetCmdTiming (wxString value)
{
	if (value == wxT("broken")) {
		m_cmdTimingButton->SetValue(true);
		m_cmdTimingButton->SetLabel(wxT("Broken"));
	}
	else {
		m_cmdTimingButton->SetValue(false);
		m_cmdTimingButton->SetLabel(wxT("Real"));
	}
}

void MiscControlPage::InitConnectorPanel ()
{
	wxArrayString connectors;
	wxString currentClass;
	m_controller->GetConnectors(connectors);
	if (connectors.GetCount() > 0) {
		for (unsigned i=0;i<connectors.GetCount();i++) {
			currentClass = m_controller->GetConnectorClass(connectors[i]);
			if (connectors[i] == wxT("joyporta")) {
				InitJoystickPort (connectors[i], "Joyport1Selector", currentClass);
			} else if (connectors[i] == wxT("joyportb")) {
				InitJoystickPort (connectors[i], "Joyport2Selector", currentClass);
			}
		}
	}
}

void MiscControlPage::InitJoystickPort (wxString connector, wxString control, wxString connectorClass)
{
	wxArrayString pluggables;
	wxArrayString classes;
	m_controller->GetPluggableClasses(classes);
	if (classes.GetCount() ==0) return;
	wxComboBox * box = (wxComboBox *)FindWindow (control);
	wxString currentval = box->GetValue();
	box->Clear();
	box->Append(wxT("--empty--"));
	wxString currentClass;
	m_controller->GetPluggables(pluggables);
	if (pluggables.GetCount() > 0) {
		for (unsigned i=0;i<pluggables.GetCount();i++) {
			if (classes[i] == connectorClass) {
				box->Append(pluggables[i]);
			}
		}
	}
	box->SetValue(currentval);
	wxString cmd = "plug";
	if (currentval == wxT("--empty--")) {
		cmd = "unplug";
		currentval = "";
	}
	m_controller->WriteCommand(wxString(cmd + " " + connector + " " + currentval));
}

void MiscControlPage::OnChangeJoystick(wxCommandEvent & event)
{
	m_lastUpdatedCombo = (wxComboBox *)event.GetEventObject();
#ifdef __UNIX__
	if (!m_joystick_update_timer.IsRunning()) {
		m_joystick_update_timer.Start(200,true);
	}
#else
	OnJoystickChanged();
#endif
}

void MiscControlPage::OnJoystickChanged()
{
	static bool MessageActive = false;
	wxComboBox * box = m_lastUpdatedCombo;
	if ((box == NULL) || MessageActive) {
		return;
	}
	wxComboBox * box2 = NULL;
	wxString * oldValue1 = NULL; // this port
	wxString * oldValue2 = NULL; // the other port
	wxString cmd = "unplug ";
	wxString connector;
	wxString connector2;
	wxString value = "";
	if (box->GetName() == "Joyport1Selector") {
		connector = "joyporta ";
		connector2 = "joyportb";
		box2 = (wxComboBox *)FindWindow (wxT("Joyport2Selector"));
		oldValue1 = &m_oldJoy1;
		oldValue2 = &m_oldJoy2;
	} 
	else if (box->GetName() == "Joyport2Selector") {
		connector = "joyportb ";
		connector2 = "joyporta";
		box2 = (wxComboBox *)FindWindow (wxT("Joyport1Selector"));
		oldValue1 = &m_oldJoy2;
		oldValue2 = &m_oldJoy1;
	}
	
	if (box->GetValue() != wxT("--empty--")) {
		value = box->GetValue();
		cmd = "plug ";
	}

	if ((box->GetValue() != ("--empty--")) && (box->GetValue() == box2->GetValue())) {
		MessageActive = true;
		int result = wxMessageBox ("Unable to plug a device in more than one port!\n\n\
Do you still want to plug it into this port?\n\
This device will then be removed from any other port(s).","Warning",wxOK | wxCANCEL);
		MessageActive = false;
		if (result == wxOK) {
			box2->SetSelection(0);
			*oldValue2 = wxT("--empty--");
			*oldValue1 = box->GetValue();
			m_controller->WriteCommand(wxString("unplug " + connector2));
			m_controller->WriteCommand(wxString(cmd + connector + value));
		}
		else{ // cancel
			box->SetSelection(box->FindString(*oldValue1));
		}
	}
	else{ // no collision
		m_controller->WriteCommand(wxString(cmd + connector + value));
		*oldValue1 = box->GetValue();
	}	

}
