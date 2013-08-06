#include "AudioControlPage.h"
#include "openMSXController.h"
#include "StatusPage.h"
#include "wxCatapultFrm.h"
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <algorithm>
#include <map>

#define MUTEBUTTONID 19999
#define FIRSTAUDIOSLIDER 20000
#define FIRSTAUDIOCOMBO 21000

IMPLEMENT_CLASS(AudioControlPage, wxPanel)
BEGIN_EVENT_TABLE(AudioControlPage, wxPanel)
	EVT_COMBOBOX(XRCID("MidiInSelector"),          CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("MidiOutSelector"),         CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("SampleInSelector"),        CatapultPage::OnClickCombo)
	EVT_TEXT    (XRCID("MidiInSelector"),          AudioControlPage::OnChangeMidiInPlug)
	EVT_TEXT    (XRCID("MidiOutSelector"),         AudioControlPage::OnChangeMidiOutPlug)
	EVT_TEXT    (XRCID("SampleInSelector"),        AudioControlPage::OnChangeSampleInPlug)
	EVT_BUTTON  (XRCID("BrowseMidiInButton"),      AudioControlPage::OnBrowseMidiInFile)
	EVT_BUTTON  (XRCID("BrowseMidiOutButton"),     AudioControlPage::OnBrowseMidiOutFile)
	EVT_BUTTON  (XRCID("BrowseSampleInputButton"), AudioControlPage::OnBrowseSampleInFile)
END_EVENT_TABLE()

AudioControlPage::AudioControlPage(wxWindow* parent, openMSXController& controller)
	: m_controller(controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("AudioControlPage"));
	m_audioPanel = (wxPanel*)FindWindowByName (wxT("AudioChannelPanel"));
	m_browseMidiInButton      = (wxBitmapButton*)FindWindowByName(wxT("BrowseMidiInButton"));
	m_browseMidiOutButton     = (wxBitmapButton*)FindWindowByName(wxT("BrowseMidiOutButton"));
	m_browseSampleInputButton = (wxBitmapButton*)FindWindowByName(wxT("BrowseSampleInputButton"));
	m_midiInLabel       = (wxStaticText*)FindWindowByName(wxT("MidiInLabel"));
	m_midiOutLabel      = (wxStaticText*)FindWindowByName(wxT("MidiOutLabel"));
	m_sampleInLabel     = (wxStaticText*)FindWindowByName(wxT("SoundInLabel"));
	m_midiInFileLabel   = (wxStaticText*)FindWindowByName(wxT("MidiInFileLabel"));
	m_midiOutFileLabel  = (wxStaticText*)FindWindowByName(wxT("MidiOutFileLabel"));
	m_sampleInFileLabel = (wxStaticText*)FindWindowByName(wxT("SampleInFileLabel"));
	if (auto* temp = FindWindowByLabel(wxT("Mixer"))) {
		temp->Enable(false);
	}
	if (auto* temp = FindWindowByLabel(wxT("Sound I/O"))) {
		temp->Enable(false);
	}
}

static wxString abbreviateType(const wxString& type)
{
	static bool init = false;
	static std::map<wxString, wxString> m;
	if (!init) {
		init = true;
		m[wxT("master")] = wxT("a master\n");
		m[wxT("1-bit click generator")] = wxT("b key-\nclick");
		m[wxT("PSG")] = wxT("c PSG\n");
		m[wxT("Turbo-R PCM")] = wxT("d PCM\n");
		m[wxT("MSX-MUSIC")] = wxT("e MSX-\nMUSIC");
		m[wxT("Konami SCC")] = wxT("g SCC\n");
		m[wxT("Konami SCC+")] = wxT("h SCC+\n");
		m[wxT("MSX-AUDIO")] = wxT("i AUDIO\nFM");
		m[wxT("MSX-AUDIO 13-bit DAC")] = wxT("j AUDIO\nDAC");
		m[wxT("MSX-AUDIO 8-bit DAC")] = wxT("k AUDIO\nDAC2");
		m[wxT("MoonSound FM-part")] = wxT("l MSnd\nFM");
		m[wxT("MoonSound wave-part")] = wxT("m MSnd\nWave");
		m[wxT("Hai no Majutsushi's DAC")] = wxT("o Majutsu\nDAC");
		m[wxT("Konami Synthesizer's DAC")] = wxT("p Konami\nSynth");
		m[wxT("Play samples via your printer port.")] = wxT("q SIMPL\n");
		m[wxT("Sony Playball's DAC")] = wxT("r Playball\n");
		m[wxT("Konami Keyboard Master's VLM5030")] = wxT("s Konami\nVLM5030");
		m[wxT("Cassetteplayer, use to read .cas or .wav files.")] = wxT("y cas-\nsette");
	}
	auto it = m.find(type);
	return (it != m.end()) ? it->second          //   known type
	                       : (wxT("z ") + type); // unknown type
}

void AudioControlPage::InitAudioChannels()
{
	m_audioChannels.clear();
	AddChannelType(wxT("master"), wxT("master"));
}

void AudioControlPage::AddChannelType(const wxString& name, const wxString& type_)
{
	// Hack: undo transformation from stupid launch script
	wxString type = type_;
	type.Replace(wxT("\n"), wxT(" "));

	ChannelInfo info;
	info.name        = name;
	info.type        = type;
	info.displayType = abbreviateType(type);
	m_audioChannels.push_back(info);
}

void AudioControlPage::SetupAudioMixer()
{
	if (auto* temp = FindWindowByLabel(wxT("Mixer"))) {
		temp->Enable(true);
	}
	auto* audioSizer = m_audioPanel->GetSizer();
	auto* noAudio = (wxStaticText*)FindWindowByName(wxT("NoAudioText"));
	audioSizer->Remove(0);
	delete noAudio;

	if (auto* mixerSizer = (wxStaticBoxSizer*)FindWindowByName(wxT("SoundSizer"))) {
		if (auto* staticBox = mixerSizer->GetStaticBox()) {
			wxSize size = staticBox->GetClientSize();
			m_audioPanel->SetClientSize(size);
			audioSizer->SetMinSize(size);
		}
	}

	sort(m_audioChannels.begin(), m_audioChannels.end(),
		[](const ChannelInfo& lhs, const ChannelInfo& rhs) {
			return lhs.displayType < rhs.displayType;
		});

	for (size_t i = 0; i < m_audioChannels.size(); ++i) {
		AddChannel(i);
	}
	audioSizer->Layout();
}

void AudioControlPage::DestroyAudioMixer()
{
	if (auto* temp = FindWindowByLabel(wxT("Mixer"))) {
		temp->Enable(false);
	}
	if (!m_audioChannels.empty()) {
		auto* audioSizer = m_audioPanel->GetSizer();
		for (size_t i = m_audioChannels.size(); i > 0; --i) {
			audioSizer->Remove(i - 1);
			auto number = wxString::Format(wxT("%u"), i - 1);
			delete FindWindowByName(wxT("AudioLabel_")  + number);
			delete FindWindowByName(wxT("AudioSlider_") + number);
			delete FindWindowByName(wxT("AudioMode_")   + number);
		}
		delete FindWindowByName(wxT("MuteButton"));

		auto* noAudio = new wxStaticText(
			m_audioPanel, -1, wxT("No audio channel data available"),
			wxDefaultPosition, wxDefaultSize,wxALIGN_CENTRE,
			wxT("NoAudioText"));
		audioSizer->Add(noAudio, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
		audioSizer->Layout();
		m_audioChannels.clear();
	}
}

void AudioControlPage::DisableAudioPanel()
{
	auto* text   = (wxTextCtrl*)FindWindowByName(wxT("MidiInFileInput"));
	auto* button = (wxButton*)  FindWindowByName(wxT("BrowseMidiInButton"));
	auto* combo  = (wxComboBox*)FindWindowByName(wxT("MidiInSelector"));
	combo->Clear(); combo ->Enable(false);
	text ->Clear(); text  ->Enable(false);
	                button->Enable(false);

	text   = (wxTextCtrl*)FindWindowByName(wxT("MidiOutFileInput"));
	button = (wxButton*)  FindWindowByName(wxT("BrowseMidiOutButton"));
	combo  = (wxComboBox*)FindWindowByName(wxT("MidiOutSelector"));
	combo->Clear(); combo ->Enable(false);
	text ->Clear(); text  ->Enable(false);
	                button->Enable(false);

	text   = (wxTextCtrl*)FindWindowByName(wxT("SampleFileInput"));
	button = (wxButton*)  FindWindowByName(wxT("BrowseSampleInputButton"));
	combo  = (wxComboBox*)FindWindowByName(wxT("SampleInSelector"));
	combo->Clear(); combo ->Enable(false);
	text ->Clear(); text  ->Enable(false);
	                button->Enable(false);

	m_midiInLabel->Enable(false);
	m_midiOutLabel->Enable(false);
	m_sampleInLabel->Enable(false);
	m_midiInFileLabel->Enable(false);
	m_midiOutFileLabel->Enable(false);
	m_sampleInFileLabel->Enable(false);

	if (auto* temp = FindWindowByLabel(wxT("Sound I/O"))) {
		temp->Enable(false);
	}
}

void AudioControlPage::AddChannel(int channelnumber)
{
	wxSize defaultsize;
#ifdef __WXMSW__
	defaultsize = wxDefaultSize;
#else
	defaultsize = wxSize(45, 18); // was 40 for wx 2.4 TODO make conditional
#endif
	int maxvol = 100;
	auto* audioSizer = m_audioPanel->GetSizer();
	auto number = wxString::Format(wxT("%u"), channelnumber);

	wxString labeltext = m_audioChannels[channelnumber].displayType.Mid(2);
	auto* label = new wxStaticText(
		m_audioPanel, -1, labeltext, wxDefaultPosition,
		wxDefaultSize, 0, wxT("AudioLabel_") + number);
	auto* slider = new wxSlider(
		m_audioPanel, FIRSTAUDIOSLIDER + channelnumber, 0, 0, maxvol,
		wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL,
		wxDefaultValidator, wxT("AudioSlider_") + number);

	const wxString& chanName = m_audioChannels[channelnumber].name;
	const wxString& chanType = m_audioChannels[channelnumber].type;
	wxString chanDesc = chanName;
	if (chanType != chanName) {
		chanDesc += wxT(" (") +chanType +wxT(")");
	}
	chanDesc.Replace(wxT("\\ "), wxT(" "));
	slider->SetToolTip(chanDesc + wxT(" volume"));

	wxComboBox* combo = nullptr;
	wxToggleButton* button = nullptr;
	if (chanType.StartsWith(wxT("MoonSound"))) {
		wxString choices2[] = {wxT("S"), wxT("O")};
		combo = new wxComboBox(
			m_audioPanel, FIRSTAUDIOCOMBO + channelnumber,
			wxT("S"), wxDefaultPosition, defaultsize, 2, choices2,
			wxCB_READONLY, wxDefaultValidator,
			wxT("AudioMode_") + number);
		combo->SetToolTip(wxT("Channel Mode"));
	} else if (chanType == wxT("master")) {
		button = new wxToggleButton(
			m_audioPanel, MUTEBUTTONID, wxT("Mute"),
			wxDefaultPosition, wxDefaultSize, 0,
			wxDefaultValidator, wxT("MuteButton"));
	} else {
		wxString choices1[] = {wxT("M"), wxT("L"), wxT("R"), wxT("O")};
		combo = new wxComboBox(
			m_audioPanel, FIRSTAUDIOCOMBO + channelnumber,
			wxT("M"), wxDefaultPosition, defaultsize, 4,
			choices1, wxCB_READONLY, wxDefaultValidator,
			wxT("AudioMode_") + number);
		combo->SetToolTip(wxT("Channel Mode"));
	}

	auto* sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(label,  0, wxALIGN_CENTER_HORIZONTAL, 0);
	sizer->Add(slider, 1, wxALIGN_CENTER_HORIZONTAL, 0);
	if (channelnumber != 0) {
		sizer->Add(combo,  0, wxALIGN_CENTER_HORIZONTAL, 0);
	} else {
		sizer->Add(button, 0, wxALIGN_CENTER_HORIZONTAL, 0);
	}
	audioSizer->Add(sizer, 0, wxEXPAND | wxRIGHT, 10);

	for (int i = wxEVT_SCROLL_TOP; i <= wxEVT_SCROLL_ENDSCROLL; ++i) {
		Connect(FIRSTAUDIOSLIDER + channelnumber, i,
		        (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&AudioControlPage::OnChangeVolume);
	}
	Connect(FIRSTAUDIOCOMBO + channelnumber, wxEVT_COMMAND_COMBOBOX_SELECTED,
		(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&AudioControlPage::OnChangeMode);
	Connect(MUTEBUTTONID, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
		(wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&AudioControlPage::OnMute);
}

void AudioControlPage::OnChangeVolume(wxScrollEvent& event)
{
	int id = event.GetId();
	const wxString& channelname = m_audioChannels[id - FIRSTAUDIOSLIDER].name;
	int scrollpos = 100 - event.GetPosition();
	m_controller.WriteCommand(wxString::Format(
		wxT("set %s_volume %d"), channelname.c_str(), scrollpos));
}

void AudioControlPage::OnChangeMode(wxCommandEvent& event)
{
	CatapultPage::OnClickCombo(event);
	int id = event.GetId();
	const wxString& channelname = m_audioChannels[id - FIRSTAUDIOCOMBO].name;
	const wxString tempval = ((wxComboBox*)event.GetEventObject())->GetValue();
	wxString value;
	switch (tempval[0]) {
	case 'M':
		value = wxT("0");    break;
	case 'L':
		value = wxT("-100"); break;
	case 'R':
		value = wxT("100");  break;
	case 'O':
		value = wxT("mute"); break;
	case 'S':
		value = wxT("0");    break; // unmute
	default:
		break;
	}
	if (value.IsNumber()) {
		m_controller.WriteCommand(wxString::Format(
			wxT("set %s_balance %s"), channelname.c_str(), value.c_str()));
		m_controller.WriteCommand(wxString::Format(
			wxT("unmute_channels %s"), channelname.c_str()));
	} else {
		m_controller.WriteCommand(wxString::Format(
			wxT("mute_channels %s"), channelname.c_str()));
	}
}

void AudioControlPage::OnMute(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("toggle mute"));
}

int AudioControlPage::FindChannel(const wxString& name_) const
{
	// HACK!! 'ChannelInfo::name' is Tcl-escaped, but 'name' isn't.
	wxString name = name_;
	name.Replace(wxT(" "), wxT("\\ ")); // This shouldn't be necessary :(
	for (size_t i = 0; i < m_audioChannels.size(); ++i) {
		if (m_audioChannels[i].name == name) return i;
	}
	return -1;
}

void AudioControlPage::SetChannelVolume(const wxString& name, const wxString& value)
{
	int channel = FindChannel(name);
	if (channel == -1) return;

	long intvalue;
	auto* slider = (wxSlider*)FindWindowById(channel + FIRSTAUDIOSLIDER, this);
	value.ToLong(&intvalue);
	slider->SetValue(100 - intvalue);
}

void AudioControlPage::SetChannelMode(const wxString& name, const wxString& value)
{
	int channel = FindChannel(name);
	if (channel == -1) return;

	wxString val;
	if (value.IsNumber()) {
		long num;
		value.ToLong(&num);
		val = wxT("M");
		if (num <= -33) val = wxT("L");
		if (num >=  33) val = wxT("R");
		const wxString& chanType = m_audioChannels[channel].type;
		if ((chanType.StartsWith(wxT("MoonSound"))) &&
		    ((val == wxT("L")) || (val == wxT("M")) || (val == wxT("R")))) {
			val = wxT("S");
		}
	}
	//if (value == wxT("off"))    val = wxT("O"); // mute
	//if (value == wxT("stereo")) val = wxT("S");
	auto* combo = (wxComboBox*)FindWindowById(channel + FIRSTAUDIOCOMBO, this);
	combo->SetSelection(combo->FindString(val));
}

void AudioControlPage::InitAudioIO()
{
	bool available = false;
	for (auto& connector : m_controller.GetConnectors()) {
		const wxArrayString& pluggables   = m_controller.GetPluggables();
		const wxArrayString& descriptions = m_controller.GetPluggableDescriptions();
		if (connector == wxT("msx-midi-in")) {
			available = true;
			if (auto* child = (wxComboBox*)FindWindowByName(wxT("MidiInSelector"))) {
				child->Enable(true);
				child->Clear();
				m_midiInLabel->Enable(true);
				m_midiInFileLabel->Enable(true);
				auto* miditext = (wxTextCtrl*)FindWindowByName(wxT("MidiInFileInput"));
				auto* midibutton = (wxButton*)FindWindowByName(wxT("BrowseMidiInButton"));
				miditext->Enable(true);
				midibutton->Enable(true);
				child->Append(wxT("--empty--"));
				for (unsigned j = 0; j < pluggables.GetCount(); ++j) {
					if (pluggables[j].Lower().StartsWith(wxT("midi-in"))) {
						if (pluggables[j] == wxT("midi-in-reader")) {
							child->Append(pluggables[j]);
						} else {
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(wxT("--empty--"));

			}
		}
		if (connector == wxT("msx-midi-out")) {
			available = true;
			if (auto* child = (wxComboBox *)FindWindowByName (wxT("MidiOutSelector"))) {
				child->Enable(true);
				child->Clear();
				m_midiOutLabel->Enable(true);
				m_midiOutFileLabel->Enable(true);
				auto* miditext = (wxTextCtrl*)FindWindowByName(wxT("MidiOutFileInput"));
				auto* midibutton = (wxButton*)FindWindowByName(wxT("BrowseMidiOutButton"));
				miditext->Enable(true);
				midibutton->Enable(true);
				child->Append(wxT("--empty--"));
				for (unsigned j = 0; j < pluggables.GetCount(); ++j) {
					if (pluggables[j].Lower().StartsWith(wxT("midi-out"))) {
						if (pluggables[j] == wxT("midi-out-logger")) {
							child->Append(pluggables[j]);
						} else {
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(wxT("--empty--"));
			}
		}
		if (connector == wxT("pcminput")) {
			available = true;
			if (auto* child = (wxComboBox *)FindWindowByName (wxT("SampleInSelector"))) {
				child->Enable(true);
				child->Clear();
				m_sampleInLabel->Enable(true);
				m_sampleInFileLabel->Enable(true);
				auto* sampletext = (wxTextCtrl*)FindWindowByName(wxT("SampleFileInput"));
				auto* samplebutton = (wxButton*)FindWindowByName(wxT("BrowseSampleInputButton"));
				sampletext->Enable(true);
				samplebutton->Enable(true);
				child->Append(wxT("--empty--"));
				for (unsigned j = 0; j < pluggables.GetCount(); ++j) {
					if (pluggables[j] == wxT("wavinput")) {
						child->Append(pluggables[j]);
					}
				}
				child->SetValue(wxT("--empty--"));
			}
		}
	}
	if (available){
		if (auto* temp = FindWindowByLabel(wxT("Sound I/O"))) {
			temp->Enable(true);
		}
	}
}

void AudioControlPage::OnChangeMidiInPlug(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")) {
		m_controller.WriteCommand(wxT("unplug msx-midi-in"));
		return;
	}
	const wxArrayString& pluggables   = m_controller.GetPluggables();
	const wxArrayString& descriptions = m_controller.GetPluggableDescriptions();
	for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
		if ((value == descriptions[i]) && pluggables[i].Contains(wxT("midi-in"))) {
			value = pluggables[i];
		}
	}
	m_controller.WriteCommand(wxT("plug msx-midi-in ") + value);
}

void AudioControlPage::InvalidMidiInReader()
{
	wxMessageBox(wxT("Unable to plug in MIDI-in file reader!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("MidiInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller.WriteCommand(wxT("unplug msx-midi-in"));
}

void AudioControlPage::OnChangeMidiOutPlug(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")) {
		m_controller.WriteCommand(wxT("unplug msx-midi-out"));
		return;
	}
	const wxArrayString& pluggables   = m_controller.GetPluggables();
	const wxArrayString& descriptions = m_controller.GetPluggableDescriptions();
	for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
		if ((value == descriptions[i]) && pluggables[i].Contains(wxT("midi-out"))) {
			value = pluggables[i];
		}
	}
	m_controller.WriteCommand(wxT("plug msx-midi-out ") + value);
}

void AudioControlPage::InvalidMidiOutLogger()
{
	wxMessageBox(wxT("Unable to plug in MIDI-out logger!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("MidiOutSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller.WriteCommand(wxT("unplug msx-midi-out"));
}

void AudioControlPage::OnChangeSampleInPlug(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")) {
		m_controller.WriteCommand(wxT("unplug pcminput"));
		return;
	}
	m_controller.WriteCommand(wxT("plug pcminput ") + box->GetValue());
}

void AudioControlPage::InvalidSampleFilename()
{
	wxMessageBox(wxT("Unable to plug in PCM wave input!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("SampleInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller.WriteCommand(wxT("unplug pcminput"));
}

void AudioControlPage::OnBrowseMidiInFile(wxCommandEvent& event)
{
	auto* miditext = (wxTextCtrl*)FindWindowByName (wxT("MidiInFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this, wxT("Select MIDI input file"), defaultpath, wxT(""), wxT("*.*"), wxOPEN);
	if (filedlg.ShowModal() == wxID_OK) {
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()) {
			m_controller.WriteCommand(wxT("set midi-in-readfilename ") + ConvertPath(miditext->GetValue(), true));
		}
	}
}

void AudioControlPage::OnBrowseMidiOutFile(wxCommandEvent& event)
{
	auto* miditext = (wxTextCtrl*)FindWindowByName(wxT("MidiOutFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this, wxT("Select MIDI output file"), defaultpath, wxT(""), wxT("*.*"), wxSAVE | wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK) {
		miditext->SetValue(filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()) {
			m_controller.WriteCommand(wxT("set midi-out-logfilename ") + ConvertPath(miditext->GetValue(), true));
		}
	}
}

void AudioControlPage::OnBrowseSampleInFile(wxCommandEvent& event)
{
	auto* sampletext = (wxTextCtrl*)FindWindowByName(wxT("SampleFileInput"));
	wxString defaultpath = ::wxPathOnly(sampletext->GetValue());
	wxFileDialog filedlg(this, wxT("Select PCM sample input file"), defaultpath, wxT(""), wxT("*.*"), wxOPEN);
	if (filedlg.ShowModal() == wxID_OK) {
		sampletext->SetValue(filedlg.GetPath());
		if (!sampletext->GetValue().IsEmpty()) {
			m_controller.WriteCommand(wxT("set audio-inputfilename ") + ConvertPath(sampletext->GetValue(), true));
		}
	}
}

void AudioControlPage::UpdateMidiPlug(const wxString& connector, const wxString& data)
{
	const wxArrayString& pluggables = m_controller.GetPluggables();
	if (pluggables.IsEmpty()) return;

	wxString value;
	if (data.Lower().StartsWith(wxT("midi-in"))) {
		if (data == wxT("midi-in-reader")) {
			value = data;
		}
	} else if (data.Lower().StartsWith(wxT("midi-out"))) {
		if (data == wxT("midi-out-logger")) {
			value = data;
		}
	} else {
		value = data;
	}

	if (value.IsEmpty()) {
		const wxArrayString& descriptions = m_controller.GetPluggableDescriptions();
		if (descriptions.GetCount() < pluggables.GetCount()) {
			return;
		}
		for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
			if (pluggables[i] == data) {
				value = descriptions[i];
			}
		}
	}
	if (auto* box = (wxComboBox*)FindWindowByName(connector)) {
		box->SetValue(data.IsEmpty() ? wxT("--empty--") : value);
	}
}
