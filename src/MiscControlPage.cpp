// $Id: MiscControlPage.cpp,v 1.31 2004/10/03 20:08:26 h_oudejans Exp $
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
#include "ConfigurationData.h"

#include <wx/joystick.h>

IMPLEMENT_CLASS(MiscControlPage, wxPanel)
BEGIN_EVENT_TABLE(MiscControlPage, wxPanel)
	EVT_BUTTON(XRCID("ResetButton"),MiscControlPage::OnReset)
	EVT_TOGGLEBUTTON (XRCID("PauseButton"),MiscControlPage::OnPause)
	EVT_TOGGLEBUTTON (XRCID("PowerButton"),MiscControlPage::OnPower)
	EVT_TOGGLEBUTTON (XRCID("FirmwareButton"),MiscControlPage::OnFirmware)
	EVT_COMMAND_SCROLL (XRCID("SpeedSlider"),MiscControlPage::OnSpeedChange)
	EVT_BUTTON(XRCID("NormalSpeedButton"),MiscControlPage::OnSetNormalSpeed)
	EVT_TOGGLEBUTTON(XRCID("MaxSpeedButton"),MiscControlPage::OnSetMaxSpeed)
	EVT_COMMAND_SCROLL (XRCID("MinFrameSkipSlider"),MiscControlPage::OnMinFrameSkipChange)
	EVT_COMMAND_SCROLL (XRCID("MaxFrameSkipSlider"),MiscControlPage::OnMaxFrameSkipChange)
	EVT_BUTTON(XRCID("DefaultMinFrameSkipButton"),MiscControlPage::OnSetDefaultMinFrameSkip)
	EVT_BUTTON(XRCID("DefaultMaxFrameSkipButton"),MiscControlPage::OnSetDefaultMaxFrameSkip)
	EVT_TOGGLEBUTTON(XRCID("ThrottleButton"),MiscControlPage::OnSetThrottle)
	EVT_TOGGLEBUTTON(XRCID("CmdTimingButton"),MiscControlPage::OnSetCmdTiming)
	EVT_TEXT(XRCID("MinFrameSkipIndicator"),MiscControlPage::OnInputMinFrameskip)
	EVT_TEXT(XRCID("MaxFrameSkipIndicator"),MiscControlPage::OnInputMaxFrameskip)
	EVT_TEXT(XRCID("SpeedIndicator"),MiscControlPage::OnInputSpeed)
	EVT_TEXT(XRCID("Joyport1Selector"),MiscControlPage::OnChangeJoystick)
	EVT_TEXT(XRCID("Joyport2Selector"),MiscControlPage::OnChangeJoystick)
	EVT_TEXT(XRCID("PrinterportSelector"),MiscControlPage::OnChangePrinterPort)
	EVT_TEXT(XRCID("PrinterLogFile"),MiscControlPage::OnChangePrinterLogFile)
	EVT_BUTTON(XRCID("BrowsePrinterLog"),MiscControlPage::OnBrowsePrinterLogFile)
	EVT_COMMAND_SCROLL (XRCID("RenshaTurboSlider"),MiscControlPage::OnChangeRenShaTurbo)
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
	m_speedMaxButton = (wxToggleButton *)FindWindow (wxT("MaxSpeedButton"));
	m_minFrameSkipIndicator = (wxTextCtrl *)FindWindow(wxT("MinFrameSkipIndicator"));
	m_maxFrameSkipIndicator = (wxTextCtrl *)FindWindow(wxT("MaxFrameSkipIndicator"));
	m_maxFrameSkipSlider = (wxSlider *)FindWindow(wxT("MaxFrameSkipSlider"));
	m_minFrameSkipSlider = (wxSlider *)FindWindow(wxT("MinFrameSkipSlider"));
	m_renshaTurboSlider = (wxSlider *)FindWindow(wxT("RenshaTurboSlider"));
	m_browsePrinterLog = (wxBitmapButton*) FindWindow(wxT("BrowsePrinterLog"));
	m_printerLogFile = (wxTextCtrl *)FindWindow(wxT("PrinterLogFile"));
	m_printerportSelector = (wxComboBox*)FindWindow(wxT("PrinterportSelector"));

	m_defaultMaxFrameSkipButton = (wxButton *)FindWindow (wxT("DefaultMaxFrameSkipButton"));
	m_defaultMinFrameSkipButton = (wxButton *)FindWindow (wxT("DefaultMinFrameSkipButton"));
	
	m_speedSlider->SetTickFreq (25,1);
	m_maxFrameSkipSlider->SetTickFreq (5,1);
	m_minFrameSkipSlider->SetTickFreq (5,1);
	m_renshaTurboSlider->SetTickFreq (5,1);
	m_oldSpeed="";

	wxBitmap & tempBmp = m_browsePrinterLog->GetBitmapLabel();
	tempBmp.SetMask(new wxMask(tempBmp,wxColour(255,0,255)));
	m_browsePrinterLog->Enable(false);
	m_browsePrinterLog->Enable(true);
	
// temporary hardcoded joystick port devices (not for BSD)
	wxString current;
	int pos;

#ifndef __BSD__ 
	wxJoystick joy(wxJOYSTICK1);
	wxString temp;
#endif
	int JoySaveID[]={ConfigurationData::CD_JOYPORT1, ConfigurationData::CD_JOYPORT2};
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
		ConfigurationData::instance()->GetParameter(JoySaveID[i],current);
		pos = box[i]->FindString(current);
		if (pos != -1){
			box[i]->SetSelection(pos);
		}		
		else{	
			box[i]->SetSelection(0);
		}	
	}
	m_oldJoy1 = box[0]->GetValue();
	m_oldJoy2 = box[1]->GetValue();

	
	
	box[0] = (wxComboBox *)FindWindow (wxT("PrinterportSelector"));
	box[0]->Clear();
	box[0]->Append(wxT("--empty--"));
	box[0]->Append(wxT("logger"));
	box[0]->Append(wxT("simpl"));	

	wxString filename;
	wxTextCtrl * text = (wxTextCtrl *)FindWindow("PrinterLogFile");
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_PRINTERPORT,current);
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_PRINTERFILE,filename);
	text->SetValue(filename);
		
	pos = box[0]->FindString(current);
	if (pos != -1){
			box[0]->SetSelection(pos);
		}		
		else{	
			box[0]->SetSelection(0);
		}
	OnPrinterportChanged(false);
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
	m_controller->WriteCommand ("set throttle on");
}

void MiscControlPage::OnSetNormalSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand ("set speed 100");
	m_speedIndicator->SetValue(wxT("100"));
	m_speedSlider->SetValue (100);
	m_controller->WriteCommand ("set throttle on");
}

void MiscControlPage::OnSetMaxSpeed(wxCommandEvent &event)
{
	if (m_speedMaxButton->GetValue()){
		m_oldSpeed = m_speedIndicator->GetValue();
		m_controller->WriteCommand ("set speed 500");
		m_speedSlider->Disable();
		m_speedIndicator->Disable();
		m_speedNormalButton->Disable();
		m_speedIndicator->SetValue(wxT("500"));
		m_controller->WriteCommand ("set throttle off");
	}
	else{
		if (m_oldSpeed != ""){
			m_controller->WriteCommand (wxString("set speed ") + m_oldSpeed);
			m_speedSlider->Enable();
			m_speedIndicator->Enable();
			m_speedNormalButton->Enable();
			m_speedIndicator->SetValue(m_oldSpeed);
			m_controller->WriteCommand ("set throttle on");
			m_oldSpeed = "";
		}
	}
}

void MiscControlPage::OnMaxFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf("%ld", event.GetInt());
	m_maxFrameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString("set maxframeskip " + skipText));
}

void MiscControlPage::OnMinFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf("%ld", event.GetInt());
	m_minFrameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString("set minframeskip " + skipText));
}

void MiscControlPage::OnSetDefaultMaxFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand ("unset maxframeskip");
	m_maxFrameSkipIndicator->Enable();
	m_maxFrameSkipSlider->Enable();
}

void MiscControlPage::OnSetDefaultMinFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand ("unset minframeskip");
	m_minFrameSkipIndicator->Enable();
	m_minFrameSkipSlider->Enable();
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
	m_minFrameSkipSlider->Enable(true);
	m_minFrameSkipIndicator->Enable(true);
	m_maxFrameSkipIndicator->Enable(true);
	m_defaultMinFrameSkipButton->Enable(true);
	m_defaultMaxFrameSkipButton->Enable(true);

	m_powerButton->Enable(true);
	m_resetButton->Enable(true);
	m_pauseButton->Enable(true);

	m_pauseButton->SetValue(false);
	m_firmwareButton->SetValue(false);
}

void MiscControlPage::EnableFirmware()
{
	m_firmwareButton->Enable(true);
}

void MiscControlPage::EnableRenShaTurbo()
{
	m_renshaTurboSlider->Enable(true);
}

void MiscControlPage::SetControlsOnEnd()
{
	m_speedSlider->Enable(false);
	m_speedIndicator->Enable(false);
	m_speedNormalButton->Enable(false);
	m_speedMaxButton->Enable(false);

	m_maxFrameSkipSlider->Enable(false);
	m_minFrameSkipSlider->Enable(false);
	m_minFrameSkipIndicator->Enable(false);
	m_maxFrameSkipIndicator->Enable(false);
	m_defaultMinFrameSkipButton->Enable(false);
	m_defaultMaxFrameSkipButton->Enable(false);

	m_powerButton->Enable(false);
	m_resetButton->Enable(false);
	m_firmwareButton->Enable(false);
	m_pauseButton->Enable(false);
	m_renshaTurboSlider->Enable(false);
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

void MiscControlPage::OnInputMinFrameskip(wxCommandEvent &event)
{
	wxString text = m_minFrameSkipIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100)
		{
			num = 100;
			text = wxT("100");
			m_minFrameSkipIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString("set minframeskip " + text));
			m_minFrameSkipSlider->SetValue(num);
		}
	}	
}

void MiscControlPage::OnInputMaxFrameskip(wxCommandEvent &event)
{
	wxString text = m_maxFrameSkipIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100)
		{
			num = 100;
			text = wxT("100");
			m_maxFrameSkipIndicator->SetValue(text);
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

void MiscControlPage::SetMinFrameskip (wxString value)
{
	m_minFrameSkipIndicator->SetValue(value);
}

void MiscControlPage::SetMaxFrameskip (wxString value)
{
	m_maxFrameSkipIndicator->SetValue(value);
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
			} else if (connectors[i] == wxT("printerport")){
				InitJoystickPort (connectors[i], "PrinterportSelector", currentClass);
			}
		}
	}
	wxTextCtrl * text = (wxTextCtrl *)FindWindow("PrinterLogFile");
	m_controller->WriteCommand(wxString("set printerlogfilename ") + ConvertPath(text->GetValue()));
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
	wxComboBox * joy1 = (wxComboBox *)FindWindow (wxT("Joyport1Selector"));
	wxComboBox * joy2 = (wxComboBox *)FindWindow (wxT("Joyport2Selector"));	
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_JOYPORT1,joy1->GetValue());
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_JOYPORT2,joy2->GetValue());
	ConfigurationData::instance()->SaveData();

}

void MiscControlPage::OnChangeRenShaTurbo(wxScrollEvent & event)
{
	wxString value;
	value.sprintf ("%ld",event.GetInt());
	
	
	m_controller->WriteCommand(wxString ("set renshaturbo ") + value);
}

void MiscControlPage::OnChangePrinterPort(wxCommandEvent & event)
{
	OnPrinterportChanged(true);
}

void MiscControlPage::OnPrinterportChanged(bool save)
{
	wxString current = m_printerportSelector->GetValue();
	if (current == "logger"){
		m_printerLogFile->Enable(true);
		m_browsePrinterLog->Enable(true);
	}
	else{
		m_printerLogFile->Enable(false);
		m_browsePrinterLog->Enable(false);
	}
	if (save){
		ConfigurationData::instance()->SetParameter(ConfigurationData::CD_PRINTERPORT,current);
		ConfigurationData::instance()->SaveData();
	}
	m_controller->WriteCommand("unplug printerport");
	m_controller->WriteCommand(wxString("plug printerport ") + current);	
}

void MiscControlPage::OnChangePrinterLogFile(wxCommandEvent &event)
{
	wxString current = ((wxTextCtrl *)event.GetEventObject())->GetValue();
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_PRINTERFILE,current);
	ConfigurationData::instance()->SaveData();
	m_controller->WriteCommand(wxString("set printerlogfilename ") + ConvertPath(current));
}

void MiscControlPage::OnBrowsePrinterLogFile(wxCommandEvent &event)
{
	wxString path;
#ifndef __MOTIF__
	path = wxT("All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,_("Choose file to save printer log to"), "", wxT(""), path ,wxSAVE | wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK){
		m_printerLogFile->SetValue(filedlg.GetPath());
	}
}
