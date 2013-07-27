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
	EVT_TIMER(-1,MiscControlPage::OnJoystickChangedTimer)
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
	m_powerButton = (wxToggleButton *)FindWindowByName(wxT("PowerButton"));
	m_pauseButton = (wxToggleButton *)FindWindowByName(wxT("PauseButton"));
	m_resetButton = (wxButton *)FindWindowByName(wxT("ResetButton"));
	m_firmwareButton = (wxToggleButton *)FindWindowByName(wxT("FirmwareButton"));
	m_speedIndicator = (wxTextCtrl *)FindWindowByName(wxT("SpeedIndicator"));
	m_speedSlider = (wxSlider *)FindWindowByName(wxT("SpeedSlider"));
	m_speedNormalButton = (wxButton *)FindWindowByName (wxT("NormalSpeedButton"));
	m_speedMaxButton = (wxToggleButton *)FindWindowByName (wxT("MaxSpeedButton"));
	m_minFrameSkipIndicator = (wxTextCtrl *)FindWindowByName(wxT("MinFrameSkipIndicator"));
	m_maxFrameSkipIndicator = (wxTextCtrl *)FindWindowByName(wxT("MaxFrameSkipIndicator"));
	m_maxFrameSkipSlider = (wxSlider *)FindWindowByName(wxT("MaxFrameSkipSlider"));
	m_minFrameSkipSlider = (wxSlider *)FindWindowByName(wxT("MinFrameSkipSlider"));
	m_renshaTurboSlider = (wxSlider *)FindWindowByName(wxT("RenshaTurboSlider"));
	m_browsePrinterLog = (wxBitmapButton*) FindWindowByName(wxT("BrowsePrinterLog"));
	m_printerLogFile = (wxTextCtrl *)FindWindowByName(wxT("PrinterLogFile"));
	m_printerportSelector = (wxComboBox*)FindWindowByName(wxT("PrinterportSelector"));

	m_defaultMaxFrameSkipButton = (wxButton *)FindWindowByName (wxT("DefaultMaxFrameSkipButton"));
	m_defaultMinFrameSkipButton = (wxButton *)FindWindowByName (wxT("DefaultMinFrameSkipButton"));

	m_speedSlider->SetTickFreq (25,1);
	m_maxFrameSkipSlider->SetTickFreq (5,1);
	m_minFrameSkipSlider->SetTickFreq (5,1);
	m_renshaTurboSlider->SetTickFreq (5,1);

	m_printerportFileLabel = (wxStaticText *)FindWindowByName (wxT("PrinterLogFileLabel"));
	m_printerportLabel = (wxStaticText *)FindWindowByName(wxT("PrinterLabel"));
	m_renshaLabel = (wxStaticText *)FindWindowByName(wxT("RenShaTurboLabel"));
	m_frameskipLabel = (wxStaticText *)FindWindowByName(wxT("FrameSkipLabel"));
	m_frameskipMaxLabel = (wxStaticText *)FindWindowByName(wxT("FrameSkipMaxLabel"));
	m_frameskipMinLabel = (wxStaticText *)FindWindowByName(wxT("FrameSkipMinLabel"));
	m_emulationSpeedLabel = (wxStaticText *)FindWindowByName(wxT("EmulationSpeedLabel"));



	wxComboBox* box = (wxComboBox *)FindWindowByName (wxT("PrinterportSelector"));
	box->Clear();
	box->Append(wxT("--empty--"));
	box->Append(wxT("logger"));
	box->Append(wxT("simpl"));
	box->Append(wxT("msx-printer"));
	box->Append(wxT("epson-printer"));

	/* This doesn't do anything, because it is "disabled" according to a comment in ConfigurationData
	wxString filename;
	wxTextCtrl * text = (wxTextCtrl *)FindWindowByName(wxT("PrinterLogFile"));
	wxString current;
	int pos;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_PRINTERPORT,current);
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_PRINTERFILE,filename);
	text->SetValue(filename);

	pos = box->FindString(current);
	if (pos != -1){
			box->SetSelection(pos);
		}
		else{
			box->SetSelection(0);
		}
	OnPrinterportChanged(false); */
}

MiscControlPage::~MiscControlPage()
{
}

void MiscControlPage::FillInitialJoystickPortValues ()
{
//  temporary hardcoded joystick port devices (not for BSD)
	wxString current;
	int pos;

#if wxUSE_JOYSTICK
	wxJoystick joy(wxJOYSTICK1);
	wxString temp;
#endif
	int JoySaveID[]={ConfigurationData::CD_JOYPORT1, ConfigurationData::CD_JOYPORT2};
	wxComboBox * box[2];
	box[0] = (wxComboBox *)FindWindowByName (wxT("Joyport1Selector"));
	box[1] = (wxComboBox *)FindWindowByName (wxT("Joyport2Selector"));
	for (int i=0;i<2;i++) {
		box[i]->Clear();
		box[i]->Append(wxT("--empty--"));
		box[i]->Append(wxT("mouse"));
		box[i]->Append(wxT("tetris2-protection"));
		box[i]->Append(wxT("magic-key"));
		box[i]->Append(wxT("keyjoystick1"));
		box[i]->Append(wxT("keyjoystick2"));

#if wxUSE_JOYSTICK
		for (int j=1;j<=joy.GetNumberJoysticks();j++) {
			temp.sprintf(wxT("joystick%d"),j);
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
}

void MiscControlPage::OnReset(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("reset"));
}

void MiscControlPage::OnPause(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("toggle pause"));
}

void MiscControlPage::OnPower(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("toggle power"));
	if (m_powerButton->GetValue())
	{
		m_controller->WriteCommand (wxT("set pause off"));
		m_pauseButton->SetValue(false);
	}
}

void MiscControlPage::OnFirmware(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxString(wxT("toggle ")) + m_firmwareSetting);
}

void MiscControlPage::OnSpeedChange(wxScrollEvent &event)
{
	wxString speedText;
	speedText.sprintf(wxT("%ld"), event.GetInt());
	m_speedIndicator->SetValue(speedText);
	m_controller->WriteCommand (wxString(wxT("set speed ")) + speedText);
	m_controller->WriteCommand (wxT("set throttle on"));
}

void MiscControlPage::OnSetNormalSpeed(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("set speed 100"));
	m_speedIndicator->SetValue(wxT("100"));
	m_speedSlider->SetValue (100);
	m_controller->WriteCommand (wxT("set throttle on"));
}

void MiscControlPage::OnSetMaxSpeed(wxCommandEvent &event)
{
	if (m_speedMaxButton->GetValue()){
		m_speedSlider->Disable();
		m_speedIndicator->Disable();
		m_speedNormalButton->Disable();
		m_controller->WriteCommand (wxT("set throttle off"));
	}
	else{
			m_speedSlider->Enable();
			m_speedIndicator->Enable();
			m_speedNormalButton->Enable();
			m_controller->WriteCommand (wxT("set throttle on"));
		}
}

void MiscControlPage::OnMaxFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf(wxT("%ld"), event.GetInt());
	m_maxFrameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString(wxT("set maxframeskip ") + skipText));
}

void MiscControlPage::OnMinFrameSkipChange(wxScrollEvent &event)
{
	wxString skipText;
	skipText.sprintf(wxT("%ld"), event.GetInt());
	m_minFrameSkipIndicator->SetValue(skipText);
	m_controller->WriteCommand (wxString(wxT("set minframeskip ") + skipText));
}

void MiscControlPage::OnSetDefaultMaxFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("unset maxframeskip"));
	m_maxFrameSkipIndicator->Enable();
	m_maxFrameSkipSlider->Enable();
}

void MiscControlPage::OnSetDefaultMinFrameSkip(wxCommandEvent &event)
{
	m_controller->WriteCommand (wxT("unset minframeskip"));
	m_minFrameSkipIndicator->Enable();
	m_minFrameSkipSlider->Enable();
}

void MiscControlPage::OnSetThrottle(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.GetEventObject();
	if (button->GetValue())
	{
		m_controller->WriteCommand (wxT("set throttle on"));
		button->SetLabel(wxT("On"));
	}
	else
	{
		m_controller->WriteCommand (wxT("set throttle off"));
		button->SetLabel(wxT("Off"));
	}
}

void MiscControlPage::OnSetCmdTiming(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.GetEventObject();
	if (button->GetValue())
	{
		m_controller->WriteCommand (wxT("set cmdtiming broken"));
		button->SetLabel(wxT("Broken"));
	}
	else
	{
		m_controller->WriteCommand (wxT("set cmdtiming real"));
		button->SetLabel(wxT("Real"));
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
	m_printerportSelector->Enable(true);
	m_pauseButton->SetValue(false);
	m_firmwareButton->SetValue(false);
	m_printerLogFile->Enable(true);
	m_browsePrinterLog->Enable(true);
	m_printerportFileLabel->Enable(true);
	m_printerportLabel->Enable(true);
	m_frameskipLabel->Enable(true);
	m_frameskipMaxLabel->Enable(true);
	m_frameskipMinLabel->Enable(true);
	m_emulationSpeedLabel->Enable(true);
	wxWindow * temp = FindWindowByLabel(wxT("Performance Controls"));
	if (temp){
		temp->Enable(true);
	}
	temp = FindWindowByLabel(wxT("Action Controls"));
	if (temp){
		temp->Enable(true);
	}

}

void MiscControlPage::EnableFirmware(wxString setting)
{
	m_firmwareSetting = setting;
	m_firmwareButton->Enable(true);
}

void MiscControlPage::EnableRenShaTurbo()
{
	m_renshaTurboSlider->Enable(true);
	m_renshaLabel->Enable(true);
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
	m_printerLogFile->Enable(false);
	m_browsePrinterLog->Enable(false);
	m_printerportSelector->Enable(false);
	m_printerportFileLabel->Enable(false);
	m_printerportLabel->Enable(false);
	m_renshaLabel->Enable(false);
	m_frameskipLabel->Enable(false);
	m_frameskipMaxLabel->Enable(false);
	m_frameskipMinLabel->Enable(false);
	m_emulationSpeedLabel->Enable(false);
	wxWindow * temp = FindWindowByLabel(wxT("Performance Controls"));
	if (temp){
		temp->Enable(false);
	}
	temp = FindWindowByLabel(wxT("Action Controls"));
	if (temp){
		temp->Enable(false);
	}
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
			m_controller->WriteCommand (wxString(wxT("set speed ")) + text);
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
			m_controller->WriteCommand (wxString(wxT("set minframeskip ") + text));
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
			m_controller->WriteCommand (wxString(wxT("set maxframeskip ") + text));
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
				InitJoystickPort (connectors[i],wxT("Joyport1Selector"), currentClass);
			} else if (connectors[i] == wxT("joyportb")) {
				InitJoystickPort (connectors[i],wxT("Joyport2Selector"), currentClass);
//			} else if (connectors[i] == wxT("printerport")){
//				InitJoystickPort (connectors[i],wxT("PrinterportSelector"), currentClass);
			}
		}
	}
//	wxTextCtrl * text = (wxTextCtrl *)FindWindowByName("PrinterLogFile");
//	m_controller->WriteCommand(wxString("set printerlogfilename ") + ConvertPath(text->GetValue()));
}

void MiscControlPage::InitJoystickPort (wxString connector, wxString control, wxString connectorClass)
{
	wxArrayString pluggables;
	wxArrayString classes;
	m_controller->GetPluggableClasses(classes);
	if (classes.GetCount() ==0) return;
	wxComboBox * box = (wxComboBox *)FindWindowByName (control);
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
	if (box->FindString(currentval) == wxNOT_FOUND )
	{
		currentval = box->GetString(0);
	}
	box->SetValue(currentval);
	wxString cmd = wxT("plug");
	if (currentval == wxT("--empty--")) {
		cmd = wxT("unplug");
		currentval = wxT("");
	}
	m_controller->WriteCommand(wxString(cmd +wxT(" ") + connector +wxT(" ") + currentval));
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

void MiscControlPage::OnJoystickChangedTimer(wxTimerEvent & event)
{
	OnJoystickChanged();
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
	wxString cmd = wxT("unplug ");
	wxString connector;
	wxString connector2;
	wxString value = wxT("");
	if (box->GetName() == wxT("Joyport1Selector")) {
		connector = wxT("joyporta ");
		connector2 = wxT("joyportb");
		box2 = (wxComboBox *)FindWindowByName (wxT("Joyport2Selector"));
		oldValue1 = &m_oldJoy1;
		oldValue2 = &m_oldJoy2;
	}
	else if (box->GetName() == wxT("Joyport2Selector")) {
		connector = wxT("joyportb ");
		connector2 = wxT("joyporta");
		box2 = (wxComboBox *)FindWindowByName (wxT("Joyport1Selector"));
		oldValue1 = &m_oldJoy2;
		oldValue2 = &m_oldJoy1;
	}

	if (box->GetValue() != wxT("--empty--")) {
		value = box->GetValue();
		cmd = wxT("plug ");
	}

	if ((box->GetValue() != (wxT("--empty--"))) && (box->GetValue() == box2->GetValue())) {
		MessageActive = true;
		int result = wxMessageBox (wxT("Unable to plug a device in more than one port!\n\n\
Do you still want to plug it into this port?\n\
This device will then be removed from any other port(s)."),wxT("Warning"),wxOK | wxCANCEL);
		MessageActive = false;
		if (result == wxOK) {
			box2->SetSelection(0);
			*oldValue2 = wxT("--empty--");
			*oldValue1 = box->GetValue();
			m_controller->WriteCommand(wxString(wxT("unplug ") + connector2));
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
	wxComboBox * joy1 = (wxComboBox *)FindWindowByName (wxT("Joyport1Selector"));
	wxComboBox * joy2 = (wxComboBox *)FindWindowByName (wxT("Joyport2Selector"));
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_JOYPORT1,joy1->GetValue());
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_JOYPORT2,joy2->GetValue());
	ConfigurationData::instance()->SaveData();

}

void MiscControlPage::OnChangeRenShaTurbo(wxScrollEvent & event)
{
	wxString value;
	value.sprintf (wxT("%ld"),event.GetInt());


	m_controller->WriteCommand(wxString (wxT("set renshaturbo ")) + value);
}

void MiscControlPage::OnChangePrinterPort(wxCommandEvent & event)
{
	OnPrinterportChanged(true);
}

void MiscControlPage::OnPrinterportChanged(bool save)
{
	wxString current = m_printerportSelector->GetValue();
	if (save){
		ConfigurationData::instance()->SetParameter(ConfigurationData::CD_PRINTERPORT,current);
		ConfigurationData::instance()->SaveData();
	}
	m_controller->WriteCommand(wxT("unplug printerport"));
	if (current != wxT("--empty--")){
		m_controller->WriteCommand(wxString(wxT("plug printerport ")) + current);

	}
	if (m_controller->IsOpenMSXRunning()){
		m_controller->UpdateMixer();
	}

}

void MiscControlPage::OnChangePrinterLogFile(wxCommandEvent &event)
{
	wxString current = ((wxTextCtrl *)event.GetEventObject())->GetValue();
	ConfigurationData::instance()->SetParameter(ConfigurationData::CD_PRINTERFILE,current);
	ConfigurationData::instance()->SaveData();
	m_controller->WriteCommand(wxString(wxT("set printerlogfilename ")) + ConvertPath(current,true));
}

void MiscControlPage::OnBrowsePrinterLogFile(wxCommandEvent &event)
{
	wxString path;
#ifndef __MOTIF__
	path = wxT("All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,wxT("Choose file to save printer log to"),wxT(""), wxT(""), path ,wxSAVE | wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK){
		m_printerLogFile->SetValue(filedlg.GetPath());
	}
}

void MiscControlPage::InvalidPrinterLogFilename()
{
	wxMessageBox (wxT("Unable to plug in printerport logger!\nPlease select a valid file name first."),wxT("Error"));
	m_printerportSelector->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug printerport"));
}
