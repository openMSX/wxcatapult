// $Id: VideoControlPage.cpp,v 1.10 2004/03/28 19:42:00 h_oudejans Exp $
// VideoControlPage.cpp: implementation of the VideoControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "openMSXController.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "wxCatapultFrm.h"

IMPLEMENT_CLASS(VideoControlPage, wxPanel)
BEGIN_EVENT_TABLE(VideoControlPage, wxPanel)
	EVT_COMBOBOX(XRCID("RendererSelector"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("ScalerSelector"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("AccuracySelector"),CatapultPage::OnClickCombo)
	EVT_TEXT(XRCID("RendererSelector"),VideoControlPage::OnChangeRenderer)
	EVT_TEXT(XRCID("ScalerSelector"),VideoControlPage::OnChangeScaler)
	EVT_TEXT(XRCID("AccuracySelector"),VideoControlPage::OnChangeAccuracy)
	EVT_TOGGLEBUTTON(XRCID("DeInterlaceButton"),VideoControlPage::OnDeInterlace)
	EVT_TOGGLEBUTTON(XRCID("LimitSpriteButton"),VideoControlPage::OnLimitSprites)
	EVT_COMMAND_SCROLL(XRCID("BlurSlider"),VideoControlPage::OnChangeBlur)
	EVT_COMMAND_SCROLL(XRCID("GlowSlider"), VideoControlPage::OnChangeGlow)
	EVT_COMMAND_SCROLL(XRCID("GammaSlider"), VideoControlPage::OnChangeGamma)
	EVT_COMMAND_SCROLL(XRCID("ScanLineSlider"), VideoControlPage::OnChangeScanlines)
	EVT_BUTTON(XRCID("ZeroBlurButton"), VideoControlPage::OnDefaultBlur)
	EVT_BUTTON(XRCID("ZeroGlowButton"), VideoControlPage::OnDefaultGlow)
	EVT_BUTTON(XRCID("DefaultGammaButton"), VideoControlPage::OnDefaultGamma)
	EVT_BUTTON(XRCID("ZeroScanlineButton"), VideoControlPage::OnDefaultScanlines)
	EVT_TEXT(XRCID("GammaIndicator"),VideoControlPage::OnInputGamma)
	EVT_TEXT(XRCID("BlurIndicator"),VideoControlPage::OnInputBlur)
	EVT_TEXT(XRCID("GlowIndicator"),VideoControlPage::OnInputGlow)
	EVT_TEXT(XRCID("ScanlineIndicator"),VideoControlPage::OnInputScanline)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

VideoControlPage::VideoControlPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("VideoControlPage"));
	m_controller = controller;
	m_rendererList = (wxComboBox *)FindWindow(_("RendererSelector"));;
	m_scalerList = (wxComboBox *)FindWindow(_("ScalerSelector"));
	m_accuracyList = (wxComboBox*)FindWindow(_("AccuracySelector"));
	m_deinterlaceButton = (wxToggleButton*)FindWindow(_("DeInterlaceButton"));
	m_limitSpritesButton = (wxToggleButton*)FindWindow(_("LimitSpriteButton"));
	m_blurSlider = (wxSlider*)FindWindow(_("BlurSlider"));
	m_blurSlider->SetTickFreq(5,1);
	m_glowSlider = (wxSlider*)FindWindow(_("GlowSlider"));
	m_glowSlider->SetTickFreq(5,1);
	m_gammaSlider = (wxSlider*)FindWindow(_("GammaSlider"));
	m_gammaSlider->SetTickFreq(25,1);
	m_scanlineSlider = (wxSlider*)FindWindow(_("ScanLineSlider"));
	m_scanlineSlider->SetTickFreq(5,1);
	m_blurIndicator = (wxTextCtrl*)FindWindow(_("BlurIndicator"));
	m_glowIndicator = (wxTextCtrl*)FindWindow(_("GlowIndicator"));
	m_gammaIndicator = (wxTextCtrl*)FindWindow(_("GammaIndicator"));
	m_scanlineIndicator =(wxTextCtrl*)FindWindow(_("ScanlineIndicator"));
	m_defaultBlurButton = (wxButton*)FindWindow(_("ZeroBlurButton"));
	m_defaultGlowButton = (wxButton*)FindWindow(_("ZeroGlowButton"));
	m_defaultGammaButton = (wxButton*)FindWindow(_("DefaultGammaButton"));
	m_defaultScanlineButton = (wxButton*)FindWindow(_("ZeroScanlineButton"));
	m_defaultBlur = "";
	m_defaultGlow = "";
	m_defaultGamma = "";
	m_defaultScanline = "";
}

VideoControlPage::~VideoControlPage()
{
}

void VideoControlPage::OnChangeRenderer(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(_("set renderer ") + m_rendererList->GetValue()));
}

void VideoControlPage::OnChangeScaler(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(_("set scaler ") + m_scalerList->GetValue().Lower()));
}

void VideoControlPage::OnChangeAccuracy(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(_("set accuracy ") + m_accuracyList->GetValue().Lower()));
} 

void VideoControlPage::OnDeInterlace(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (_("set deinterlace on"));
		button->SetLabel("On");
	}
	else
	{
		m_controller->WriteCommand (_("set deinterlace off"));
		button->SetLabel("Off");
	}			
}

void VideoControlPage::OnLimitSprites(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (_("set limitsprites on"));
		button->SetLabel("On");
	}
	else
	{
		m_controller->WriteCommand (_("set limitsprites off"));
		button->SetLabel("Off");
	}
}

void VideoControlPage::OnChangeBlur(wxScrollEvent &event)
{
	wxString text;
	text.sprintf("%ld", event.GetInt());
	m_blurIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(_("set blur ")) + text);	
}

void VideoControlPage::OnChangeGlow(wxScrollEvent &event)
{
	wxString text;
	text.sprintf("%ld", event.GetInt());
	m_glowIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(_("set glow ")) + text);
}

void VideoControlPage::OnChangeGamma(wxScrollEvent &event)
{
	wxString text;
	text.sprintf("%ld.%02ld", event.GetInt() / 100, event.GetInt() % 100);
	m_gammaIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(_("set gamma ")) + text);
}

void VideoControlPage::OnChangeScanlines(wxScrollEvent &event)
{
	wxString text;
	text.sprintf("%ld", event.GetInt());
	m_scanlineIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(_("set scanline ")) + text);
}

 void VideoControlPage::OnDefaultBlur(wxCommandEvent &event)
{
	m_blurIndicator->SetValue(m_defaultBlur);
}

void VideoControlPage::OnDefaultGlow(wxCommandEvent &event)
{
	m_glowIndicator->SetValue(m_defaultGlow);
}

void VideoControlPage::OnDefaultGamma(wxCommandEvent &event)
{
	m_gammaIndicator->SetValue(m_defaultGamma);
}

void VideoControlPage::OnDefaultScanlines(wxCommandEvent &event)
{
	m_scanlineIndicator->SetValue(m_defaultScanline);
}

void VideoControlPage::OnInputBlur(wxCommandEvent &event)
{
	wxString text = m_blurIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100){
			num = 100;
			text = _("100");
			m_blurIndicator->SetValue(text);
		}
		if (num >= 0){
			m_controller->WriteCommand (wxString(_("set blur ")) + text);
			m_blurSlider->SetValue(num);	
		}
	}
}

void VideoControlPage::OnInputGlow(wxCommandEvent &event)
{
	wxString text = m_glowIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100)
		{
			num = 100;
			text = _("100");
			m_glowIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString(_("set glow ")) + text);	
			m_glowSlider->SetValue(num);
		}
	}
}

void VideoControlPage::OnInputGamma(wxCommandEvent &event)
{
	wxString text = m_gammaIndicator->GetValue ();
	double floatNumber;
	unsigned long num = 0;

	if (text.ToDouble(&floatNumber) && (text[text.Len()-1] != '.'))
	{
		num = (int)(floatNumber * 100);
		if (num > 500)
		{
			num = 500;
			text = _("5.00");
			m_gammaIndicator->SetValue(text);
		}
		if (num >= 10)
		{
			m_controller->WriteCommand (wxString(_("set gamma ")) + text);
			m_gammaSlider->SetValue(num);
		}
	}
}

void VideoControlPage::OnInputScanline(wxCommandEvent &event)
{
	wxString text = m_scanlineIndicator->GetValue ();
	if (text.IsNumber())
	{
		unsigned long num;
		text.ToULong(&num);
		if (num > 100)
		{
			num = 100;
			text = _("100");
			m_scanlineIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString(_("set scanline ")) + text);
			m_scanlineSlider->SetValue(num);
		}
	}
}

void VideoControlPage::SetControlsOnLaunch()
{
	m_blurSlider->Enable(true);
	m_blurIndicator->Enable(true);
	m_defaultBlurButton->Enable(true);
	m_glowSlider->Enable(true);
	m_glowIndicator->Enable(true);
	m_defaultGlowButton->Enable(true);
	m_gammaSlider->Enable(true);
	m_gammaIndicator->Enable(true);
	m_defaultGammaButton->Enable(true);
	m_scanlineSlider->Enable(true);
	m_scanlineIndicator->Enable(true);
	m_defaultScanlineButton->Enable(true);
	m_rendererList->Enable(true);
	m_scalerList->Enable(true);
	m_accuracyList->Enable(true);
	m_deinterlaceButton->Enable(true);
	m_limitSpritesButton->Enable(true);
}

void VideoControlPage::SetControlsOnEnd()
{
	m_blurSlider->Enable(false);
	m_blurIndicator->Enable(false);
	m_defaultBlurButton->Enable(false);
	m_glowSlider->Enable(false);
	m_glowIndicator->Enable(false);
	m_defaultGlowButton->Enable(false);
	m_gammaSlider->Enable(false);
	m_gammaIndicator->Enable(false);
	m_defaultGammaButton->Enable(false);
	m_scanlineSlider->Enable(false);
	m_scanlineIndicator->Enable(false);
	m_defaultScanlineButton->Enable(false);
	m_rendererList->Enable(false);
	m_scalerList->Enable(false);
	m_accuracyList->Enable(false);
	m_deinterlaceButton->Enable(false);
	m_limitSpritesButton->Enable(false);
}

void VideoControlPage::FillRenderers(wxString renderers)
{
	FillComboBox (m_rendererList, renderers);
}

void VideoControlPage::FillScalers(wxString scalers)
{
	FillComboBox (m_scalerList, scalers);
}

void VideoControlPage::FillAccuracy(wxString accuracy)
{
	FillComboBox (m_accuracyList, accuracy);
}


void VideoControlPage::FillComboBox (wxComboBox * control, wxString contents)
{	
	int pos;
	control->Clear();
	wxString temp = contents;
	do
	{
		pos = temp.Find(_("\n"));
		if (pos != -1)
		{
			control->Append(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		control->Append(temp);
}

void VideoControlPage::SetRenderer (wxString value)
{
	m_rendererList->SetSelection(m_rendererList->FindString(value));
}

void VideoControlPage::SetScaler (wxString value)
{
	m_scalerList->SetSelection(m_scalerList->FindString(value));	
}

void VideoControlPage::SetAccuracy(wxString value)
{
	m_accuracyList->SetSelection(m_accuracyList->FindString(value));
}

void VideoControlPage::SetDeinterlace(wxString value)
{
	if (value == _("on")){
		m_deinterlaceButton->SetValue(true);
		m_deinterlaceButton->SetLabel(_("On"));
	}
	else {
		m_deinterlaceButton->SetValue(false);
		m_deinterlaceButton->SetLabel(_("Off"));
	}
}

void VideoControlPage::SetLimitSprites(wxString value)
{
	if (value == _("on")){
		m_limitSpritesButton->SetValue(true);
		m_limitSpritesButton->SetLabel(_("On"));
	}
	else {
		m_limitSpritesButton->SetValue(false);
		m_limitSpritesButton->SetLabel(_("Off"));
	}
}

void VideoControlPage::SetBlur(wxString value)
{
	if (m_defaultBlur == _("")){
		m_defaultBlur = value;
	}
	m_blurIndicator->SetValue(value);
}

void VideoControlPage::SetGlow(wxString value)
{
	if (m_defaultGlow == _("")){
		m_defaultGlow = value;
	}
	m_glowIndicator->SetValue(value);
}

void VideoControlPage::SetGamma(wxString value)
{
	if (m_defaultGamma == _("")){
		m_defaultGamma = value;
	}
	m_gammaIndicator->SetValue(value);
}

void VideoControlPage::SetScanline(wxString value)
{
	if (m_defaultScanline == _("")){
		m_defaultScanline = value;
	}
	m_scanlineIndicator->SetValue(value);
}
