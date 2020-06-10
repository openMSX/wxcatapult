#ifndef VIDEOCONTROLPAGE_H
#define VIDEOCONTROLPAGE_H

#include "CatapultPage.h"
#include <wx/tglbtn.h>

class openMSXController;
class wxComboBox;
class wxButton;
class wxStaticText;
class wxToggleButton;
class wxSlider;
class wxTextCtrl;

class VideoControlPage : public CatapultPage
{
public:
	VideoControlPage(wxWindow* parent, openMSXController& controller);

	void SetControlsOnEnd();
	void SetControlsOnLaunch();
	void SetSliderDefaults();

private:
	void UpdateScreenshotCounter();
	void OnInputScanline(wxCommandEvent& event);
	void OnInputGamma(wxCommandEvent& event);
	void OnInputNoise(wxCommandEvent& event);
	void OnInputGlow(wxCommandEvent& event);
	void OnInputBlur(wxCommandEvent& event);
	void OnDefaultScanlines(wxCommandEvent& event);
	void OnDefaultGamma(wxCommandEvent& event);
	void OnDefaultNoise(wxCommandEvent& event);
	void OnDefaultGlow(wxCommandEvent& event);
	void OnDefaultBlur(wxCommandEvent& event);
	void OnChangeScanlines(wxScrollEvent& event);
	void OnChangeGamma(wxScrollEvent& event);
	void OnChangeNoise(wxScrollEvent& event);
	void OnChangeGlow(wxScrollEvent& event);
	void OnChangeBlur(wxScrollEvent& event);
	void OnLimitSprites(wxCommandEvent& event);
	void OnDisableSprites(wxCommandEvent& event);
	void OnFullScreen(wxCommandEvent& event);
	void OnDeInterlace(wxCommandEvent& event);
	void OnVSync(wxCommandEvent& event);
	void OnChangeVideoSource(wxCommandEvent& event);
	void OnChangeScalerAlgo(wxCommandEvent& event);
	void OnChangeScalerFactor(wxCommandEvent& event);
	void OnChangeRenderer(wxCommandEvent& event);
	void OnBrowseScreenShot(wxCommandEvent& event);
	void OnTakeScreenShot(wxCommandEvent& event);
	void OnChangeScreenshotFilename(wxCommandEvent& event);
	void setNewRenderersAndScalers();

	openMSXController& m_controller;

	wxComboBox* m_videoSourceList;
	wxComboBox* m_rendererList;
	wxComboBox* m_scalerAlgoList;
	wxComboBox* m_scalerFactorList;

	wxButton* m_defaultGammaButton;
	wxButton* m_defaultNoiseButton;
	wxButton* m_defaultBlurButton;
	wxButton* m_defaultGlowButton;
	wxButton* m_defaultScanlineButton;
	wxButton* m_screenShotButton;

	wxStaticText* m_rendererLabel;
	wxStaticText* m_scalerLabel;
	wxStaticText* m_videoSourceLabel;
	wxStaticText* m_vSyncLabel;
	wxStaticText* m_deinterlaceLabel;
	wxStaticText* m_limitSpritesLabel;
	wxStaticText* m_disableSpritesLabel;
	wxStaticText* m_fullScreenLabel;
	wxStaticText* m_scanLineLabel;
	wxStaticText* m_blurLabel;
	wxStaticText* m_glowLabel;
	wxStaticText* m_gammaLabel;
	wxStaticText* m_noiseLabel;
	wxStaticText* m_timesLabel;

	wxToggleButton* m_vSyncButton;
	wxToggleButton* m_limitSpritesButton;
	wxToggleButton* m_disableSpritesButton;
	wxToggleButton* m_deinterlaceButton;
	wxToggleButton* m_fullscreenButton;

	wxSlider* m_blurSlider;
	wxSlider* m_glowSlider;
	wxSlider* m_gammaSlider;
	wxSlider* m_noiseSlider;
	wxSlider* m_scanlineSlider;

	wxTextCtrl* m_blurIndicator;
	wxTextCtrl* m_glowIndicator;
	wxTextCtrl* m_gammaIndicator;
	wxTextCtrl* m_noiseIndicator;
	wxTextCtrl* m_scanlineIndicator;
	wxTextCtrl* m_screenShotFile;
	wxTextCtrl* m_screenShotCounter;

	wxString m_defaultBlur;
	wxString m_defaultGlow;
	wxString m_defaultGamma;
	wxString m_defaultNoise;
	wxString m_defaultScanline;

	DECLARE_CLASS(VideoControlPage)
	DECLARE_EVENT_TABLE()
};

#endif
