// $Id: AudioControlPage.h,v 1.11 2004/04/12 19:28:08 h_oudejans Exp $
// AudioControlPage.h: interface for the AudioControlPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AUDIOCONTROLPAGE_H
#define AUDIOCONTROLPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MUTEBUTTONID 19999
#define FIRSTAUDIOSLIDER 20000
#define FIRSTAUDIOCOMBO 21000

class openMSXController;

class AudioControlPage : public CatapultPage  
{
	public:
		void SetMidiFilename (wxString value, bool midiIn);
		void SetSampleFilename (wxString value);
		void InvalidMidiInReader();
		void InvalidMidiOutLogger();
		void InvalidSampleFilename();
		void SetChannelVolume(int number, wxString value);
		void SetChannelMode (int number,wxString value);
		wxString GetAudioChannelName(int number);
		wxString GetAudioChannelType (int number);
		int GetNumberOfAudioChannels();
		void OnChangeVolume(wxScrollEvent& event);
		void OnChangeMode(wxCommandEvent& event);
		void OnMute(wxCommandEvent& event);
		void OnChangeMidiInPlug(wxCommandEvent & event);
		void OnChangeMidiOutPlug(wxCommandEvent & event);
		void OnChangeSampleInPlug(wxCommandEvent & event);
		void OnBrowseMidiInFile(wxCommandEvent & event);
		void OnBrowseMidiOutFile(wxCommandEvent & event);
		void OnBrowseSampleInFile(wxCommandEvent & event);		
		void InitAudioChannels(wxString channels);
		void AddChannelType(int channel,wxString type);
		void SetupAudioMixer ();
		void DestroyAudioMixer ();
		void DisableAudioPanel ();
		AudioControlPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller = NULL);
		virtual ~AudioControlPage();
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus);
		void InitAudioIO();
		void UpdateMidiPlug (wxString connector, wxString data);

	private:
		void ConvertChannelNames(wxArrayString & names);
		void AddChannel (wxString label, int channelnumber);
		openMSXController * m_controller;
		wxPanel * m_audioPanel;
		wxArrayString m_audioChannels;
		wxString m_midiInFilename;
		wxString m_midiOutFilename;
		wxString m_sampleInFilename;

		DECLARE_CLASS(AudioControlPage)
		// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()

};

#endif 
