#include "MiscControlPage.h"
#include "SessionPage.h"
#include "wxCatapultFrm.h"
#include "openMSXController.h"
#include "ConfigurationData.h"
#include "utils.h"
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/joystick.h>

IMPLEMENT_CLASS(MiscControlPage, wxPanel)
BEGIN_EVENT_TABLE(MiscControlPage, wxPanel)
	EVT_BUTTON        (XRCID("ResetButton"),              MiscControlPage::OnReset)
	EVT_TOGGLEBUTTON  (XRCID("PauseButton"),              MiscControlPage::OnPause)
	EVT_TOGGLEBUTTON  (XRCID("PowerButton"),              MiscControlPage::OnPower)
	EVT_TOGGLEBUTTON  (XRCID("FirmwareButton"),           MiscControlPage::OnFirmware)
	EVT_COMMAND_SCROLL(XRCID("SpeedSlider"),              MiscControlPage::OnSpeedChange)
	EVT_BUTTON        (XRCID("NormalSpeedButton"),        MiscControlPage::OnSetNormalSpeed)
	EVT_TOGGLEBUTTON  (XRCID("FastForwardButton"),        MiscControlPage::OnSetFastForward)
	EVT_COMMAND_SCROLL(XRCID("FastForwardSpeedSlider"),   MiscControlPage::OnFastForwardSpeedChange)
	EVT_TOGGLEBUTTON  (XRCID("FastLoadingButton"),        MiscControlPage::OnSetFastLoading)
	EVT_COMMAND_SCROLL(XRCID("MaxFrameSkipSlider"),       MiscControlPage::OnMaxFrameSkipChange)
	EVT_BUTTON        (XRCID("DefaultFastForwardSpeedButton"),MiscControlPage::OnSetDefaultFastForwardSpeed)
	EVT_BUTTON        (XRCID("DefaultMaxFrameSkipButton"),MiscControlPage::OnSetDefaultMaxFrameSkip)
	EVT_TOGGLEBUTTON  (XRCID("CmdTimingButton"),          MiscControlPage::OnSetCmdTiming)
	EVT_TEXT          (XRCID("FastForwardSpeedIndicator"), MiscControlPage::OnInputFastForwardSpeed)
	EVT_TEXT          (XRCID("MaxFrameSkipIndicator"),    MiscControlPage::OnInputMaxFrameskip)
	EVT_TEXT          (XRCID("SpeedIndicator"),           MiscControlPage::OnInputSpeed)
	EVT_TEXT          (XRCID("Joyport1Selector"),         MiscControlPage::OnChangeJoystick)
	EVT_TEXT          (XRCID("Joyport2Selector"),         MiscControlPage::OnChangeJoystick)
	EVT_TEXT          (XRCID("PrinterportSelector"),      MiscControlPage::OnChangePrinterPort)
	EVT_TEXT          (XRCID("PrinterLogFile"),           MiscControlPage::OnChangePrinterLogFile)
	EVT_BUTTON        (XRCID("BrowsePrinterLog"),         MiscControlPage::OnBrowsePrinterLogFile)
	EVT_COMMAND_SCROLL(XRCID("RenshaTurboSlider"),        MiscControlPage::OnChangeRenShaTurbo)
#ifdef __UNIX__
	EVT_TIMER(-1, MiscControlPage::OnJoystickChangedTimer)
#endif
END_EVENT_TABLE()

MiscControlPage::MiscControlPage(wxWindow* parent, openMSXController& controller)
	: m_controller(controller)
{
#ifdef __UNIX__
	m_joystick_update_timer.SetOwner(this, -1);
#endif
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("MiscControlPage"));
	m_powerButton = (wxToggleButton*)FindWindowByName(wxT("PowerButton"));
	m_pauseButton = (wxToggleButton*)FindWindowByName(wxT("PauseButton"));
	m_resetButton = (wxButton*)FindWindowByName(wxT("ResetButton"));
	m_firmwareButton = (wxToggleButton*)FindWindowByName(wxT("FirmwareButton"));
	m_speedIndicator = (wxTextCtrl*)FindWindowByName(wxT("SpeedIndicator"));
	m_speedSlider = (wxSlider*)FindWindowByName(wxT("SpeedSlider"));
	m_speedNormalButton = (wxButton*)FindWindowByName (wxT("NormalSpeedButton"));
	m_fastForwardButton = (wxToggleButton*)FindWindowByName (wxT("FastForwardButton"));
	m_fastForwardSpeedIndicator = (wxTextCtrl*)FindWindowByName(wxT("FastForwardSpeedIndicator"));
	m_fastLoadingButton = (wxToggleButton*)FindWindowByName (wxT("FastLoadingButton"));
	m_maxFrameSkipIndicator = (wxTextCtrl*)FindWindowByName(wxT("MaxFrameSkipIndicator"));
	m_maxFrameSkipSlider = (wxSlider*)FindWindowByName(wxT("MaxFrameSkipSlider"));
	m_fastForwardSpeedSlider = (wxSlider*)FindWindowByName(wxT("FastForwardSpeedSlider"));
	m_renshaTurboSlider  = (wxSlider*)FindWindowByName(wxT("RenshaTurboSlider"));
	m_browsePrinterLog = (wxBitmapButton*)FindWindowByName(wxT("BrowsePrinterLog"));
	m_printerLogFile = (wxTextCtrl*)FindWindowByName(wxT("PrinterLogFile"));
	m_printerportSelector = (wxComboBox*)FindWindowByName(wxT("PrinterportSelector"));

	m_defaultMaxFrameSkipButton = (wxButton*)FindWindowByName(wxT("DefaultMaxFrameSkipButton"));
	m_defaultFastForwardSpeedButton = (wxButton*)FindWindowByName(wxT("DefaultFastForwardSpeedButton"));

	m_speedSlider->SetTickFreq(25);
	m_maxFrameSkipSlider->SetTickFreq(5);
	m_fastForwardSpeedSlider->SetTickFreq(250);
	m_renshaTurboSlider->SetTickFreq(5);

	m_printerportFileLabel = (wxStaticText*)FindWindowByName(wxT("PrinterLogFileLabel"));
	m_printerportLabel     = (wxStaticText*)FindWindowByName(wxT("PrinterLabel"));
	m_renshaLabel          = (wxStaticText*)FindWindowByName(wxT("RenShaTurboLabel"));
	m_frameskipMaxLabel    = (wxStaticText*)FindWindowByName(wxT("FrameSkipMaxLabel"));
	m_fastForwardSpeedLabel= (wxStaticText*)FindWindowByName(wxT("FastForwardSpeedLabel"));
	m_emulationSpeedLabel  = (wxStaticText*)FindWindowByName(wxT("EmulationSpeedLabel"));

	auto* box = (wxComboBox*)FindWindowByName(wxT("PrinterportSelector"));
	box->Clear();
	box->Append(wxT("--empty--"));
	box->Append(wxT("logger"));
	box->Append(wxT("simpl"));
	box->Append(wxT("msx-printer"));
	box->Append(wxT("epson-printer"));

	/* This doesn't do anything, because it is "disabled" according to a comment in ConfigurationData
	wxString filename;
	auto* text = (wxTextCtrl*)FindWindowByName(wxT("PrinterLogFile"));
	wxString current;
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_PRINTERPORT, current);
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_PRINTERFILE, filename);
	text->SetValue(filename);

	int pos = box->FindString(current);
	if (pos != wxNOT_FOUND) {
		box->SetSelection(pos);
	} else {
		box->SetSelection(0);
	}
	OnPrinterportChanged(false); */
}

void MiscControlPage::FillInitialJoystickPortValues()
{
	//  temporary hardcoded joystick port devices (not for BSD)
#if wxUSE_JOYSTICK
	wxJoystick joy(wxJOYSTICK1);
#endif
	int JoySaveID[] = {
		ConfigurationData::CD_JOYPORT1,
		ConfigurationData::CD_JOYPORT2
	};
	wxComboBox* box[2];
	box[0] = (wxComboBox*)FindWindowByName(wxT("Joyport1Selector"));
	box[1] = (wxComboBox*)FindWindowByName(wxT("Joyport2Selector"));
	for (int i = 0; i < 2; ++i) {
		box[i]->Clear();
		box[i]->Append(wxT("--empty--"));
		box[i]->Append(wxT("mouse"));
		box[i]->Append(wxT("tetris2-protection"));
		box[i]->Append(wxT("magic-key"));
		box[i]->Append(wxT("keyjoystick1"));
		box[i]->Append(wxT("keyjoystick2"));

#if wxUSE_JOYSTICK
		for (int j = 1; j <= joy.GetNumberJoysticks(); ++j) {
			box[i]->Append(wxString::Format(wxT("joystick%d"), j));
		}
#endif
		wxString current;
		ConfigurationData::instance().GetParameter(JoySaveID[i], current);
		int pos = box[i]->FindString(current);
		if (pos != wxNOT_FOUND) {
			box[i]->SetSelection(pos);
		} else {
			box[i]->SetSelection(0);
		}
	}
	m_oldJoy1 = box[0]->GetValue();
	m_oldJoy2 = box[1]->GetValue();
}

void MiscControlPage::OnReset(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("reset"));
}

void MiscControlPage::OnPause(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("toggle pause"));
}

void MiscControlPage::OnPower(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("toggle power"));
	if (m_powerButton->GetValue()) {
		m_controller.WriteCommand(wxT("set pause off"));
		m_pauseButton->SetValue(false);
	}
}

void MiscControlPage::OnFirmware(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("toggle ") + m_firmwareSetting);
}

void MiscControlPage::OnSpeedChange(wxScrollEvent& event)
{
	auto speedText = wxString::Format(wxT("%d"), event.GetInt());
	m_speedIndicator->SetValue(speedText);
	m_controller.WriteCommand(wxT("set speed ") + speedText);
	m_controller.WriteCommand(wxT("set throttle on; set fastforward off"));
}

void MiscControlPage::OnSetNormalSpeed(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("set speed 100"));
	m_speedIndicator->SetValue(wxT("100"));
	m_speedSlider->SetValue(100);
	m_controller.WriteCommand(wxT("set throttle on; set fastforward off"));
}

void MiscControlPage::OnSetFastForward(wxCommandEvent& event)
{
	if (m_fastForwardButton->GetValue()) {
		m_speedSlider->Disable();
		m_speedIndicator->Disable();
		m_speedNormalButton->Disable();
		m_controller.WriteCommand(wxT("set fastforward on"));
	} else {
		m_speedSlider->Enable();
		m_speedIndicator->Enable();
		m_speedNormalButton->Enable();
		m_controller.WriteCommand(wxT("set fastforward off"));
	}
}

void MiscControlPage::OnSetFastLoading(wxCommandEvent& event)
{
	if (m_fastLoadingButton->GetValue()) {
		m_controller.WriteCommand(wxT("set fullspeedwhenloading on"));
	} else {
		m_controller.WriteCommand(wxT("set fullspeedwhenloading off"));
	}
}

void MiscControlPage::OnMaxFrameSkipChange(wxScrollEvent& event)
{
	auto skipText = wxString::Format(wxT("%d"), event.GetInt());
	m_maxFrameSkipIndicator->SetValue(skipText);
	m_controller.WriteCommand(wxT("set maxframeskip ") + skipText);
}

void MiscControlPage::OnFastForwardSpeedChange(wxScrollEvent& event)
{
	auto skipText = wxString::Format(wxT("%d"), event.GetInt());
	m_fastForwardSpeedIndicator->SetValue(skipText);
	m_controller.WriteCommand(wxT("set fastforwardspeed ") + skipText);
}

void MiscControlPage::OnSetDefaultMaxFrameSkip(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("unset maxframeskip"));
	m_maxFrameSkipIndicator->Enable();
	m_maxFrameSkipSlider->Enable();
}

void MiscControlPage::OnSetDefaultFastForwardSpeed(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("unset fastforwardspeed"));
	m_fastForwardSpeedIndicator->Enable();
	m_fastForwardSpeedSlider->Enable();
}

void MiscControlPage::OnSetCmdTiming(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	if (button->GetValue()) {
		m_controller.WriteCommand(wxT("set cmdtiming broken"));
		button->SetLabel(wxT("Broken"));
	} else {
		m_controller.WriteCommand(wxT("set cmdtiming real"));
		button->SetLabel(wxT("Real"));
	}
}

void MiscControlPage::SetControlsOnLaunch()
{
	m_speedSlider->Enable(true);
	m_speedIndicator->Enable(true);
	m_speedNormalButton->Enable(true);
	m_fastForwardButton->Enable(true);
	m_fastLoadingButton->Enable(true);

	m_fastForwardSpeedSlider->Enable(true);
	m_fastForwardSpeedIndicator->Enable(true);
	m_defaultFastForwardSpeedButton->Enable(true);

	m_maxFrameSkipSlider->Enable(true);
	m_maxFrameSkipIndicator->Enable(true);
	m_defaultMaxFrameSkipButton->Enable(true);

	m_powerButton->Enable(true);
	m_resetButton->Enable(true);
	m_pauseButton->Enable(true);
	m_pauseButton->SetValue(false);
	m_firmwareButton->SetValue(false);
	m_printerportSelector->Enable(true);
	m_printerLogFile->Enable(true);
	m_browsePrinterLog->Enable(true);
	m_printerportFileLabel->Enable(true);
	m_printerportLabel->Enable(true);
	m_frameskipMaxLabel->Enable(true);
	m_fastForwardSpeedLabel->Enable(true);
	m_emulationSpeedLabel->Enable(true);
	if (auto* temp = FindWindowByLabel(wxT("Performance Controls"))) {
		temp->Enable(true);
	}
	if (auto* temp = FindWindowByLabel(wxT("Action Controls"))) {
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
	m_fastForwardButton->Enable(false);
	m_fastLoadingButton->Enable(false);

	m_fastForwardSpeedSlider->Enable(false);
	m_fastForwardSpeedIndicator->Enable(false);
	m_defaultFastForwardSpeedButton->Enable(false);

	m_maxFrameSkipSlider->Enable(false);
	m_maxFrameSkipIndicator->Enable(false);
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
	m_frameskipMaxLabel->Enable(false);
	m_fastForwardSpeedLabel->Enable(false);
	m_emulationSpeedLabel->Enable(false);
	if (auto* temp = FindWindowByLabel(wxT("Performance Controls"))) {
		temp->Enable(false);
	}
	if (auto* temp = FindWindowByLabel(wxT("Action Controls"))) {
		temp->Enable(false);
	}
}

void MiscControlPage::OnInputSpeed(wxCommandEvent& event)
{
	wxString text = m_speedIndicator->GetValue();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 500) {
			num = 500;
			text = wxT("500");
			m_speedIndicator->SetValue(text);
		}
		if (num >= 1) {
			m_controller.WriteCommand(wxT("set speed ") + text);
			m_speedSlider->SetValue(num);
		}
	}
}

void MiscControlPage::OnInputFastForwardSpeed(wxCommandEvent& event)
{
	wxString text = m_fastForwardSpeedIndicator->GetValue();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 10000) {
			num = 10000;
			text = wxT("10000");
			m_fastForwardSpeedIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set fastforwardspeed ") + text);
		m_fastForwardSpeedSlider->SetValue(num);
	}
}

void MiscControlPage::OnInputMaxFrameskip(wxCommandEvent& event)
{
	wxString text = m_maxFrameSkipIndicator->GetValue ();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 100) {
			num = 100;
			text = wxT("100");
			m_maxFrameSkipIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set maxframeskip ") + text);
		m_maxFrameSkipSlider->SetValue(num);
	}
}

void MiscControlPage::InitConnectorPanel()
{
	// first disable them all, for cases where we don't have that whole connector present
	FindWindowByName(wxT("PrinterportSelector"))->Enable(false);
	FindWindowByName(wxT("Joyport1Selector"))->Enable(false);
	FindWindowByName(wxT("Joyport2Selector"))->Enable(false);
	for (auto& connector : m_controller.GetConnectors()) {
		wxString currentClass = m_controller.GetConnectorClass(connector);
		if (connector == wxT("joyporta")) {
			InitJoystickOrPrinterPort(connector, wxT("Joyport1Selector"), currentClass);
		} else if (connector == wxT("joyportb")) {
			InitJoystickOrPrinterPort(connector, wxT("Joyport2Selector"), currentClass);
		} else if (connector == wxT("printerport")) {
			InitJoystickOrPrinterPort(connector, wxT("PrinterportSelector"), currentClass);
		}
	}
}

void MiscControlPage::InitJoystickOrPrinterPort(wxString connector, wxString control, wxString connectorClass)
{
	const wxArrayString& classes = m_controller.GetPluggableClasses();
	if (classes.IsEmpty()) return;

	auto* box = (wxComboBox*)FindWindowByName(control);
	box->Enable(true);
	wxString currentval = box->GetValue();
	box->Clear();
	box->Append(wxT("--empty--"));

	const wxArrayString& pluggables = m_controller.GetPluggables();
	for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
		if (classes[i] == connectorClass) {
			box->Append(pluggables[i]);
		}
	}
	if (m_controller.IsRelaunching()) {
		// if relaunching, then just update the values
		wxString currentlyPlugged = m_controller.GetConnectorPlugged(connector);
		box->SetValue(currentlyPlugged.IsEmpty() ? wxT("--empty--") : currentlyPlugged);
	} else {
		// Initializing (initial launch)
		if (box->FindString(currentval) == wxNOT_FOUND) {
			currentval = box->GetString(0);
		}
		box->SetValue(currentval);
		if (currentval != wxT("--empty--")) {
			m_controller.WriteCommand(wxT("plug ") + connector + wxT(" ") + currentval);
		} else {
			m_controller.WriteCommand(wxT("unplug ") + connector);
		}
	}
}

void MiscControlPage::OnChangeJoystick(wxCommandEvent& event)
{
	m_lastUpdatedCombo = (wxComboBox*)event.GetEventObject();
#ifdef __UNIX__
	if (!m_joystick_update_timer.IsRunning()) {
		m_joystick_update_timer.Start(200, true);
	}
#else
	OnJoystickChanged();
#endif
}

void MiscControlPage::OnJoystickChangedTimer(wxTimerEvent& event)
{
	OnJoystickChanged();
}

void MiscControlPage::OnJoystickChanged()
{
	static bool MessageActive = false;
	wxComboBox* box = m_lastUpdatedCombo;
	if (!box || MessageActive) {
		return;
	}
	wxComboBox* box2 = nullptr;
	wxString* oldValue1 = nullptr; // this port
	wxString* oldValue2 = nullptr; // the other port
	wxString cmd = wxT("unplug ");
	wxString connector;
	wxString connector2;
	if (box->GetName() == wxT("Joyport1Selector")) {
		connector = wxT("joyporta ");
		connector2 = wxT("joyportb");
		box2 = (wxComboBox*)FindWindowByName(wxT("Joyport2Selector"));
		oldValue1 = &m_oldJoy1;
		oldValue2 = &m_oldJoy2;
	} else if (box->GetName() == wxT("Joyport2Selector")) {
		connector = wxT("joyportb ");
		connector2 = wxT("joyporta");
		box2 = (wxComboBox*)FindWindowByName(wxT("Joyport1Selector"));
		oldValue1 = &m_oldJoy2;
		oldValue2 = &m_oldJoy1;
	}

	wxString value;
	if (box->GetValue() != wxT("--empty--")) {
		value = box->GetValue();
		cmd = wxT("plug ");
	}

	bool sameDevicePluggedInTwoPorts = false;
	if (box->GetValue() != wxT("--empty--")) {
		assert(box2 != 0);
		if (!box2) throw "box2 must not be null, but it is null.";

		if (box->GetValue() == box2->GetValue()) {
			sameDevicePluggedInTwoPorts = true;
			assert(oldValue1 != 0);
			if (!oldValue1) throw "oldValue1 must not be null, but it is null.";
			MessageActive = true;
			int result = wxMessageBox(wxT("Unable to plug a device in more than one port!\n\nDo you still want to plug it into this port?\nThis device will then be removed from any other port(s)."),
				wxT("Warning"),
				wxOK | wxCANCEL);
			MessageActive = false;
			if (result == wxOK) {
				assert(oldValue2 != 0);
				if (!oldValue2) throw "oldValue2 must not be null, but it is null.";

				box2->SetSelection(0);
				*oldValue2 = wxT("--empty--");
				*oldValue1 = box->GetValue();
				m_controller.WriteCommand(wxT("unplug ") + connector2);
				m_controller.WriteCommand(cmd + connector + value);
			} else {
				// cancel
				box->SetSelection(box->FindString(*oldValue1));
			}
		}
	}
	if (!sameDevicePluggedInTwoPorts) {
		// no collision
		m_controller.WriteCommand(cmd + connector + value);
		assert(oldValue1!=0);
		if(!oldValue1)throw "oldValue1 must not be null, but it is null.";
		*oldValue1 = box->GetValue();
	}
	auto* joy1 = (wxComboBox*)FindWindowByName(wxT("Joyport1Selector"));
	auto* joy2 = (wxComboBox*)FindWindowByName(wxT("Joyport2Selector"));
	ConfigurationData::instance().SetParameter(ConfigurationData::CD_JOYPORT1, joy1->GetValue());
	ConfigurationData::instance().SetParameter(ConfigurationData::CD_JOYPORT2, joy2->GetValue());
	ConfigurationData::instance().SaveData();
}

void MiscControlPage::OnChangeRenShaTurbo(wxScrollEvent& event)
{
	m_controller.WriteCommand(wxString::Format(
		wxT("set renshaturbo %d"), event.GetInt()));
}

void MiscControlPage::OnChangePrinterPort(wxCommandEvent& event)
{
	OnPrinterportChanged(true);
}

void MiscControlPage::OnPrinterportChanged(bool save)
{
	wxString current = m_printerportSelector->GetValue();
	if (save) {
		ConfigurationData::instance().SetParameter(ConfigurationData::CD_PRINTERPORT, current);
		ConfigurationData::instance().SaveData();
	}
	m_controller.WriteCommand(wxT("unplug printerport"));
	if (current != wxT("--empty--")) {
		m_controller.WriteCommand(wxT("plug printerport ") + current,
			nullptr,
			[&](const wxString& c, const wxString&) {
				if (c == wxT("plug printerport logger")) {
					InvalidPrinterLogFilename();
				}
			});
	}
	if (m_controller.IsOpenMSXRunning()) {
		m_controller.UpdateMixer();
	}
}

void MiscControlPage::OnChangePrinterLogFile(wxCommandEvent& event)
{
	wxString current = ((wxTextCtrl*)event.GetEventObject())->GetValue();
	ConfigurationData::instance().SetParameter(ConfigurationData::CD_PRINTERFILE, current);
	ConfigurationData::instance().SaveData();
	m_controller.WriteCommand(wxT("set printerlogfilename ") + utils::ConvertPathForCommand(current));
}

void MiscControlPage::OnBrowsePrinterLogFile(wxCommandEvent& event)
{
	wxString path = wxT("All files|*.*||");
	wxFileDialog filedlg(this, wxT("Choose file to save printer log to"), wxT(""), wxT(""), path, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK) {
		m_printerLogFile->SetValue(filedlg.GetPath());
	}
}

void MiscControlPage::InvalidPrinterLogFilename()
{
	wxMessageBox(wxT("Unable to plug in printerport logger!\nPlease select a valid file name first."), wxT("Error"));
	m_printerportSelector->SetValue(wxT("--empty--"));
	m_controller.WriteCommand(wxT("unplug printerport"));
}
