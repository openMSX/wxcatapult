// $Id: VideoControlPage.h,v 1.19 2005/12/08 18:14:41 manuelbi Exp $
// VideoControlPage.h: interface for the VideoControlPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef VIDEOCONTROLPAGE_H
#define VIDEOCONTROLPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>

class wxCatapultFrame;
class openMSXController;
class VideoControlPage : public CatapultPage 
{
	public:
		void SetControlsOnEnd();
		void SetControlsOnLaunch();
		VideoControlPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller = NULL);
		virtual ~VideoControlPage();
		void OnInputScanline (wxCommandEvent & event);
		void OnInputGamma (wxCommandEvent & event);
		void OnInputGlow (wxCommandEvent & event);
		void OnInputBlur (wxCommandEvent & event);
		void OnDefaultScanlines (wxCommandEvent & event);
		void OnDefaultGamma (wxCommandEvent & event);
		void OnDefaultGlow (wxCommandEvent & event);
		void OnDefaultBlur (wxCommandEvent & event);
		void OnChangeScanlines (wxScrollEvent & event);
		void OnChangeGamma (wxScrollEvent & event);
		void OnChangeGlow (wxScrollEvent &event);
		void OnChangeBlur (wxScrollEvent &event);
		void OnLimitSprites(wxCommandEvent &event);
		void OnFullScreen (wxCommandEvent &event);
		void OnDeInterlace (wxCommandEvent &event);
		void OnChangeAccuracy (wxCommandEvent &event);
		void OnChangeScalerAlgo (wxCommandEvent &event);
		void OnChangeScalerFactor (wxCommandEvent &event);
		void OnChangeRenderer(wxCommandEvent &event);
		void OnBrowseScreenShot(wxCommandEvent &event);
		void OnTakeScreenShot(wxCommandEvent &event);
		void OnChangeScreenshotFilename(wxCommandEvent & event);
		void UpdateScreenshotCounter();
		void FillRenderers (wxString renderers);
		void FillScalerAlgos (wxString scalerAlgos);
		void FillScalerFactors (wxString scalerFactors);
		void FillAccuracy(wxString accuracy);
		void SetRenderer (wxString value);
		void SetScalerAlgo (wxString value);
		void SetScalerFactor (wxString value);
		void SetAccuracy (wxString value);
		void SetDeinterlace (wxString value);
		void SetLimitSprites (wxString value);
		void SetSliderDefaults();
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus) {};
		void UpdateBlurControls (int value);
		void FillComboBox (wxString control, wxString contents);
		void setNewRenderersAndScalers();
		
	private:
		int FindFirstFreeScreenshotFile (wxString prefix);
		openMSXController * m_controller;
		
		wxComboBox * m_accuracyList;
		wxComboBox * m_rendererList;
		wxComboBox * m_scalerAlgoList;
		wxComboBox * m_scalerFactorList;
		
		wxBitmapButton * m_browseScreenshotButton;
		wxButton * m_defaultGammaButton;
		wxButton * m_defaultBlurButton;
		wxButton * m_defaultGlowButton;
		wxButton * m_defaultScanlineButton;
		wxButton * m_screenShotButton;
	
		wxStaticText * m_rendererLabel;
		wxStaticText * m_scalerLabel;
		wxStaticText * m_accuracyLabel;
		wxStaticText * m_deinterlaceLabel;
		wxStaticText * m_limitSpritesLabel;
		wxStaticText * m_fullScreenLabel;
		wxStaticText * m_scanLineLabel;
		wxStaticText * m_blurLabel;
		wxStaticText * m_glowLabel;
		wxStaticText * m_gammaLabel;
		wxStaticText * m_timesLabel;

		wxToggleButton * m_limitSpritesButton;
		wxToggleButton * m_deinterlaceButton;
		wxToggleButton * m_fullscreenButton;

		wxSlider * m_blurSlider;
		wxSlider * m_glowSlider;
		wxSlider * m_gammaSlider;
		wxSlider * m_scanlineSlider;

		wxTextCtrl * m_blurIndicator;
		wxTextCtrl * m_glowIndicator;
		wxTextCtrl * m_gammaIndicator;
		wxTextCtrl * m_scanlineIndicator;
		wxTextCtrl * m_screenShotFile;
		wxTextCtrl * m_screenShotCounter;
		
		wxString m_defaultBlur;
		wxString m_defaultGlow;
		wxString m_defaultGamma;
		wxString m_defaultScanline;

		DECLARE_CLASS(VideoControlPage)
			// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_VIDEOCONTROLPAGE_H__268A514C_6305_4E87_BB95_5D2F5458DF92__INCLUDED_)
