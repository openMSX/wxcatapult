// $Id: VideoControlPage.h,v 1.10 2004/04/18 11:57:41 h_oudejans Exp $
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

class wxCatapultFrame;
class openMSXController;
class VideoControlPage : public CatapultPage 
{
	public:
		void SetControlsOnEnd();
		void SetControlsOnLaunch();
		VideoControlPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller = NULL);
		virtual ~VideoControlPage();
#ifdef __WINDOWS__		
		void RestoreNormalScreen();
#endif
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
		void OnChangeScaler (wxCommandEvent &event);
		void OnChangeRenderer(wxCommandEvent &event);
		void FillRenderers (wxString renderers);
		void FillScalers (wxString scalers);
		void FillAccuracy(wxString accuracy);
		void SetRenderer (wxString value);
		void SetScaler (wxString value);
		void SetAccuracy (wxString value);
		void SetDeinterlace (wxString value);
		void SetLimitSprites (wxString value);
		void SetSliderDefaults();
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus){};
		void UpdateBlurControls (int value);
		void FillComboBox (wxString control, wxString contents);
	private:
		openMSXController * m_controller;
		
		wxComboBox * m_accuracyList;
		wxComboBox * m_rendererList;
		wxComboBox * m_scalerList;
		
		wxButton * m_defaultGammaButton;
		wxButton * m_defaultBlurButton;
		wxButton * m_defaultGlowButton;
		wxButton * m_defaultScanlineButton;

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
		
		wxString m_defaultBlur;
		wxString m_defaultGlow;
		wxString m_defaultGamma;
		wxString m_defaultScanline;

		DECLARE_CLASS(VideoControlPage)
			// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_VIDEOCONTROLPAGE_H__268A514C_6305_4E87_BB95_5D2F5458DF92__INCLUDED_)
