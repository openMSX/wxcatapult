// $Id: MiscControlPage.cpp,v 1.5 2004/03/21 13:50:14 manuelbi Exp $
// MiscControlPage.cpp: implementation of the MiscControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SessionPage.h"
#include "MiscControlPage.h"
#include "wxCatapultFrm.h"
#include "openMSXController.h"

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
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

MiscControlPage::MiscControlPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("MiscControlPage"));
	m_controller = controller;
	m_powerButton = (wxToggleButton *)FindWindow(_("PowerButton"));
	m_pauseButton = (wxToggleButton *)FindWindow(_("PauseButton"));	
	m_resetButton = (wxButton *)FindWindow(_("ResetButton"));
	m_firmwareButton = (wxToggleButton *)FindWindow(_("FirmwareButton"));
	m_speedIndicator = (wxTextCtrl *)FindWindow(_("SpeedIndicator"));
	m_speedSlider = (wxSlider *)FindWindow(_("SpeedSlider"));

	m_speedNormalButton = (wxButton *)FindWindow (_("NormalSpeedButton"));
	m_speedMaxButton = (wxButton *)FindWindow (_("MaxSpeedButton"));
	m_frameSkipIndicator = (wxTextCtrl *)FindWindow(_("FrameSkipIndicator"));
	m_frameSkipSlider = (wxSlider *)FindWindow(_("FrameSkipSlider"));

	m_frameSkipZeroButton = (wxButton *)FindWindow (_("ZeroFrameSkipButton"));
	m_frameSkipAutoButton = (wxToggleButton *)FindWindow (_("AutoFrameSkipButton"));
	m_throttleButton = (wxToggleButton *)FindWindow (_("ThrottleButton"));
	m_cmdTimingButton = (wxToggleButton *)FindWindow (_("CmdTimingButton"));

	m_speedSlider->SetTickFreq (25,1);
	m_frameSkipSlider->SetTickFreq (5,1);
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
	m_controller->WriteCommand (wxString(_("set speed ")) + speedText);
	m_throttleButton->SetValue(true);
	m_throttleButton->SetLabel(_("On"));
	m_controller->WriteCommand (_("set throttle on"));
}

void MiscControlPage::OnSetNormalSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set speed 100");
	m_speedIndicator->SetValue(_("100"));
	m_speedSlider->SetValue (100);
	m_throttleButton->SetValue(true);
	m_throttleButton->SetLabel(_("On"));
	m_controller->WriteCommand (_("set throttle on"));
}

void MiscControlPage::OnSetMaxSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set speed 500");
	m_speedIndicator->SetValue(_("500"));
	m_speedSlider->SetValue (500);
	m_throttleButton->SetValue(false);
	m_throttleButton->SetLabel(_("Off"));
	m_controller->WriteCommand (_("set throttle off"));
}

void MiscControlPage::OnFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf("%ld", event.GetInt());
	m_frameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString(_("set frameskip ")) + skipText);
}

void MiscControlPage::OnSetZeroFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set frameskip 0");
	m_frameSkipIndicator->Enable();
	m_frameSkipIndicator->SetValue(_("0"));
	m_frameSkipSlider->Enable();
	m_frameSkipSlider->SetValue (0);
	m_frameSkipAutoButton->SetValue(false);
}

void MiscControlPage::OnSetAutoFrameSkip(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (_("set frameskip auto"));
		m_frameSkipIndicator->Disable();
		m_frameSkipSlider->Disable();
	}
	else
	{
		m_frameSkipIndicator->Enable();
		m_controller->WriteCommand (wxString(_("set frameskip ")) + m_frameSkipIndicator->GetValue());
		m_frameSkipSlider->Enable();
	}
}

void MiscControlPage::OnSetThrottle(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (_("set throttle on"));
		button->SetLabel("On");
	}
	else
	{
		m_controller->WriteCommand (_("set throttle off"));
		button->SetLabel("Off");
	}	
}

void MiscControlPage::OnSetCmdTiming(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (_("set cmdtiming broken"));
		button->SetLabel("Broken");
	}
	else
	{
		m_controller->WriteCommand (_("set cmdtiming real"));
		button->SetLabel("Real");
	}
}

void MiscControlPage::EnableControls()
{
	m_speedSlider->Enable(true);
	m_speedIndicator->Enable(true);
	m_speedNormalButton->Enable(true);
	m_speedMaxButton->Enable(true);

	m_frameSkipSlider->Enable(true);
	m_frameSkipIndicator->Enable(true);
	m_frameSkipAutoButton->Enable(true);
	m_frameSkipZeroButton->Enable(true);

	m_powerButton->Enable(true);
	m_resetButton->Enable(true);
	m_firmwareButton->Enable(true);
	m_pauseButton->Enable(true);

	m_throttleButton->Enable(true);
	m_cmdTimingButton->Enable(true);
}

void MiscControlPage::DisableControls()
{
	m_speedSlider->Enable(false);
	m_speedIndicator->Enable(false);
	m_speedNormalButton->Enable(false);
	m_speedMaxButton->Enable(false);

	m_frameSkipSlider->Enable(false);
	m_frameSkipIndicator->Enable(false);
	m_frameSkipAutoButton->Enable(false);
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
			text = _("500");
			m_speedIndicator->SetValue(text);
		}
		if (num >= 1)
		{
			m_controller->WriteCommand (wxString(_("set speed ")) + text);
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
			text = _("100");
			m_frameSkipIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString(_("set frameskip ")) + text);
			m_frameSkipSlider->SetValue(num);
			m_frameSkipIndicator->Enable();
			m_frameSkipSlider->Enable();
			m_frameSkipAutoButton->SetValue(false);
		}
	}	
	if (!text.CmpNoCase(_("auto")))
	{
		m_controller->WriteCommand (_("set frameskip auto"));
		m_frameSkipIndicator->Disable();
		m_frameSkipSlider->Disable();
		m_frameSkipAutoButton->SetValue(true);
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
	if (value == _("on")){
		m_throttleButton->SetValue(true);
		m_throttleButton->SetLabel(_("On"));
	}
	else {
		m_throttleButton->SetValue(false);
		m_throttleButton->SetLabel(_("Off"));
	}
}

void MiscControlPage::SetCmdTiming (wxString value)
{
	if (value == _("broken")){
		m_cmdTimingButton->SetValue(true);
		m_cmdTimingButton->SetLabel(_("Broken"));
	}
	else {
		m_cmdTimingButton->SetValue(false);
		m_cmdTimingButton->SetLabel(_("Real"));
	}
}
