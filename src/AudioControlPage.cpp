// $id: $
// AudioControlPage.cpp: implementation of the AudioControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "openMSXController.h"
#include "StatusPage.h"
#include "AudioControlPage.h"
#include "wxCatapultFrm.h"

IMPLEMENT_CLASS(AudioControlPage, wxPanel)
BEGIN_EVENT_TABLE(AudioControlPage, wxPanel)
	EVT_COMBOBOX(XRCID("MidiInSelector"),AudioControlPage::OnChangeMidiInPlug)
	EVT_COMBOBOX(XRCID("MidiOutSelector"),AudioControlPage::OnChangeMidiOutPlug)
	EVT_COMBOBOX(XRCID("SampleInSelector"),AudioControlPage::OnChangeSampleInPlug)
	EVT_BUTTON(XRCID("BrowseMidiInButton"),AudioControlPage::OnBrowseMidiInFile)
	EVT_BUTTON(XRCID("BrowseMidiOutButton"),AudioControlPage::OnBrowseMidiOutFile)
	EVT_BUTTON(XRCID("BrowseSampleInputButton"),AudioControlPage::OnBrowseSampleInFile)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AudioControlPage::AudioControlPage(wxWindow * parent, openMSXController * controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("AudioControlPage"));
	m_audioPanel = (wxPanel *)FindWindow (_("AudioChannelPanel"));
	m_controller=controller;
	m_masterVolumeEnabled = true;
	m_midiInFilename = _("");
	m_midiOutFilename = _("");
}

AudioControlPage::~AudioControlPage()
{
}

void AudioControlPage::InitAudioChannels(wxString channels)
{
	if (channels.IsEmpty())
		return;
	m_audioChannels.Clear();
	m_audioChannels.Add("master;;master");
	int pos;
	wxString temp = channels;
	do
	{
		pos = temp.Find(_("\n"));
		if (pos != -1)
		{
			m_audioChannels.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_audioChannels.Add(temp);
}

void AudioControlPage::AddChannelType(int channel,wxString type)
{
	m_audioChannels[channel] = type + _(";;") + m_audioChannels[channel];
}

void AudioControlPage::SetupAudioMixer()
{
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxStaticText * noAudio = (wxStaticText *) FindWindow(_("NoAudioText"));
	AudioSizer->Remove(0);
	delete noAudio;

	ConvertChannelNames (m_audioChannels);
	for (unsigned int i=0;i<m_audioChannels.GetCount();i++){
		AddChannel (m_audioChannels[i],i);
	}
	
	if (!m_masterVolumeEnabled){
		wxWindow * mastervol = FindWindow(_("AudioSlider_0"));
		mastervol->Enable(false);
	}
	InitAudioIO();
}

void AudioControlPage::DestroyAudioMixer()
{
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxWindow * child; 
	if (m_audioChannels.GetCount() > 0)
	{
		for (unsigned i=m_audioChannels.GetCount();i>0;i--)	
		{
			AudioSizer->Remove(i-1);
			wxString number;
			number.sprintf ("%u",i-1);
			child = FindWindow(wxString(_("AudioLabel_") + number));
			if (child){
				delete child;
			}
			child = FindWindow(wxString(_("AudioSlider_") + number));
			if (child){
				delete child;
			}
			child = FindWindow(wxString(_("AudioMode_") + number));
			if (child){
				delete child;
			}
		}
		child = FindWindow (wxString(_("MuteButton")));
		if (child){
			delete child;
		}

		wxStaticText * noAudio = new wxStaticText(m_audioPanel, -1, _("No audio channel data available"),
				wxDefaultPosition, wxDefaultSize,wxALIGN_CENTRE, _("NoAudioText"));
		AudioSizer->Add(noAudio, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
		AudioSizer->Layout();
		m_audioChannels.Clear();
		wxTextCtrl * text = (wxTextCtrl *)FindWindow (_("MidiInFileInput"));
		wxButton * button = (wxButton *)FindWindow(_("BrowseMidiInButton"));
		wxComboBox * combo = (wxComboBox *)FindWindow (_("MidiInSelector"));
		combo->Clear();combo->Enable(false);
		text->Clear(); text->Enable(false);
		button->Enable(false);
		text = (wxTextCtrl *)FindWindow (_("MidiOutFileInput"));
		button = (wxButton *)FindWindow(_("BrowseMidiOutButton"));
		combo = (wxComboBox *)FindWindow (_("MidiOutSelector"));
		combo->Clear();combo->Enable(false);
		text->Clear(); text->Enable(false);
		button->Enable(false);
		text = (wxTextCtrl *)FindWindow (_("SampleFileInput"));
		button = (wxButton *)FindWindow(_("BrowseSampleInputButton"));
		combo = (wxComboBox *)FindWindow (_("SampleInSelector"));
		combo->Clear();combo->Enable(false);
		text->Clear(); text->Enable(false);
		button->Enable(false);
	}
}

void AudioControlPage::AddChannel(wxString labeltext, int channelnumber)
{
	wxSize defaultsize;
#ifdef __WINDOWS__
	defaultsize = wxDefaultSize;
#else
	defaultsize = wxSize(40,18);
#endif
	int maxvol = 100;
	if (!m_masterVolumeEnabled){
		maxvol = 32767;
	}
	int i;
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxString choices1[3]={_("M"), _("L"), _("R")};
	wxString choices2[1]={_("S")};
	wxString number;
	number.sprintf("%u",channelnumber);
	wxStaticText * label = new wxStaticText(m_audioPanel, -1, labeltext.Mid(0,labeltext.Find("::")),
			wxDefaultPosition, wxDefaultSize,0,
			wxString(_("AudioLabel_")+number));
	wxSlider * slider = new wxSlider(m_audioPanel,FIRSTAUDIOSLIDER+channelnumber,0,0,maxvol,wxDefaultPosition,
			wxDefaultSize,wxSL_VERTICAL,wxDefaultValidator,
			wxString(_("AudioSlider_")+number));
	wxComboBox * combo = NULL;
	wxToggleButton * button = NULL;
	if (GetAudioChannelType(channelnumber).Mid(0,9) == _("MoonSound")){
		combo = new wxComboBox(m_audioPanel,FIRSTAUDIOCOMBO+channelnumber,
				_("S"), wxDefaultPosition,defaultsize,1,choices2,wxCB_READONLY,
				wxDefaultValidator,wxString(_("AudioMode_")+number));
	}
	else if (GetAudioChannelType(channelnumber) == _("master")){	
		button = new wxToggleButton(m_audioPanel,MUTEBUTTONID,_("Mute"), wxDefaultPosition,
				wxDefaultSize,0,wxDefaultValidator,wxString(_("MuteButton")));
	}
	else {	
		combo = new wxComboBox(m_audioPanel,FIRSTAUDIOCOMBO+channelnumber,
				_("M"), wxDefaultPosition,defaultsize,3,choices1,wxCB_READONLY,
				wxDefaultValidator,wxString(_("AudioMode_")+number));
	}

	wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(label, 0, wxALIGN_CENTER_HORIZONTAL,0);
	sizer->Add(slider, 1, wxALIGN_CENTER_HORIZONTAL,0);
	if (channelnumber !=0){	
		sizer->Add(combo,0,wxALIGN_CENTER_HORIZONTAL,0);
	}
	else {
		sizer->Add(button,0,wxALIGN_CENTER_HORIZONTAL,0);
	}
	AudioSizer->Add(sizer,0,wxEXPAND | wxRIGHT,10);
	AudioSizer->Fit(m_audioPanel);

	for (i=wxEVT_SCROLL_TOP;i<=wxEVT_SCROLL_ENDSCROLL;i++){	
		Connect(FIRSTAUDIOSLIDER+channelnumber,i,(wxObjectEventFunction)
				(wxEventFunction)(wxCommandEventFunction)
				&AudioControlPage::OnChangeVolume);
	}
	Connect (FIRSTAUDIOCOMBO+channelnumber,wxEVT_COMMAND_COMBOBOX_SELECTED ,(wxObjectEventFunction)
			(wxEventFunction)(wxCommandEventFunction)
			&AudioControlPage::OnChangeMode);
	Connect (MUTEBUTTONID, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, (wxObjectEventFunction)
			(wxEventFunction)(wxCommandEventFunction)
			&AudioControlPage::OnMute);
}

void AudioControlPage::ConvertChannelNames(wxArrayString & names)
{
	unsigned int i;
	wxArrayString In;
	wxArrayString Out;
	In.Add(_("master"));Out.Add(_("a master\n"));
	In.Add(_("1-bit click generator"));Out.Add(_("b click\n"));
	In.Add(_("PSG"));Out.Add(_("c PSG\n"));
	In.Add(_("Turbo-R PCM"));Out.Add(_("d PCM\n"));
	In.Add(_("MSX-MUSIC"));Out.Add(_("e Music\n"));
	In.Add(_("Konami SCC"));Out.Add(_("g SCC\n"));
	In.Add(_("Konami SCC+"));Out.Add(_("h SCC+\n"));
	In.Add(_("MSX-AUDIO"));Out.Add(_("i Audio\nFM"));
	In.Add(_("MSX-AUDIO 13-bit DAC"));Out.Add(_("j Audio\nDAC"));
	In.Add(_("MoonSound FM-part"));Out.Add(_("l MSnd\nFM"));
	In.Add(_("MoonSound wave-part"));Out.Add(_("m MSnd\nWave"));
	In.Add(_("Majutsushi DAC"));Out.Add(_("o Majutsu\nDAC"));
	In.Add(_("Konami Synthesizer DAC"));Out.Add(_("p Konami\nSynth"));
	In.Add(_("Play samples via your printer port."));Out.Add(_("q Simple"));
	In.Add(_("Cassetteplayer, use to read .cas or .wav files."));Out.Add(_("z tape\n"));

	for (i=0;i<names.GetCount();i++){
		unsigned int InIndex = 0;
		bool found = false;
		while ((InIndex < In.GetCount()) && (found == false)){
			if (GetAudioChannelType(i) == In[InIndex]){
				names[i] = Out[InIndex]+ _("::") + names[i];
				found = true;
			}
			InIndex++;
		}
	}

	names.Sort();
	for (i=0;i<names.GetCount();i++){
		names[i]=names[i].substr(2);
	}
}

void AudioControlPage::OnChangeVolume(wxScrollEvent& event)
{
	int ID=event.GetId();
	wxString channelname = GetAudioChannelName(ID-FIRSTAUDIOSLIDER);
	int scrollpos = 100-event.GetPosition();
	wxString cmd;
	cmd.sprintf("set %s_volume %d",channelname.c_str(),scrollpos);
	m_controller->WriteCommand(cmd);
}

void AudioControlPage::OnChangeMode(wxCommandEvent& event)
{
	int ID=event.GetId();
	wxString channelname = GetAudioChannelName(ID-FIRSTAUDIOCOMBO);
	const wxString tempval = ((wxComboBox *)event.GetEventObject())->GetValue();
	wxString value;
	switch (tempval[0])
	{
		case 'M':
			value = _("mono");
			break;
		case 'L':
			value = _("left");
			break;
		case 'R':
			value = _("right");
			break;
		case 'S':
			value = _("stereo");
		default:
			break;
	}
	wxString cmd;
	cmd.sprintf("set %s_mode %s",channelname.c_str(),value.c_str());
	m_controller->WriteCommand(cmd);
}

void AudioControlPage::OnMute(wxCommandEvent& event)
{
	m_controller->WriteCommand ("toggle mute");
}

wxString AudioControlPage::GetAudioChannelName (int number)
{
	wxString temp;
	int pos = m_audioChannels[number].Find(";;");
	if (pos == -1){
		temp = m_audioChannels[number];
	}
	else{
		temp = m_audioChannels[number].Mid(pos+2);
	}
	temp.Replace (" ","\\ ",true);	
	return wxString(temp);
}

wxString AudioControlPage::GetAudioChannelType (int number)
{
	wxString temp;
	int pos2 = m_audioChannels[number].Find(";;");
	if (pos2 == -1){
		return wxString(_(""));
	}
	int pos = m_audioChannels[number].Find("::");
	if (pos == -1){
		temp = m_audioChannels[number].Mid(0,pos2);
	}
	else{
		temp = m_audioChannels[number].Mid(pos+2,pos2-pos-2);
	}
	temp.Replace ("\n"," ",true);
	temp.Trim ();
	return wxString(temp);
}

int AudioControlPage::GetNumberOfAudioChannels ()
{
	return m_audioChannels.GetCount();
}

void AudioControlPage::SetChannelVolume (int number, wxString value)
{
	int maxvol = 100;
	if (!m_masterVolumeEnabled){
		maxvol = 32767;
	}	
	long intvalue;
	wxSlider * slider = (wxSlider *)FindWindowById(number+FIRSTAUDIOSLIDER,this);
	value.ToLong(&intvalue);	
	slider->SetValue (maxvol-intvalue);
}	

void AudioControlPage::SetChannelMode (int number, wxString value)
{
	wxString val;
	if (value==_("mono")) val = _("M");
	if (value==_("left")) val = _("L");
	if (value==_("right")) val = _("R");
	if (value==_("stereo")) val = _("S");
	wxComboBox * combo = (wxComboBox *)FindWindowById(number+FIRSTAUDIOCOMBO,this);
	combo->SetValue(val);
}

void AudioControlPage::DisableMasterVolume()
{
	m_masterVolumeEnabled = false;	
}

void AudioControlPage::InitAudioIO()
{
	wxComboBox * child;
	unsigned int i;
	unsigned int j;
	wxArrayString connectors;
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetConnectors(connectors);
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	if (connectors.IsEmpty()){
		return;
	}
	for  (i=0;i<connectors.GetCount();i++){
		if (connectors[i] == "msx-midi-in"){
			child = (wxComboBox *)FindWindow (_("MidiInSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (_("MidiInFileInput"));
				wxButton * midibutton = (wxButton *)FindWindow(_("BrowseMidiInButton"));
				miditext->Enable(true);
				miditext->SetValue(m_midiInFilename);
				midibutton->Enable(true);				
				child->Append(_("--empty--"));
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j].Lower().Mid(0,7) == _("midi-in")){
						if (pluggables[j] == _("midi-in-reader")){
							child->Append(pluggables[j]);
						}
						else{
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(_("--empty--"));
				
			}
		}
		if (connectors[i] == "msx-midi-out"){
			child = (wxComboBox *)FindWindow (_("MidiOutSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (_("MidiOutFileInput"));
				wxButton * midibutton = (wxButton *)FindWindow(_("BrowseMidiOutButton"));
				miditext->Enable(true);
				miditext->SetValue(m_midiOutFilename);
				midibutton->Enable(true);				
				child->Append(_("--empty--"));
				miditext->SetValue(m_midiInFilename);
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j].Lower().Mid(0,8) == _("midi-out")){
						if (pluggables[j] == _("midi-out-logger")){
							child->Append(pluggables[j]);
						}
						else{
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(_("--empty--"));			
			}
		}
		if (connectors[i] == "pcminput"){
			child = (wxComboBox *)FindWindow (_("SampleInSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * sampletext = (wxTextCtrl *)FindWindow (_("SampleFileInput"));
				wxButton * samplebutton = (wxButton *)FindWindow(_("BrowseSampleInputButton"));
				sampletext->Enable(true);
				sampletext->SetValue(m_sampleInFilename);
				samplebutton->Enable(true);				
				child->Append(_("--empty--"));
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j] == _("wavinput")){
						child->Append(pluggables[j]);
					}
				}
				child->SetValue(_("--empty--"));				
			}
		}
	}
}

void AudioControlPage::OnChangeMidiInPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == _("--empty--")){
		m_controller->WriteCommand(_("unplug msx-midi-in"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i=0;i<pluggables.GetCount();i++){
		if ((value == descriptions[i]) && (pluggables[i].Find(_("midi-in"))!= -1)){
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(_("plug msx-midi-in ") + value));
}

void AudioControlPage::InvalidMidiInReader()
{
	wxMessageBox (_("Unable to plug in Midi filereader\nPlease select a valid filename first"),_("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (_("MidiInSelector"));
	box->SetValue(_("--empty--"));
	m_controller->WriteCommand(_("unplug msx-midi-in"));
}

void AudioControlPage::OnChangeMidiOutPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == _("--empty--")){
		m_controller->WriteCommand(_("unplug msx-midi-out"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i=0;i<pluggables.GetCount();i++){
		if ((value == descriptions[i]) && (pluggables[i].Find(_("midi-out"))!= -1)){
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(_("plug msx-midi-out ") + value));
}

void AudioControlPage::InvalidMidiOutLogger()
{
	wxMessageBox (_("Unable to plug in Midi logger\nPlease select a valid filename first"),_("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (_("MidiOutSelector"));
	box->SetValue(_("--empty--"));
	m_controller->WriteCommand(_("unplug msx-midi-out"));
}

void AudioControlPage::OnChangeSampleInPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == _("--empty--")){
		m_controller->WriteCommand(_("unplug pcminput"));
		return;
	}
	m_controller->WriteCommand(wxString(_("plug pcminput ") + box->GetValue()));
}

void AudioControlPage::InvalidSampleFilename()
{
	wxMessageBox (_("Unable to plug in Wave input\nPlease select a valid filename first"),_("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (_("SampleInSelector"));
	box->SetValue(_("--empty--"));
	m_controller->WriteCommand(_("unplug pcminput"));
}

void AudioControlPage::SetMidiFilename (wxString value, bool midiIn)
{
	if (midiIn){
		m_midiInFilename = value;
	}
	else{
		m_midiOutFilename = value;
	}
}

void AudioControlPage::SetSampleFilename (wxString value)
{
	m_sampleInFilename = value;
}

void AudioControlPage::HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)
{
}

void AudioControlPage::OnBrowseMidiInFile (wxCommandEvent & event)
{
	wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (_("MidiInFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this,_("Select Midi Inputfile"), defaultpath, _(""), _("*.*") ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()){
			m_controller->WriteCommand(_("set midi-in-readfilename ") + ConvertPath(miditext->GetValue(),true));
		}		
	}
}

void AudioControlPage::OnBrowseMidiOutFile (wxCommandEvent & event)
{
	wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (_("MidiOutFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this,_("Select Midi Outputfile"), defaultpath, _(""), _("*.*") ,wxSAVE|wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK)
	{
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()){
			m_controller->WriteCommand(_("set midi-out-logfilename ") + ConvertPath(miditext->GetValue(),true));
		}		
	}
}

void AudioControlPage::OnBrowseSampleInFile (wxCommandEvent & event)
{
	wxTextCtrl * sampletext = (wxTextCtrl *)FindWindow (_("SampleFileInput"));
	wxString defaultpath = ::wxPathOnly(sampletext->GetValue());
	wxFileDialog filedlg(this,_("Select PCM Sample Inputfile"), defaultpath, _(""), _("*.*") ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		sampletext->SetValue (filedlg.GetPath());
		if (!sampletext->GetValue().IsEmpty()){
			m_controller->WriteCommand(_("set audio-inputfilename ") + ConvertPath(sampletext->GetValue(),true));
		}		
	}
}

