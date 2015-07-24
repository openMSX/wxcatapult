#ifndef AUDIOCONTROLPAGE_H
#define AUDIOCONTROLPAGE_H

#include "CatapultPage.h"
#include <vector>

class openMSXController;
class wxStaticText;
class wxBitmapButton;

class AudioControlPage : public CatapultPage
{
public:
	AudioControlPage(wxWindow* parent, openMSXController& controller);

	void SetChannelVolume(const wxString& name, const wxString& value);
	void SetChannelMode  (const wxString& name, const wxString& value);
	void InitAudioChannels();
	void AddChannelType(const wxString& name, const wxString& type);
	void SetupAudioMixer();
	void DestroyAudioMixer();
	void DisableAudioPanel();
	void InitAudioIO();
	void UpdateMidiPlug(const wxString& connector, const wxString& data);

private:
	void InvalidMidiInReader();
	void InvalidMidiOutLogger();
	void InvalidSampleFilename();
	void AddChannel(int channelnumber);
	int FindChannel(const wxString& name) const;
	void OnChangeVolume(wxScrollEvent& event);
	void OnChangeMode(wxCommandEvent& event);
	void OnMute(wxCommandEvent& event);
	void OnChangeMidiInPlug(wxCommandEvent& event);
	void OnChangeMidiOutPlug(wxCommandEvent& event);
	void OnChangeSampleInPlug(wxCommandEvent& event);
	void OnBrowseMidiInFile(wxCommandEvent& event);
	void OnBrowseMidiOutFile(wxCommandEvent& event);
	void OnBrowseSampleInFile(wxCommandEvent& event);

	openMSXController& m_controller;
	wxPanel* m_audioPanel;

	struct ChannelInfo {
		wxString name;
		wxString type;
		wxString displayType; // includes 2-char sort-id
	};
	std::vector<ChannelInfo> m_audioChannels;
	wxString m_midiInConnector;
	wxString m_midiOutConnector;

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
