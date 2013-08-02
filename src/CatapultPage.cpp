#include "CatapultPage.h"
#include "AudioControlPage.h"
#include "VideoControlPage.h"
#include "ConfigurationData.h"
#include <wx/msgdlg.h>
#include <wx/combobox.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/slider.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

static const int S_CONVERT = 1;
static const int S_EVENT   = 2;
static const int S_INVERT  = 4;

CatapultPage::CatapultPage(wxWindow* parent)
	: m_parent(parent)
{
	AddSetting(wxT("renderer"), wxT("RendererSelector"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("scale_algorithm"), wxT("ScalerAlgoSelector"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("scale_factor"), wxT("ScalerFactorSelector"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("accuracy"), wxT("AccuracySelector"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("deinterlace"), wxT("DeInterlaceButton"), &CatapultPage::UpdateToggle, S_CONVERT);
	AddSetting(wxT("limitsprites"), wxT("LimitSpriteButton"), &CatapultPage::UpdateToggle, S_CONVERT);
	AddSetting(wxT("blur"), wxT("BlurIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("glow"), wxT("GlowIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("gamma"), wxT("GammaIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("scanline"), wxT("ScanlineIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("speed"), wxT("SpeedIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("minframeskip"), wxT("MinFrameSkipIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("maxframeskip"), wxT("MaxFrameSkipIndicator"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("throttle"), wxT("MaxSpeedButton"), &CatapultPage::UpdateToggle, S_INVERT | S_EVENT);
	AddSetting(wxT("power"), wxT("PowerButton"), &CatapultPage::UpdateToggle);
	AddSetting(wxT("pause"), wxT("PauseButton"), &CatapultPage::UpdateToggle);
	AddSetting(wxT("frontswitch"), wxT("FirmwareButton"), &CatapultPage::UpdateToggle);
	AddSetting(wxT("firmwareswitch"), wxT("FirmwareButton"), &CatapultPage::UpdateToggle);
	AddSetting(wxT("mute"), wxT("MuteButton"), &CatapultPage::UpdateToggle);
	AddSetting(wxT("midi-in-readfilename"), wxT("MidiInFileInput"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("midi-out-logfilename"), wxT("MidiOutFileInput"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("audio-inputfilename"), wxT("SampleFileInput"), &CatapultPage::UpdateIndicator);
	AddSetting(wxT("*_volume"), wxT("volume"), &CatapultPage::UpdateAudio);
	AddSetting(wxT("*_balance"), wxT("balance"), &CatapultPage::UpdateAudio);
	AddSetting(wxT("msx-midi-in"), wxT("MidiInSelector"), &CatapultPage::UpdateMidiPlug);
	AddSetting(wxT("msx-midi-out"), wxT("MidiOutSelector"), &CatapultPage::UpdateMidiPlug);
	AddSetting(wxT("pcminput"), wxT("SampleInSelector"), &CatapultPage::UpdatePluggable);
	AddSetting(wxT("joyporta"), wxT("Joyport1Selector"), &CatapultPage::UpdatePluggable);
	AddSetting(wxT("joyportb"), wxT("Joyport2Selector"), &CatapultPage::UpdatePluggable);
	AddSetting(wxT("printerport"), wxT("PrinterportSelector"), &CatapultPage::UpdatePluggable);
	AddSetting(wxT("renshaturbo"), wxT("RenshaTurboSlider"), &CatapultPage::UpdateSlider);
	AddSetting(wxT("diska"), wxT("DiskAContents"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("diskb"), wxT("DiskBContents"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("cassetteplayer"), wxT("CassetteContents"), &CatapultPage::UpdateCombo);
	AddSetting(wxT("fullscreen"), wxT("FullScreenButton"), &CatapultPage::UpdateToggle, S_CONVERT);
	AddSetting(wxT("save_settings_on_exit"), wxT("Save Settings On Exit"), &CatapultPage::UpdateMenu);
	AddSetting(wxT("printerlogfilename"), wxT("PrinterLogFile"),&CatapultPage::UpdateIndicator, S_CONVERT);
}

void CatapultPage::AddSetting(
	const wxString& setting, const wxString& control,
	void (CatapultPage::*pfunction)(const wxString&, const wxString&, const wxString&, int),
	int flags)
{
	SettingElement elem;
	elem.setting   = setting;
	elem.control   = control;
	elem.pfunction = pfunction;
	elem.flags     = flags;
	m_settingTable.push_back(elem);
}

wxString CatapultPage::ConvertPath(wxString path, bool convertSlash, bool backwards)
{
	if (backwards) {
		path.Replace(wxT("\""), wxT(""), true);
	} else {
		path.Prepend(wxT("\""));
		path.Append(wxT("\""));
	}
	if (convertSlash) {
		if (backwards){
			path.Replace(wxT("/"), wxT("\\"), true);
		} else {
			path.Replace(wxT("\\"), wxT("/"), true);
		}
	}
	return path;
}

void CatapultPage::OnClickCombo(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString sel = box->GetString(box->GetSelection());
	if (sel != box->GetValue()) {
		box->SetValue(sel);
	}
}

void CatapultPage::UpdateSetting(const wxString& name_, const wxString& data)
{
	wxString name = name_; // TODO HACK: need a proper Tcl parser
	name.Replace(wxT("\\"), wxT(""), true);

	for (auto& elem : m_settingTable) {
		if (name.Matches(elem.setting.c_str())) {
			(*this.*(elem.pfunction))(
				name, data, elem.control, elem.flags);
			break;
		}
	}
}

static bool isTclTrue(const wxString& str)
{
	return (str == wxT("on"))   ||
	       (str == wxT("true")) ||
	       (str == wxT("1"))    ||
	       (str == wxT("yes"));
}

void CatapultPage::UpdateToggle(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* button = (wxToggleButton*)m_parent->FindWindowByName(control)) {
		bool active = !(flags & S_INVERT);
		bool sendvalue = isTclTrue(data) ? active : !active;
		button->SetValue(sendvalue);

		if (flags & S_EVENT) {
			wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, button->GetId());
			event.SetInt(active);
			event.SetEventObject(button);
			button->ProcessEvent(event);
		}

		if (flags & S_CONVERT) {
			button->SetLabel(isTclTrue(data) ? wxT("On") : wxT("Off"));
		}
	}
}

void CatapultPage::UpdateCombo(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	wxString valuetext = data;
	if (flags & S_CONVERT) {
		valuetext = data.Mid(0, 1).Upper() + data.Mid(1).Lower();
	}
	if (auto* box = (wxComboBox*)m_parent->FindWindowByName(control)) {
		if (box->GetWindowStyle() & wxCB_READONLY) {
			box->SetSelection(box->FindString(valuetext));
		} else {
			box->SetValue(valuetext);
		}
	}
}

void CatapultPage::UpdateIndicator(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* indicator = (wxTextCtrl*)m_parent->FindWindowByName(control)) {
		wxString tempData = data;
		if (flags & S_CONVERT) {
			tempData = ConvertPath(data, true, true);
		}
		if (indicator->GetValue() != tempData) {
			indicator->SetValue(tempData);
		}
	}
}

void CatapultPage::UpdateSlider(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	if (auto* slider = (wxSlider*)m_parent->FindWindowByName(control)) {
		long value;
		data.ToLong(&value, 10);
		if (slider->GetValue() != value) {
			slider->SetValue(value);
		}
	}
}

void CatapultPage::UpdateMenu(const wxString& setting, const wxString& data, const wxString& control, int flags)
{
	auto* appwnd = (wxFrame*)GetParent()->GetGrandParent();
	int menusetting = appwnd->GetMenuBar()->FindMenuItem(wxT("&Settings"), wxT("Save openMSX Settings On &Exit"));
	if (menusetting != wxNOT_FOUND) {
		appwnd->GetMenuBar()->Check(menusetting, isTclTrue(data));
	}
}

void CatapultPage::UpdateAudio(const wxString& setting, const wxString& data, const wxString& selection, int flags)
{
	auto* notebook = (wxNotebook*)m_parent;
	AudioControlPage* audiopage = nullptr;
	for (unsigned i = 0; i < notebook->GetPageCount(); ++i) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage*)notebook->GetPage(i);
			break;
		}
	}
	assert(audiopage);

	wxString channel = setting.Mid(0, setting.Length() - selection.Length() - 1);
	if (selection == wxT("volume")) {
		audiopage->SetChannelVolume(channel, data);
	} else {
		assert(selection == wxT("balance"));
		audiopage->SetChannelMode(channel, data);
	}
}

void CatapultPage::UpdateMidiPlug(const wxString& connector, const wxString& data, const wxString& control, int flags)
{
	auto* notebook = (wxNotebook*)m_parent;
	AudioControlPage* audiopage = nullptr;
	for (unsigned int i = 0; i < notebook->GetPageCount(); ++i) {
		if (notebook->GetPageText(i) == wxT("Audio Controls")) {
			audiopage = (AudioControlPage*)notebook->GetPage(i);
			break;
		}
	}
	assert(audiopage);
	audiopage->UpdateMidiPlug(control, data);
}

void CatapultPage::UpdatePluggable(const wxString& connector, const wxString& data, const wxString& control, int flags)
{
	if (auto* box = (wxComboBox*)m_parent->FindWindowByName(control)) {
		box->SetValue(data.IsEmpty() ? wxT("--empty--") : data);
	}
}
