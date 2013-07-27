#include "AudioControlPage.h"
#include "openMSXController.h"
#include "StatusPage.h"
#include "wxCatapultFrm.h"
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/wx.h>

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

AudioControlPage::AudioControlPage(wxWindow* parent, openMSXController* controller)
	: CatapultPage(parent)
	, m_controller(controller)
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
	m_midiInFilename  = wxT("");
	m_midiOutFilename = wxT("");
	if (auto* temp = FindWindowByLabel(wxT("Mixer"))) {
		temp->Enable(false);
	}
	if (auto* temp = FindWindowByLabel(wxT("Sound I/O"))) {
		temp->Enable(false);
	}
}

void AudioControlPage::InitAudioChannels(wxString channels)
{
	if (channels.IsEmpty()) return;

	m_audioChannels.Clear();
	m_audioChannels.Add(wxT("master;;master"));
	int pos;
	wxString temp = channels;
	do {
		pos = temp.Find(wxT("\n"));
		if (pos != -1) {
			m_audioChannels.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);
		}
	} while (pos != -1);
	if (!temp.IsEmpty()) {
		// not everything parsed?
		m_audioChannels.Add(temp);
	}
}

void AudioControlPage::AddChannelType(int channel, wxString type)
{
	m_audioChannels[channel] = type + wxT(";;") + m_audioChannels[channel];
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

	ConvertChannelNames(m_audioChannels);
	for (unsigned i = 0; i < m_audioChannels.GetCount(); ++i) {
		AddChannel(m_audioChannels[i], i);
	}

	audioSizer->Layout();
}

void AudioControlPage::DestroyAudioMixer()
{
	if (auto* temp = FindWindowByLabel(wxT("Mixer"))) {
		temp->Enable(false);
	}
	if (m_audioChannels.GetCount() > 0) {
		auto* audioSizer = m_audioPanel->GetSizer();
		for (unsigned i = m_audioChannels.GetCount(); i > 0; --i) {
			audioSizer->Remove(i - 1);
			wxString number;
			number.sprintf(wxT("%u"), i - 1);
			delete FindWindowByName(wxString(wxT("AudioLabel_")  + number));
			delete FindWindowByName(wxString(wxT("AudioSlider_") + number));
			delete FindWindowByName(wxString(wxT("AudioMode_")   + number));
		}
		delete FindWindowByName(wxString(wxT("MuteButton")));

		wxStaticText * noAudio = new wxStaticText(m_audioPanel, -1, wxT("No audio channel data available"),
				wxDefaultPosition, wxDefaultSize,wxALIGN_CENTRE, wxT("NoAudioText"));
		audioSizer->Add(noAudio, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
		audioSizer->Layout();
		m_audioChannels.Clear();
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

void AudioControlPage::AddChannel(wxString labeltext, int channelnumber)
{
	wxSize defaultsize;
#ifdef __WXMSW__
	defaultsize = wxDefaultSize;
#else
	defaultsize = wxSize(45, 18); // was 40 for wx 2.4 TODO make conditional
#endif
	int maxvol = 100;
	auto* audioSizer = m_audioPanel->GetSizer();
	wxString number;
	number.sprintf(wxT("%u"), channelnumber);

	auto* label = new wxStaticText(
		m_audioPanel, -1, labeltext.Mid(0, labeltext.Find(wxT("::"))),
		wxDefaultPosition, wxDefaultSize, 0,
		wxString(wxT("AudioLabel_") + number));
	auto* slider = new wxSlider(
		m_audioPanel, FIRSTAUDIOSLIDER + channelnumber, 0, 0, maxvol,
		wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL,
		wxDefaultValidator, wxString(wxT("AudioSlider_") + number));

	wxString chanName = GetAudioChannelName(channelnumber);
	wxString chanType = GetAudioChannelType(channelnumber);
	wxString chanDesc = chanName;
	if (chanType != chanName) {
		chanDesc += wxT(" (") +chanType +wxT(")");
	}
	chanDesc.Replace(wxT("\\ "), wxT(" "), true);
	slider->SetToolTip(wxString(chanDesc + wxT(" volume")));

	wxComboBox* combo = nullptr;
	wxToggleButton* button = nullptr;
	if (chanType.Mid(0, 9) == wxT("MoonSound")) {
		wxString choices2[] = {wxT("S"), wxT("O")};
		combo = new wxComboBox(
			m_audioPanel, FIRSTAUDIOCOMBO + channelnumber,
			wxT("S"), wxDefaultPosition, defaultsize, 2, choices2,
			wxCB_READONLY, wxDefaultValidator,
			wxString(wxT("AudioMode_") + number));
		combo->SetToolTip(wxT("Channel Mode"));
	}
	else if (chanType == wxT("master")) {
		button = new wxToggleButton(
			m_audioPanel, MUTEBUTTONID, wxT("Mute"),
			wxDefaultPosition, wxDefaultSize, 0,
			wxDefaultValidator, wxString(wxT("MuteButton")));
	} else {
		wxString choices1[] = {wxT("M"), wxT("L"), wxT("R"), wxT("O")};
		combo = new wxComboBox(
			m_audioPanel, FIRSTAUDIOCOMBO + channelnumber,
			wxT("M"), wxDefaultPosition, defaultsize, 4,
			choices1, wxCB_READONLY, wxDefaultValidator,
			wxString(wxT("AudioMode_") + number));
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

void AudioControlPage::ConvertChannelNames(wxArrayString& names)
{
	// TODO WTF is this??
	//  It seems to want to replace known names with abbreviations, but
	//  why is it so complicated? What is this extra character in front
	//  of the abbreviation, to sort the names?
	wxArrayString In;
	wxArrayString Out;
	In.Add(wxT("master")); Out.Add(wxT("a master\n"));
	In.Add(wxT("1-bit click generator")); Out.Add(wxT("b key-\nclick"));
	In.Add(wxT("PSG")); Out.Add(wxT("c PSG\n"));
	In.Add(wxT("Turbo-R PCM")); Out.Add(wxT("d PCM\n"));
	In.Add(wxT("MSX-MUSIC")); Out.Add(wxT("e MSX-\nMUSIC"));
	In.Add(wxT("Konami SCC")); Out.Add(wxT("g SCC\n"));
	In.Add(wxT("Konami SCC+")); Out.Add(wxT("h SCC+\n"));
	In.Add(wxT("MSX-AUDIO")); Out.Add(wxT("i AUDIO\nFM"));
	In.Add(wxT("MSX-AUDIO 13-bit DAC")); Out.Add(wxT("j AUDIO\nDAC"));
	In.Add(wxT("MSX-AUDIO 8-bit DAC")); Out.Add(wxT("k AUDIO\nDAC2"));
	In.Add(wxT("MoonSound FM-part")); Out.Add(wxT("l MSnd\nFM"));
	In.Add(wxT("MoonSound wave-part")); Out.Add(wxT("m MSnd\nWave"));
	In.Add(wxT("Hai no Majutsushi's DAC")); Out.Add(wxT("o Majutsu\nDAC"));
	In.Add(wxT("Konami Synthesizer's DAC")); Out.Add(wxT("p Konami\nSynth"));
	In.Add(wxT("Play samples via your printer port.")); Out.Add(wxT("q SIMPL\n"));
	In.Add(wxT("Sony Playball's DAC")); Out.Add(wxT("r Playball\n"));
	In.Add(wxT("Konami Keyboard Master's VLM5030")); Out.Add(wxT("s Konami\nVLM5030"));
	In.Add(wxT("Cassetteplayer, use to read .cas or .wav files.")); Out.Add(wxT("z cas-\nsette"));

	for (unsigned i = 0; i < names.GetCount(); ++i) {
		unsigned inIndex = 0;
		while (inIndex < In.GetCount()) {
			if (GetAudioChannelType(i) == In[inIndex]) {
				names[i] = Out[inIndex] + wxT("::") + names[i];
				break;
			}
			++inIndex;
		}
	}
	names.Sort();
	for (unsigned i = 0; i < names.GetCount(); ++i) {
		names[i] = names[i].substr(2);
	}
}

void AudioControlPage::OnChangeVolume(wxScrollEvent& event)
{
	int id = event.GetId();
	wxString channelname = GetAudioChannelName(id - FIRSTAUDIOSLIDER);
	int scrollpos = 100 - event.GetPosition();
	wxString cmd;
	cmd.sprintf(wxT("set %s_volume %d"), (char*)channelname.c_str(), scrollpos);
	m_controller->WriteCommand(cmd);
}

void AudioControlPage::OnChangeMode(wxCommandEvent& event)
{
	CatapultPage::OnClickCombo(event);
	int id = event.GetId();
	wxString channelname = GetAudioChannelName(id - FIRSTAUDIOCOMBO);
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
	wxString cmd;
	if (value.IsNumber()) {
		cmd.sprintf(wxT("set %s_balance %s"), (char*)channelname.c_str(), (char*)value.c_str());
		m_controller->WriteCommand(cmd);
		cmd.sprintf(wxT("unmute_channels %s"), (char*)channelname.c_str());
		m_controller->WriteCommand(cmd);
	} else {
		cmd.sprintf(wxT("mute_channels %s"), (char*)channelname.c_str());
		m_controller->WriteCommand(cmd);
	}
}

void AudioControlPage::OnMute(wxCommandEvent& event)
{
	m_controller->WriteCommand(wxT("toggle mute"));
}

wxString AudioControlPage::GetAudioChannelName(int number)
{
	wxString temp;
	int pos = m_audioChannels[number].Find(wxT(";;"));
	if (pos == -1) {
		temp = m_audioChannels[number];
	} else {
		temp = m_audioChannels[number].Mid(pos + 2);
	}
	temp.Replace(wxT(" "), wxT("\\ "), true);
	return temp;
}

wxString AudioControlPage::GetAudioChannelType(int number)
{
	wxString temp;
	int pos2 = m_audioChannels[number].Find(wxT(";;"));
	if (pos2 == -1) {
		return wxString(wxT(""));
	}
	int pos = m_audioChannels[number].Find(wxT("::"));
	if (pos == -1) {
		temp = m_audioChannels[number].Mid(0, pos2);
	} else {
		temp = m_audioChannels[number].Mid(pos + 2, pos2 - pos - 2);
	}
	temp.Replace(wxT("\n"), wxT(" "), true);
	temp.Trim();
	return temp;
}

unsigned int AudioControlPage::GetNumberOfAudioChannels()
{
	return (unsigned)m_audioChannels.GetCount();
}

void AudioControlPage::SetChannelVolume(int number, wxString value)
{
	long intvalue;
	auto* slider = (wxSlider*)FindWindowById(number + FIRSTAUDIOSLIDER, this);
	value.ToLong(&intvalue);
	slider->SetValue(100 - intvalue);
}

void AudioControlPage::SetChannelMode(int number, wxString value)
{
	wxString val;
	if (value.IsNumber()) {
		long num;
		value.ToLong(&num);
		val = wxT("M");
		if (num <= -33) val = wxT("L");
		if (num >=  33) val = wxT("R");
		wxString chanType = GetAudioChannelType(number);
		if ((chanType.Mid(0, 9) == wxT("MoonSound")) &&
		    ((val == wxT("L")) || (val == wxT("M")) || (val == wxT("R")))) {
			val = wxT("S");
		}
	}
	//if (value == wxT("off"))    val = wxT("O"); // mute
	//if (value == wxT("stereo")) val = wxT("S");
	auto* combo = (wxComboBox*)FindWindowById(number + FIRSTAUDIOCOMBO, this);
	combo->SetSelection(combo->FindString(val));
}

void AudioControlPage::InitAudioIO()
{
	wxArrayString connectors;
	m_controller->GetConnectors(connectors);
	if (connectors.IsEmpty()) return;

	wxArrayString pluggables;
	m_controller->GetPluggables(pluggables);
	wxArrayString descriptions;
	m_controller->GetPluggableDescriptions(descriptions);

	bool available = false;
	for (unsigned i = 0; i < connectors.GetCount(); ++i) {
		if (connectors[i] == wxT("msx-midi-in")) {
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
					if (pluggables[j].Lower().Mid(0, 7) == wxT("midi-in")) {
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
		if (connectors[i] == wxT("msx-midi-out")) {
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
					if (pluggables[j].Lower().Mid(0, 8) == wxT("midi-out")) {
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
		if (connectors[i] == wxT("pcminput")) {
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
		m_controller->WriteCommand(wxT("unplug msx-midi-in"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
		if ((value == descriptions[i]) && (pluggables[i].Find(wxT("midi-in")) != -1)) {
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(wxT("plug msx-midi-in ") + value));
}

void AudioControlPage::InvalidMidiInReader()
{
	wxMessageBox(wxT("Unable to plug in MIDI-in file reader!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("MidiInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug msx-midi-in"));
}

void AudioControlPage::OnChangeMidiOutPlug(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")) {
		m_controller->WriteCommand(wxT("unplug msx-midi-out"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
		if ((value == descriptions[i]) && (pluggables[i].Find(wxT("midi-out")) != -1)) {
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(wxT("plug msx-midi-out ") + value));
}

void AudioControlPage::InvalidMidiOutLogger()
{
	wxMessageBox(wxT("Unable to plug in MIDI-out logger!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("MidiOutSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug msx-midi-out"));
}

void AudioControlPage::OnChangeSampleInPlug(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")) {
		m_controller->WriteCommand(wxT("unplug pcminput"));
		return;
	}
	m_controller->WriteCommand(wxString(wxT("plug pcminput ") + box->GetValue()));
}

void AudioControlPage::InvalidSampleFilename()
{
	wxMessageBox(wxT("Unable to plug in PCM wave input!\nPlease select a valid file name first."),
	             wxT("Error"));
	auto* box = (wxComboBox*)FindWindowByName(wxT("SampleInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug pcminput"));
}

void AudioControlPage::SetMidiFilename(wxString value, bool midiIn)
{
	if (midiIn) {
		m_midiInFilename = value;
	} else {
		m_midiOutFilename = value;
	}
}

void AudioControlPage::SetSampleFilename(wxString value)
{
	m_sampleInFilename = value;
}

void AudioControlPage::HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus)
{
}

void AudioControlPage::OnBrowseMidiInFile(wxCommandEvent& event)
{
	auto* miditext = (wxTextCtrl*)FindWindowByName (wxT("MidiInFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this, wxT("Select MIDI input file"), defaultpath, wxT(""), wxT("*.*") ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK) {
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()) {
			m_controller->WriteCommand(wxT("set midi-in-readfilename ") + ConvertPath(miditext->GetValue(), true));
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
			m_controller->WriteCommand(wxT("set midi-out-logfilename ") + ConvertPath(miditext->GetValue(), true));
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
			m_controller->WriteCommand(wxT("set audio-inputfilename ") + ConvertPath(sampletext->GetValue(), true));
		}
	}
}

void AudioControlPage::UpdateMidiPlug(wxString connector, wxString data)
{
	wxString value = wxT("");
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	if (pluggables.GetCount() == 0) {
		return;
	}
	if (data.Lower().Mid(0, 7) == wxT("midi-in")) {
		if (data == wxT("midi-in-reader")) {
			value = data;
		}
	} else if (data.Lower().Mid(0, 8) == wxT("midi-out")) {
		if (data == wxT("midi-out-logger")) {
			value = data;
		}
	} else {
		value = data;
	}

	if (value == wxT("")) {
		if (descriptions.GetCount() < pluggables.GetCount()) {
			return;
		}
		for (unsigned i = 0; i < pluggables.GetCount(); ++i) {
			if (pluggables[i] == data) {
				value = descriptions[i];
			}
		}
	}
	if (data == wxT("")) {
		value = wxT("--empty--");
	}

	if (auto* box = (wxComboBox*)FindWindowByName(connector)) {
		box->SetValue(value);
	}
}
