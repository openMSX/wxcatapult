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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AudioControlPage::AudioControlPage(wxWindow * parent, openMSXController * controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("AudioControlPage"));
	m_audioPanel = (wxPanel *)FindWindow (_("AudioChannelPanel"));
	m_controller=controller;
}

AudioControlPage::~AudioControlPage()
{

}

void AudioControlPage::SetupAudioMixer(wxString channels)
{
	if (channels.IsEmpty())
		return;
	m_audioChannels.Clear();
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxStaticText * noAudio = (wxStaticText *) FindWindow(_("NoAudioText"));
	AudioSizer->Remove(0);
	delete noAudio;
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
	ConvertChannelNames (m_audioChannels);
	for (unsigned int i=0;i<m_audioChannels.GetCount();i++){
		AddChannel (m_audioChannels[i],i);
	}

//	GetSizer()->Fit(this); 
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
	child = FindWindow (wxString(_("_MuteButton")));
	if (child){
		delete child;
	}

	wxStaticText * noAudio = new wxStaticText(m_audioPanel, -1, _("No audio channel data available"),
							wxDefaultPosition, wxDefaultSize,wxALIGN_CENTRE, _("NoAudioText"));
	AudioSizer->Add(noAudio, 1, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 0);
	AudioSizer->Layout();
	m_audioChannels.Clear();
	}
}

void AudioControlPage::AddChannel(wxString labeltext, int channelnumber)
{
	int i;
//	if (labeltext.Len()>9)
//		labeltext=labeltext.Left(9);
//		labeltext.append(_("\n"));
	wxSizer * AudioSizer = m_audioPanel->GetSizer();
	wxString choices[3]={_("M"), _("L"), _("R")};		
	wxString number;
	number.sprintf("%u",channelnumber);
//	m_audioChannels.Add(labeltext);
	wxStaticText * label = new wxStaticText(m_audioPanel, -1, labeltext.Mid(0,labeltext.Find("::")),
					wxDefaultPosition, wxDefaultSize,0,
					wxString(_("AudioLabel_")+number));
	wxSlider * slider = new wxSlider(m_audioPanel,FIRSTAUDIOSLIDER+channelnumber,0,0,100,wxDefaultPosition,
					wxDefaultSize,wxSL_VERTICAL,wxDefaultValidator,
					wxString(_("AudioSlider_")+number));
	wxComboBox * combo = NULL;
	wxButton * button = NULL;
	if (channelnumber !=0){
		combo = new wxComboBox(m_audioPanel,FIRSTAUDIOCOMBO+channelnumber,
				_("M"), wxDefaultPosition,wxDefaultSize,3,choices,wxCB_READONLY,
				wxDefaultValidator,wxString(_("AudioMode_")+number));
	}
	else{
		button = new wxButton(m_audioPanel,-1,_("Mute"), wxDefaultPosition,
				wxDefaultSize,0,wxDefaultValidator,wxString(_("_MuteButton")));
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
		Connect(-1,i,(wxObjectEventFunction)
				(wxEventFunction)(wxCommandEventFunction)
				&AudioControlPage::OnChangeVolume);
	}
	Connect (-1,wxEVT_COMMAND_COMBOBOX_SELECTED ,(wxObjectEventFunction)
				(wxEventFunction)(wxCommandEventFunction)
				&AudioControlPage::OnChangeMode);
}

void AudioControlPage::ConvertChannelNames(wxArrayString & names)
{
	unsigned int i;
	wxArrayString In;
	wxArrayString Out;
	names.Add(_("master"));
	In.Add(_("master"));Out.Add(_("a master\n"));
	In.Add(_("keyclick"));Out.Add(_("b click\n"));
	In.Add(_("psg"));Out.Add(_("c PSG\n"));
	In.Add(_("PCM"));Out.Add(_("d PCM\n"));
	In.Add(_("MSX Music"));Out.Add(_("e Music\n"));
	In.Add(_("FM PAC"));Out.Add(_("f FMPac\n"));
	In.Add(_("Empty SCC Cartridge"));Out.Add(_("g SCC\n"));
	In.Add(_("SCC+ Cartridge"));Out.Add(_("h SCC+\n"));
	In.Add(_("MSX-Audio"));Out.Add(_("i Audio\nFM"));
	In.Add(_("MSX-Audio_DAC"));Out.Add(_("j Audio\nDAC"));
	In.Add(_("MoonSoundFM"));Out.Add(_("l MSnd\nFM"));
	In.Add(_("MoonSoundWave"));Out.Add(_("m MSnd\nWave"));
	In.Add(_("cassetteplayer"));Out.Add(_("n tape\n"));
	
	for (i=0;i<names.GetCount();i++){
		unsigned int InIndex = 0;
		bool found = false;
		while ((InIndex < In.GetCount()) && (found == false)){
			if (names[i] == In[InIndex]){
				names[i] = Out[InIndex]+ _("::") + names[i];
				found = true;
			}
			InIndex++;
		}
		if (!found){
			if (names[i].Find(_("MSXRom")) != -1){
				names[i]=_("o Cart\n")+names[i].substr(6) + _("::")+names[i];
			}
			else // shorten the name
			{
				if (names[i].Len()>18){
					names[i]=_("z ")+names[i].Mid(0,9)+"\n"+names[i].Mid(9,9)+_("::")+names[i];
				} 
				else if (names[i].Len() >9){
					names[i]=_("z ")+names[i].Mid(0,9)+"\n"+names[i].Mid(9)+_("::")+names[i];
				}
				else {
					names[i]=_("z ")+names[i]+_("\n::")+names[i];
				}
			}
		}
	}

	names.Sort();
	for (i=0;i<names.GetCount();i++){
		names[i]=names[i].substr(2);
	}
}

void AudioControlPage::OnChangeVolume(wxScrollEvent &event)
{
	int ID=event.GetId();
	wxString channel = m_audioChannels[ID-FIRSTAUDIOSLIDER];
	int pos = channel.Find(_("::"));
	wxString channelname = m_audioChannels[event.GetId()-FIRSTAUDIOSLIDER].Mid(pos+2);
	int scrollpos = 100-event.GetPosition();
	wxString cmd;
	cmd.sprintf("set %s_volume %d",channelname.c_str(),scrollpos);
	m_controller->WriteCommand(cmd);
}

void AudioControlPage::OnChangeMode(wxCommandEvent & event)
{
	int ID=event.GetId();
	wxString channelname = GetAudioChannelName(ID-FIRSTAUDIOCOMBO);
	wxString tempval = ((wxComboBox *)event.GetEventObject())->GetValue();
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
	default:
		break;
	}
	wxString cmd;
	cmd.sprintf("set %s_mode %s",channelname.c_str(),value.c_str());
	m_controller->WriteCommand(cmd);
}

wxString AudioControlPage::GetAudioChannelName (int number)
{
	wxString temp = m_audioChannels[number].Mid(m_audioChannels[number].Find("::")+2);
	temp.Replace (" ","\\ ",true);	
	return wxString(temp);
}

int AudioControlPage::GetNumberOfAudioChannels ()
{
	return m_audioChannels.GetCount();
}
	
void AudioControlPage::SetChannelVolume (int number, wxString value)
{
	long intvalue;
	wxSlider * slider = (wxSlider *)FindWindowById(number+FIRSTAUDIOSLIDER,this);
	value.ToLong(&intvalue);	
	slider->SetValue (100-intvalue);
}	
	
void AudioControlPage::SetChannelMode (int number, wxString value)
{
	wxString val;
	if (value==_("mono")) val = _("M");
	if (value==_("left")) val = _("L");
	if (value==_("right")) val = _("R");
	wxComboBox * combo = (wxComboBox *)FindWindowById(number+FIRSTAUDIOCOMBO,this);
	combo->SetValue(val);
}



