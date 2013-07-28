#ifndef AUDIOCONTROLPAGE_H
#define AUDIOCONTROLPAGE_H

#include "CatapultPage.h"

#define MUTEBUTTONID 19999
#define FIRSTAUDIOSLIDER 20000
#define FIRSTAUDIOCOMBO 21000

class openMSXController;
class wxStaticText;
class wxBitmapButton;

class AudioControlPage : public CatapultPage
{
public:
	AudioControlPage(wxWindow* parent, openMSXController* controller);

	void SetMidiFilename(wxString value, bool midiIn);
	void SetSampleFilename(wxString value);
	void InvalidMidiInReader();
	void InvalidMidiOutLogger();
	void InvalidSampleFilename();
	void SetChannelVolume(int number, wxString value);
	void SetChannelMode(int number, wxString value);
	wxString GetAudioChannelName(int number);
	wxString GetAudioChannelType(int number);
	unsigned GetNumberOfAudioChannels();
	void OnChangeVolume(wxScrollEvent& event);
	void OnChangeMode(wxCommandEvent& event);
	void OnMute(wxCommandEvent& event);
	void OnChangeMidiInPlug(wxCommandEvent& event);
	void OnChangeMidiOutPlug(wxCommandEvent& event);
	void OnChangeSampleInPlug(wxCommandEvent& event);
	void OnBrowseMidiInFile(wxCommandEvent& event);
	void OnBrowseMidiOutFile(wxCommandEvent& event);
	void OnBrowseSampleInFile(wxCommandEvent& event);
	void InitAudioChannels(wxString channels);
	void AddChannelType(int channel, wxString type);
	void SetupAudioMixer();
	void DestroyAudioMixer();
	void DisableAudioPanel();
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus);
	void InitAudioIO();
	void UpdateMidiPlug(wxString connector, wxString data);

private:
	void ConvertChannelNames(wxArrayString& names);
	void AddChannel(wxString label, int channelnumber);
	openMSXController* m_controller;
	wxPanel* m_audioPanel;
	wxArrayString m_audioChannels;
	wxString m_midiInFilename;
	wxString m_midiOutFilename;
	wxString m_sampleInFilename;

	wxStaticText* m_midiInLabel;
	wxStaticText* m_midiOutLabel;
	wxStaticText* m_sampleInLabel;
	wxStaticText* m_midiInFileLabel;
	wxStaticText* m_midiOutFileLabel;
	wxStaticText* m_sampleInFileLabel;

	wxBitmapButton* m_browseMidiInButton;
	wxBitmapButton* m_browseMidiOutButton;
	wxBitmapButton* m_browseSampleInputButton;

	DECLARE_CLASS(AudioControlPage)
	DECLARE_EVENT_TABLE()
};

#endif
