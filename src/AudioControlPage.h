// AudioControlPage.h: interface for the AudioControlPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AUDIOCONTROLPAGE_H
#define AUDIOCONTROLPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FIRSTAUDIOSLIDER 20000
#define FIRSTAUDIOCOMBO 21000

class openMSXController;

class AudioControlPage : public wxPanel  
{
	public:
		void SetChannelVolume(int number, wxString value);
		void SetChannelMode (int number,wxString value);
		wxString GetAudioChannelName(int number);
		wxString GetAudioChannelType (int number);
		int GetNumberOfAudioChannels();
		void OnChangeVolume(wxScrollEvent& event);
		void OnChangeMode(wxCommandEvent& event);
		void OnMute(wxCommandEvent& event);
		void InitAudioChannels(wxString channels);
		void AddChannelType(int channel,wxString type);
		void SetupAudioMixer ();
		void DestroyAudioMixer ();
		AudioControlPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller = NULL);
		virtual ~AudioControlPage();
	private:
		void ConvertChannelNames(wxArrayString & names);
		void AddChannel (wxString label, int channelnumber);
		openMSXController * m_controller;
		wxPanel * m_audioPanel;
		wxArrayString m_audioChannels;
};

#endif 
