// $Id: AudioControlPage.cpp,v 1.15 2004/04/17 14:41:17 manuelbi Exp $
// AudioControlPage.cpp: implementation of the AudioControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "openMSXController.h"
#include "StatusPage.h"
#include "AudioControlPage.h"
#include "wxCatapultFrm.h"

IMPLEMENT_CLASS(AudioControlPage, wxPanel)
BEGIN_EVENT_TABLE(AudioControlPage, wxPanel)
	EVT_COMBOBOX(XRCID("MidiInSelector"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("MidiOutSelector"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("SampleInSelector"),CatapultPage::OnClickCombo)
	EVT_TEXT(XRCID("MidiInSelector"),AudioControlPage::OnChangeMidiInPlug)
	EVT_TEXT(XRCID("MidiOutSelector"),AudioControlPage::OnChangeMidiOutPlug)
	EVT_TEXT(XRCID("SampleInSelector"),AudioControlPage::OnChangeSampleInPlug)
	EVT_BUTTON(XRCID("BrowseMidiInButton"),AudioControlPage::OnBrowseMidiInFile)
	EVT_BUTTON(XRCID("BrowseMidiOutButton"),AudioControlPage::OnBrowseMidiOutFile)
	EVT_BUTTON(XRCID("BrowseSampleInputButton"),AudioControlPage::OnBrowseSampleInFile)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AudioControlPage::AudioControlPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("AudioControlPage"));
	m_audioPanel = (wxPanel *)FindWindow (wxT("AudioChannelPanel"));
	m_controller=controller;
	m_masterVolumeEnabled = true;
	m_midiInFilename = wxT("");
	m_midiOutFilename = wxT("klein testje");
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
		pos = temp.Find(wxT("\n"));
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
	m_audioChannels[channel] = type + wxT(";;") + m_audioChannels[channel];
}

void AudioControlPage::SetupAudioMixer()
{
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxStaticText * noAudio = (wxStaticText *) FindWindow(wxT("NoAudioText"));
	AudioSizer->Remove(0);
	delete noAudio;

	ConvertChannelNames (m_audioChannels);
	for (unsigned int i=0;i<m_audioChannels.GetCount();i++){
		AddChannel (m_audioChannels[i],i);
	}
	
	if (!m_masterVolumeEnabled){
		wxWindow * mastervol = FindWindow(wxT("AudioSlider_0"));
		mastervol->Enable(false);
	}
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
			child = FindWindow(wxString(wxT("AudioLabel_") + number));
			if (child){
				delete child;
			}
			child = FindWindow(wxString(wxT("AudioSlider_") + number));
			if (child){
				delete child;
			}
			child = FindWindow(wxString(wxT("AudioMode_") + number));
			if (child){
				delete child;
			}
		}
		child = FindWindow (wxString(wxT("MuteButton")));
		if (child){
			delete child;
		}

		wxStaticText * noAudio = new wxStaticText(m_audioPanel, -1, wxT("No audio channel data available"),
				wxDefaultPosition, wxDefaultSize,wxALIGN_CENTRE, wxT("NoAudioText"));
		AudioSizer->Add(noAudio, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
		AudioSizer->Layout();
		m_audioChannels.Clear();
	}
}

void AudioControlPage::DisableAudioPanel ()
{
	wxTextCtrl * text = (wxTextCtrl *)FindWindow (wxT("MidiInFileInput"));
	wxButton * button = (wxButton *)FindWindow(wxT("BrowseMidiInButton"));
	wxComboBox * combo = (wxComboBox *)FindWindow (wxT("MidiInSelector"));
	combo->Clear();combo->Enable(false);
	text->Clear(); text->Enable(false);
	button->Enable(false);
	text = (wxTextCtrl *)FindWindow (wxT("MidiOutFileInput"));
	button = (wxButton *)FindWindow(wxT("BrowseMidiOutButton"));
	combo = (wxComboBox *)FindWindow (wxT("MidiOutSelector"));
	combo->Clear();combo->Enable(false);
	text->Clear(); text->Enable(false);
	button->Enable(false);
	text = (wxTextCtrl *)FindWindow (wxT("SampleFileInput"));
	button = (wxButton *)FindWindow(wxT("BrowseSampleInputButton"));
	combo = (wxComboBox *)FindWindow (wxT("SampleInSelector"));
	combo->Clear();combo->Enable(false);
	text->Clear(); text->Enable(false);
	button->Enable(false);
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
	wxString choices1[3]={wxT("M"), wxT("L"), wxT("R")};
	wxString choices2[1]={wxT("S")};
	wxString number;
	number.sprintf("%u",channelnumber);
	wxStaticText * label = new wxStaticText(m_audioPanel, -1, labeltext.Mid(0,labeltext.Find("::")),
			wxDefaultPosition, wxDefaultSize,0,
			wxString(wxT("AudioLabel_")+number));
	wxSlider * slider = new wxSlider(m_audioPanel,FIRSTAUDIOSLIDER+channelnumber,0,0,maxvol,wxDefaultPosition,
			wxDefaultSize,wxSL_VERTICAL,wxDefaultValidator,
			wxString(wxT("AudioSlider_")+number));
	wxString chanName =  GetAudioChannelName(channelnumber);
	wxString chanType =  GetAudioChannelType(channelnumber);
	wxString chanDesc = chanName;
	if (chanType !=  chanName) { 
		chanDesc += " (" +chanType + ")";
	}
	chanDesc.Replace ("\\ "," ",true);
	slider->SetToolTip(wxString(chanDesc + " volume"));
	wxComboBox * combo = NULL;
	wxToggleButton * button = NULL;
	if (chanType.Mid(0,9) == wxT("MoonSound")){
		combo = new wxComboBox(m_audioPanel,FIRSTAUDIOCOMBO+channelnumber,
				wxT("S"), wxDefaultPosition,defaultsize,1,choices2,wxCB_READONLY,
				wxDefaultValidator,wxString(wxT("AudioMode_")+number));
		combo->SetToolTip("Channel Mode");
	}
	else if (chanType == wxT("master")){	
		button = new wxToggleButton(m_audioPanel,MUTEBUTTONID,wxT("Mute"), wxDefaultPosition,
				wxDefaultSize,0,wxDefaultValidator,wxString(wxT("MuteButton")));
	}
	else {	
		combo = new wxComboBox(m_audioPanel,FIRSTAUDIOCOMBO+channelnumber,
				wxT("M"), wxDefaultPosition,defaultsize,3,choices1,wxCB_READONLY,
				wxDefaultValidator,wxString(wxT("AudioMode_")+number));
		combo->SetToolTip("Channel Mode");
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
	In.Add(wxT("master"));Out.Add(wxT("a master\n"));
	In.Add(wxT("1-bit click generator"));Out.Add(wxT("b click\n"));
	In.Add(wxT("PSG"));Out.Add(wxT("c PSG\n"));
	In.Add(wxT("Turbo-R PCM"));Out.Add(wxT("d PCM\n"));
	In.Add(wxT("MSX-MUSIC"));Out.Add(wxT("e Music\n"));
	In.Add(wxT("Konami SCC"));Out.Add(wxT("g SCC\n"));
	In.Add(wxT("Konami SCC+"));Out.Add(wxT("h SCC+\n"));
	In.Add(wxT("MSX-AUDIO"));Out.Add(wxT("i Audio\nFM"));
	In.Add(wxT("MSX-AUDIO 13-bit DAC"));Out.Add(wxT("j Audio\nDAC"));
	In.Add(wxT("MoonSound FM-part"));Out.Add(wxT("l MSnd\nFM"));
	In.Add(wxT("MoonSound wave-part"));Out.Add(wxT("m MSnd\nWave"));
	In.Add(wxT("Majutsushi DAC"));Out.Add(wxT("o Majutsu\nDAC"));
	In.Add(wxT("Konami Synthesizer DAC"));Out.Add(wxT("p Konami\nSynth"));
	In.Add(wxT("Play samples via your printer port."));Out.Add(wxT("q SIMPL"));
	In.Add(wxT("Cassetteplayer, use to read .cas or .wav files."));Out.Add(wxT("z tape\n"));

	for (i=0;i<names.GetCount();i++){
		unsigned int InIndex = 0;
		bool found = false;
		while ((InIndex < In.GetCount()) && (found == false)){
			if (GetAudioChannelType(i) == In[InIndex]){
				names[i] = Out[InIndex]+ wxT("::") + names[i];
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
	CatapultPage::OnClickCombo (event);
	int ID=event.GetId();
	wxString channelname = GetAudioChannelName(ID-FIRSTAUDIOCOMBO);
	const wxString tempval = ((wxComboBox *)event.GetEventObject())->GetValue();
	wxString value;
	switch (tempval[0])
	{
		case 'M':
			value = wxT("mono");
			break;
		case 'L':
			value = wxT("left");
			break;
		case 'R':
			value = wxT("right");
			break;
		case 'S':
			value = wxT("stereo");
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
		return wxString(wxT(""));
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
	if (value==wxT("mono")) val = wxT("M");
	if (value==wxT("left")) val = wxT("L");
	if (value==wxT("right")) val = wxT("R");
	if (value==wxT("stereo")) val = wxT("S");
	wxComboBox * combo = (wxComboBox *)FindWindowById(number+FIRSTAUDIOCOMBO,this);
	combo->SetSelection(combo->FindString(val));
}

void AudioControlPage::DisableMasterVolume()
{
	m_masterVolumeEnabled = false;	
}

void AudioControlPage::EnableMasterVolume()
{
	m_masterVolumeEnabled = true;
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
			child = (wxComboBox *)FindWindow (wxT("MidiInSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (wxT("MidiInFileInput"));
				wxButton * midibutton = (wxButton *)FindWindow(wxT("BrowseMidiInButton"));
				miditext->Enable(true);
				midibutton->Enable(true);				
				child->Append(wxT("--empty--"));
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j].Lower().Mid(0,7) == wxT("midi-in")){
						if (pluggables[j] == wxT("midi-in-reader")){
							child->Append(pluggables[j]);
						}
						else{
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(wxT("--empty--"));
				
			}
		}
		if (connectors[i] == "msx-midi-out"){
			child = (wxComboBox *)FindWindow (wxT("MidiOutSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (wxT("MidiOutFileInput"));
				wxButton * midibutton = (wxButton *)FindWindow(wxT("BrowseMidiOutButton"));
				miditext->Enable(true);
				midibutton->Enable(true);				
				child->Append(wxT("--empty--"));
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j].Lower().Mid(0,8) == wxT("midi-out")){
						if (pluggables[j] == wxT("midi-out-logger")){
							child->Append(pluggables[j]);
						}
						else{
							child->Append(descriptions[j]);
						}
					}
				}
				child->SetValue(wxT("--empty--"));			
			}
		}
		if (connectors[i] == "pcminput"){
			child = (wxComboBox *)FindWindow (wxT("SampleInSelector"));
			if (child){
				child->Enable(true);
				child->Clear();
				wxTextCtrl * sampletext = (wxTextCtrl *)FindWindow (wxT("SampleFileInput"));
				wxButton * samplebutton = (wxButton *)FindWindow(wxT("BrowseSampleInputButton"));
				sampletext->Enable(true);
				samplebutton->Enable(true);				
				child->Append(wxT("--empty--"));
				for (j=0;j<pluggables.GetCount();j++){
					if (pluggables[j] == wxT("wavinput")){
						child->Append(pluggables[j]);
					}
				}
				child->SetValue(wxT("--empty--"));				
			}
		}
	}
}

void AudioControlPage::OnChangeMidiInPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")){
		m_controller->WriteCommand(wxT("unplug msx-midi-in"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i=0;i<pluggables.GetCount();i++){
		if ((value == descriptions[i]) && (pluggables[i].Find(wxT("midi-in"))!= -1)){
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(wxT("plug msx-midi-in ") + value));
}

void AudioControlPage::InvalidMidiInReader()
{
	wxMessageBox (wxT("Unable to plug in Midi filereader\nPlease select a valid filename first"),wxT("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (wxT("MidiInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug msx-midi-in"));
}

void AudioControlPage::OnChangeMidiOutPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")){
		m_controller->WriteCommand(wxT("unplug msx-midi-out"));
		return;
	}
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);
	for (unsigned i=0;i<pluggables.GetCount();i++){
		if ((value == descriptions[i]) && (pluggables[i].Find(wxT("midi-out"))!= -1)){
			value = pluggables[i];
		}
	}
	m_controller->WriteCommand(wxString(wxT("plug msx-midi-out ") + value));
}

void AudioControlPage::InvalidMidiOutLogger()
{
	wxMessageBox (wxT("Unable to plug in Midi logger\nPlease select a valid filename first"),wxT("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (wxT("MidiOutSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug msx-midi-out"));
}

void AudioControlPage::OnChangeSampleInPlug(wxCommandEvent & event)
{
	wxComboBox * box = (wxComboBox *)event.GetEventObject();
	wxString value = box->GetValue();
	if (value == wxT("--empty--")){
		m_controller->WriteCommand(wxT("unplug pcminput"));
		return;
	}
	m_controller->WriteCommand(wxString(wxT("plug pcminput ") + box->GetValue()));
}

void AudioControlPage::InvalidSampleFilename()
{
	wxMessageBox (wxT("Unable to plug in Wave input\nPlease select a valid filename first"),wxT("Error"));
	wxComboBox * box = (wxComboBox *)FindWindow (wxT("SampleInSelector"));
	box->SetValue(wxT("--empty--"));
	m_controller->WriteCommand(wxT("unplug pcminput"));
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
	wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (wxT("MidiInFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this,wxT("Select Midi Inputfile"), defaultpath, wxT(""), wxT("*.*") ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()){
			m_controller->WriteCommand(wxT("set midi-in-readfilename ") + ConvertPath(miditext->GetValue(),true));
		}		
	}
}

void AudioControlPage::OnBrowseMidiOutFile (wxCommandEvent & event)
{
	wxTextCtrl * miditext = (wxTextCtrl *)FindWindow (wxT("MidiOutFileInput"));
	wxString defaultpath = ::wxPathOnly(miditext->GetValue());
	wxFileDialog filedlg(this,wxT("Select Midi Outputfile"), defaultpath, wxT(""), wxT("*.*") ,wxSAVE|wxOVERWRITE_PROMPT);
	if (filedlg.ShowModal() == wxID_OK)
	{
		miditext->SetValue (filedlg.GetPath());
		if (!miditext->GetValue().IsEmpty()){
			m_controller->WriteCommand(wxT("set midi-out-logfilename ") + ConvertPath(miditext->GetValue(),true));
		}		
	}
}

void AudioControlPage::OnBrowseSampleInFile (wxCommandEvent & event)
{
	wxTextCtrl * sampletext = (wxTextCtrl *)FindWindow (wxT("SampleFileInput"));
	wxString defaultpath = ::wxPathOnly(sampletext->GetValue());
	wxFileDialog filedlg(this,wxT("Select PCM Sample Inputfile"), defaultpath, wxT(""), wxT("*.*") ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		sampletext->SetValue (filedlg.GetPath());
		if (!sampletext->GetValue().IsEmpty()){
			m_controller->WriteCommand(wxT("set audio-inputfilename ") + ConvertPath(sampletext->GetValue(),true));
		}		
	}
}

void AudioControlPage::UpdateMidiPlug (wxString connector, wxString data)
{
	wxString value = wxT("");
	wxArrayString pluggables;
	wxArrayString descriptions;
	m_controller->GetPluggables(pluggables);
	m_controller->GetPluggableDescriptions(descriptions);	
	if (pluggables.GetCount() ==0){
		return;
	}
	if (data.Lower().Mid(0,7) == wxT("midi-in")){
		if (data == wxT("midi-in-reader")){
			value = data;
		}
	}
	else if (data.Lower().Mid(0,8) == wxT("midi-out")){
		if (data == wxT("midi-out-logger")){
			value = data;
		}
	}
	else {
		value = data;
	}

	if (value == wxT("")){
		if (descriptions.GetCount() < pluggables.GetCount()){
			return;
		}
		for (unsigned i = 0;i<pluggables.GetCount();i++){
			if (pluggables[i] == data){
				value = descriptions[i];
			}
		}
	}
	if (data == wxT("")){
		value = wxT("--empty--");
	}

	wxComboBox * box = (wxComboBox *)FindWindow(connector);
	if (box != NULL){
		box->SetValue(value);
	}
}
