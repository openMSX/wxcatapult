#include "VideoControlPage.h"
#include "StatusPage.h"
#include "wxCatapultFrm.h"
#include "FullScreenDlg.h"
#include "ConfigurationData.h"
#include "openMSXController.h"
#include "utils.h"
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/filedlg.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/filefn.h>

IMPLEMENT_CLASS(VideoControlPage, wxPanel)
BEGIN_EVENT_TABLE(VideoControlPage, wxPanel)
	EVT_COMBOBOX(      XRCID("RendererSelector"),       CatapultPage::OnClickCombo)
	EVT_COMBOBOX(      XRCID("ScalerAlgoSelector"),     CatapultPage::OnClickCombo)
	EVT_COMBOBOX(      XRCID("ScalerFactorSelector"),   CatapultPage::OnClickCombo)
	EVT_COMBOBOX(      XRCID("VideoSourceSelector"),    CatapultPage::OnClickCombo)
	EVT_TEXT(          XRCID("RendererSelector"),       VideoControlPage::OnChangeRenderer)
	EVT_TEXT(          XRCID("ScalerAlgoSelector"),     VideoControlPage::OnChangeScalerAlgo)
	EVT_TEXT(          XRCID("ScalerFactorSelector"),   VideoControlPage::OnChangeScalerFactor)
	EVT_TEXT(          XRCID("VideoSourceSelector"),    VideoControlPage::OnChangeVideoSource)
	EVT_TOGGLEBUTTON(  XRCID("VSyncButton"),            VideoControlPage::OnVSync)
	EVT_TOGGLEBUTTON(  XRCID("DeInterlaceButton"),      VideoControlPage::OnDeInterlace)
	EVT_TOGGLEBUTTON(  XRCID("LimitSpriteButton"),      VideoControlPage::OnLimitSprites)
	EVT_TOGGLEBUTTON(  XRCID("DisableSpritesButton"),   VideoControlPage::OnDisableSprites)
	EVT_TOGGLEBUTTON(  XRCID("FullScreenButton"),       VideoControlPage::OnFullScreen)
	EVT_COMMAND_SCROLL(XRCID("BlurSlider"),             VideoControlPage::OnChangeBlur)
	EVT_COMMAND_SCROLL(XRCID("GlowSlider"),             VideoControlPage::OnChangeGlow)
	EVT_COMMAND_SCROLL(XRCID("GammaSlider"),            VideoControlPage::OnChangeGamma)
	EVT_COMMAND_SCROLL(XRCID("NoiseSlider"),            VideoControlPage::OnChangeNoise)
	EVT_COMMAND_SCROLL(XRCID("ScanLineSlider"),         VideoControlPage::OnChangeScanlines)
	EVT_BUTTON(        XRCID("ZeroBlurButton"),         VideoControlPage::OnDefaultBlur)
	EVT_BUTTON(        XRCID("ZeroGlowButton"),         VideoControlPage::OnDefaultGlow)
	EVT_BUTTON(        XRCID("DefaultGammaButton"),     VideoControlPage::OnDefaultGamma)
	EVT_BUTTON(        XRCID("DefaultNoiseButton"),     VideoControlPage::OnDefaultNoise)
	EVT_BUTTON(        XRCID("ZeroScanlineButton"),     VideoControlPage::OnDefaultScanlines)
	EVT_TEXT(          XRCID("GammaIndicator"),         VideoControlPage::OnInputGamma)
	EVT_TEXT(          XRCID("NoiseIndicator"),         VideoControlPage::OnInputNoise)
	EVT_TEXT(          XRCID("BlurIndicator"),          VideoControlPage::OnInputBlur)
	EVT_TEXT(          XRCID("GlowIndicator"),          VideoControlPage::OnInputGlow)
	EVT_TEXT(          XRCID("ScanlineIndicator"),      VideoControlPage::OnInputScanline)
	EVT_BUTTON(        XRCID("BrowseScreenShotButton"), VideoControlPage::OnBrowseScreenShot)
	EVT_BUTTON(        XRCID("ScreenShotButton"),       VideoControlPage::OnTakeScreenShot)
	EVT_TEXT(          XRCID("ScreenShotFilename"),     VideoControlPage::OnChangeScreenshotFilename)
END_EVENT_TABLE()

VideoControlPage::VideoControlPage(wxWindow* parent, openMSXController& controller)
	: m_controller(controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("VideoControlPage"));
	m_rendererList     = (wxComboBox*)FindWindowByName(wxT("RendererSelector"));
	m_scalerAlgoList   = (wxComboBox*)FindWindowByName(wxT("ScalerAlgoSelector"));
	m_timesLabel       = (wxStaticText*)FindWindowByName(wxT("times"));
	m_scalerFactorList   = (wxComboBox*)FindWindowByName(wxT("ScalerFactorSelector"));
	m_vSyncButton        = (wxToggleButton*)FindWindowByName(wxT("VSyncButton"));
	m_videoSourceList    = (wxComboBox*)FindWindowByName(wxT("VideoSourceSelector"));
	m_deinterlaceButton  = (wxToggleButton*)FindWindowByName(wxT("DeInterlaceButton"));
	m_limitSpritesButton = (wxToggleButton*)FindWindowByName(wxT("LimitSpriteButton"));
	m_disableSpritesButton = (wxToggleButton*)FindWindowByName(wxT("DisableSpritesButton"));
	m_fullscreenButton   = (wxToggleButton*)FindWindowByName(wxT("FullScreenButton"));
	m_blurSlider     = (wxSlider*)FindWindowByName(wxT("BlurSlider"));
	m_blurSlider->SetTickFreq(5);
	m_glowSlider     = (wxSlider*)FindWindowByName(wxT("GlowSlider"));
	m_glowSlider->SetTickFreq(5);
	m_gammaSlider    = (wxSlider*)FindWindowByName(wxT("GammaSlider"));
	m_gammaSlider->SetTickFreq(25);
	m_noiseSlider    = (wxSlider*)FindWindowByName(wxT("NoiseSlider"));
	m_noiseSlider->SetTickFreq(25);
	m_scanlineSlider = (wxSlider*)FindWindowByName(wxT("ScanLineSlider"));
	m_scanlineSlider->SetTickFreq(5);
	m_blurIndicator     = (wxTextCtrl*)FindWindowByName(wxT("BlurIndicator"));
	m_glowIndicator     = (wxTextCtrl*)FindWindowByName(wxT("GlowIndicator"));
	m_gammaIndicator    = (wxTextCtrl*)FindWindowByName(wxT("GammaIndicator"));
	m_noiseIndicator    = (wxTextCtrl*)FindWindowByName(wxT("NoiseIndicator"));
	m_scanlineIndicator = (wxTextCtrl*)FindWindowByName(wxT("ScanlineIndicator"));
	m_defaultBlurButton = (wxButton*)FindWindowByName(wxT("ZeroBlurButton"));
	m_defaultGlowButton     = (wxButton*)FindWindowByName(wxT("ZeroGlowButton"));
	m_defaultGammaButton    = (wxButton*)FindWindowByName(wxT("DefaultGammaButton"));
	m_defaultNoiseButton    = (wxButton*)FindWindowByName(wxT("DefaultNoiseButton"));
	m_defaultScanlineButton = (wxButton*)FindWindowByName(wxT("ZeroScanlineButton"));
	m_screenShotFile    = (wxTextCtrl*)FindWindowByName(wxT("ScreenShotFilename"));
	m_screenShotCounter = (wxTextCtrl*)FindWindowByName(wxT("ScreenShotCounter"));
	m_screenShotButton = (wxButton*)FindWindowByName(wxT("ScreenShotButton"));

	m_rendererLabel      = (wxStaticText*)FindWindowByName(wxT("RendererLabel"));
	m_scalerLabel        = (wxStaticText*)FindWindowByName(wxT("ScalerLabel"));
	m_vSyncLabel         = (wxStaticText*)FindWindowByName(wxT("VSyncLabel"));
	m_videoSourceLabel   = (wxStaticText*)FindWindowByName(wxT("VideoSourceLabel"));
	m_deinterlaceLabel   = (wxStaticText*)FindWindowByName(wxT("DeInterlaceLabel"));
	m_limitSpritesLabel  = (wxStaticText*)FindWindowByName(wxT("LimitSpriteLabel"));
	m_disableSpritesLabel= (wxStaticText*)FindWindowByName(wxT("DisableSpritesLabel"));
	m_fullScreenLabel    = (wxStaticText*)FindWindowByName(wxT("FullScreenLabel"));
	m_scanLineLabel      = (wxStaticText*)FindWindowByName(wxT("ScanLineLabel"));
	m_blurLabel          = (wxStaticText*)FindWindowByName(wxT("BlurLabel"));
	m_glowLabel          = (wxStaticText*)FindWindowByName(wxT("GlowLabel"));
	m_gammaLabel         = (wxStaticText*)FindWindowByName(wxT("GammaLabel"));
	m_noiseLabel         = (wxStaticText*)FindWindowByName(wxT("NoiseLabel"));

	setNewRenderersAndScalers();
	wxCommandEvent event;
	OnChangeScreenshotFilename(event);
}

void VideoControlPage::OnChangeRenderer(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("set renderer ") + m_rendererList->GetValue());
}

void VideoControlPage::OnChangeScalerAlgo(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("set scale_algorithm ") + m_scalerAlgoList->GetValue().Lower());
}

void VideoControlPage::OnChangeScalerFactor(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("set scale_factor ") + m_scalerFactorList->GetValue().Lower());
}

void VideoControlPage::OnVSync(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	if (button->GetValue()) {
		m_controller.WriteCommand(wxT("set vsync on"));
		button->SetLabel(wxT("On"));
	} else {
		m_controller.WriteCommand(wxT("set vsync off"));
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnChangeVideoSource(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("set videosource ") + utils::tclEscapeWord(m_videoSourceList->GetValue().Lower()));
}

void VideoControlPage::OnDeInterlace(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	if (button->GetValue()) {
		m_controller.WriteCommand(wxT("set deinterlace on"));
		button->SetLabel(wxT("On"));
	} else {
		m_controller.WriteCommand(wxT("set deinterlace off"));
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnLimitSprites(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	if (button->GetValue()) {
		m_controller.WriteCommand(wxT("set limitsprites on"));
		button->SetLabel(wxT("On"));
	} else {
		m_controller.WriteCommand(wxT("set limitsprites off"));
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnDisableSprites(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	if (button->GetValue()) {
		m_controller.WriteCommand(wxT("set disablesprites on"));
		button->SetLabel(wxT("On"));
	} else {
		m_controller.WriteCommand(wxT("set disablesprites off"));
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnFullScreen(wxCommandEvent& event)
{
	auto* button = (wxToggleButton*)event.GetEventObject();
	FullScreenDlg dlg;
	dlg.Center();
	auto& config = ConfigurationData::instance();
	int notwarn;
	config.GetParameter(ConfigurationData::CD_FULLSCREENWARN, &notwarn);
	bool doIt = false;
	if (!notwarn) {
		if (dlg.ShowModal() == wxID_OK) {
			doIt = true;
		}
	} else {
		doIt = true;
	}

	if (doIt) {
		if (button->GetValue()) {
			m_controller.RaiseOpenMSX();
			m_controller.WriteCommand(wxT("set fullscreen on"));
			button->SetLabel(wxT("On"));
		} else {
			m_controller.WriteCommand(wxT("set fullscreen off"));
			button->SetLabel(wxT("Off"));
#ifdef __WXMSW__
			Sleep(500);
#endif
			m_controller.RestoreOpenMSX();
		}
	} else {
		button->SetValue(0);
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnChangeBlur(wxScrollEvent& event)
{
	auto text = wxString::Format(wxT("%d"), event.GetInt());
	m_blurIndicator->SetValue(text);
	m_controller.WriteCommand(wxT("set blur ") + text);
}

void VideoControlPage::OnChangeGlow(wxScrollEvent& event)
{
	auto text = wxString::Format(wxT("%d"), event.GetInt());
	m_glowIndicator->SetValue(text);
	m_controller.WriteCommand(wxT("set glow ") + text);
}

void VideoControlPage::OnChangeGamma(wxScrollEvent& event)
{
	auto text = wxString::Format(wxT("%.2f"), event.GetInt() / 100.0);
	m_gammaIndicator->SetValue(text);
	m_controller.WriteCommand(wxT("set gamma ") + text);
}

void VideoControlPage::OnChangeNoise(wxScrollEvent& event)
{
	auto text = wxString::Format(wxT("%.1f"), event.GetInt() / 10.0);
	m_noiseIndicator->SetValue(text);
	m_controller.WriteCommand(wxT("set noise ") + text);
}

void VideoControlPage::OnChangeScanlines(wxScrollEvent& event)
{
	auto text = wxString::Format(wxT("%d"), event.GetInt());
	m_scanlineIndicator->SetValue(text);
	m_controller.WriteCommand(wxT("set scanline ") + text);
}

 void VideoControlPage::OnDefaultBlur(wxCommandEvent& event)
{
	m_blurIndicator->SetValue(m_defaultBlur);
}

void VideoControlPage::OnDefaultGlow(wxCommandEvent& event)
{
	m_glowIndicator->SetValue(m_defaultGlow);
}

void VideoControlPage::OnDefaultGamma(wxCommandEvent& event)
{
	m_gammaIndicator->SetValue(m_defaultGamma);
}

void VideoControlPage::OnDefaultNoise(wxCommandEvent& event)
{
	m_noiseIndicator->SetValue(m_defaultNoise);
}

void VideoControlPage::OnDefaultScanlines(wxCommandEvent& event)
{
	m_scanlineIndicator->SetValue(m_defaultScanline);
}

void VideoControlPage::OnInputBlur(wxCommandEvent& event)
{
	wxString text = m_blurIndicator->GetValue();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 100) {
			num = 100;
			text = wxT("100");
			m_blurIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set blur ") + text);
		m_blurSlider->SetValue(num);
	}
}

void VideoControlPage::OnInputGlow(wxCommandEvent& event)
{
	wxString text = m_glowIndicator->GetValue();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 100) {
			num = 100;
			text = wxT("100");
			m_glowIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set glow ") + text);
		m_glowSlider->SetValue(num);
	}
}

void VideoControlPage::OnInputGamma(wxCommandEvent& event)
{
	wxString text = m_gammaIndicator->GetValue();
	double floatNumber;
	unsigned long num = 0;
	if (text.ToDouble(&floatNumber) && (text[text.Len() - 1] != '.')) {
		num = (int)(floatNumber * 100);
		if (num > 500) {
			num = 500;
			text = wxT("5.00");
			m_gammaIndicator->SetValue(text);
		}
		if (num >= 10) {
			m_controller.WriteCommand(wxT("set gamma ") + text);
			m_gammaSlider->SetValue(num);
		}
	}
}

void VideoControlPage::OnInputNoise(wxCommandEvent& event)
{
	wxString text = m_noiseIndicator->GetValue();
	double floatNumber;
	unsigned long num = 0;
	if (text.ToDouble(&floatNumber) && (text[text.Len() - 1] != '.')) {
		num = (int)(floatNumber * 10);
		if (num > 1000) {
			num = 1000;
			text = wxT("100.0");
			m_noiseIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set noise ") + text);
		m_noiseSlider->SetValue(num);
	}
}

void VideoControlPage::OnInputScanline(wxCommandEvent& event)
{
	wxString text = m_scanlineIndicator->GetValue();
	if (text.IsNumber()) {
		unsigned long num;
		text.ToULong(&num);
		if (num > 100) {
			num = 100;
			text = wxT("100");
			m_scanlineIndicator->SetValue(text);
		}
		m_controller.WriteCommand(wxT("set scanline ") + text);
		m_scanlineSlider->SetValue(num);
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
	m_noiseSlider->Enable(true);
	m_noiseIndicator->Enable(true);
	m_defaultNoiseButton->Enable(true);
	m_scanlineSlider->Enable(true);
	m_scanlineIndicator->Enable(true);
	m_defaultScanlineButton->Enable(true);
	m_rendererList->Enable(true);
	m_scalerAlgoList->Enable(true);
	m_timesLabel->Enable(true);
	m_scalerFactorList->Enable(true);
	m_vSyncButton->Enable(true);
	m_videoSourceList->Enable(true);
	m_deinterlaceButton->Enable(true);
	m_limitSpritesButton->Enable(true);
	m_disableSpritesButton->Enable(true);
	m_fullscreenButton->Enable(true);
	m_screenShotButton->Enable(true);
	m_rendererLabel->Enable(true);
	m_scalerLabel->Enable(true);
	m_vSyncLabel->Enable(true);
	m_videoSourceLabel->Enable(true);
	m_deinterlaceLabel->Enable(true);
	m_limitSpritesLabel->Enable(true);
	m_disableSpritesLabel->Enable(true);
	m_fullScreenLabel->Enable(true);
	m_scanLineLabel->Enable(true);
	m_blurLabel->Enable(true);
	m_glowLabel->Enable(true);
	m_gammaLabel->Enable(true);
	m_noiseLabel->Enable(true);
	if (auto* temp = FindWindowByLabel(wxT("Switches"))) {
		temp->Enable(true);
	}
	if (auto* temp = FindWindowByLabel(wxT("Monitor Simulation Controls"))) {
		temp->Enable(true);
	}
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
	m_noiseSlider->Enable(false);
	m_noiseIndicator->Enable(false);
	m_defaultNoiseButton->Enable(false);
	m_scanlineSlider->Enable(false);
	m_scanlineIndicator->Enable(false);
	m_defaultScanlineButton->Enable(false);
	m_rendererList->Enable(false);
	m_scalerAlgoList->Enable(false);
	m_timesLabel->Enable(false);
	m_scalerFactorList->Enable(false);
	m_vSyncButton->Enable(false);
	m_videoSourceList->Enable(false);
	m_deinterlaceButton->Enable(false);
	m_limitSpritesButton->Enable(false);
	m_disableSpritesButton->Enable(false);
	m_fullscreenButton->Enable(false);
	m_screenShotButton->Enable(false);
	m_rendererLabel->Enable(false);
	m_scalerLabel->Enable(false);
	m_vSyncLabel->Enable(false);
	m_videoSourceLabel->Enable(false);
	m_deinterlaceLabel->Enable(false);
	m_limitSpritesLabel->Enable(false);
	m_disableSpritesLabel->Enable(false);
	m_fullScreenLabel->Enable(false);
	m_scanLineLabel->Enable(false);
	m_blurLabel->Enable(false);
	m_glowLabel->Enable(false);
	m_gammaLabel->Enable(false);
	m_noiseLabel->Enable(false);
	if (auto* temp = FindWindowByLabel(wxT("Switches"))) {
		temp->Enable(false);
	}
	if (auto* temp = FindWindowByLabel(wxT("Monitor Simulation Controls"))) {
		temp->Enable(false);
	}
}

void VideoControlPage::SetSliderDefaults()
{
	m_defaultBlur = m_blurIndicator->GetValue();
	m_defaultGlow = m_glowIndicator->GetValue();
	m_defaultGamma = m_gammaIndicator->GetValue();
	m_defaultNoise = m_noiseIndicator->GetValue();
	m_defaultScanline = m_scanlineIndicator->GetValue();
}

void VideoControlPage::OnBrowseScreenShot(wxCommandEvent& event)
{
	int countersize = 0;
	wxString path = wxT("Screenshot Files(*.png)|*.png;*.PNG|All files|*.*||");
	wxFileDialog filedlg(this, wxT("Choose file to save screenshot to"), wxT(""), wxT(""), path, wxFD_SAVE);
	if (filedlg.ShowModal() == wxID_OK) {
		wxString screenshotpath = filedlg.GetPath();
		screenshotpath = screenshotpath.Left(screenshotpath.Length() - 4); // remove extension
		wxString screenshotfile = filedlg.GetFilename();
		screenshotfile = screenshotfile.Left(screenshotfile.Length() - 4); // idem

		while ((screenshotfile.Right(countersize + 1).IsNumber()) && (countersize < 4)) {
			++countersize;
		}
		wxString currentcounter = screenshotpath.Right(countersize);
		currentcounter.Prepend(wxT("0000"));
		currentcounter = currentcounter.Right(4);

		m_screenShotFile->SetValue(screenshotpath.Left(screenshotpath.Length() - countersize));
//		m_screenShotCounter->SetValue(currentcounter);
	}
}

void VideoControlPage::OnTakeScreenShot(wxCommandEvent& event)
{
	wxString screenshotfile = m_screenShotFile->GetValue();
	wxString counter = m_screenShotCounter->GetValue();
	wxString cmd = wxT("screenshot");
	if (!screenshotfile.IsEmpty()) {
		if (wxIsAbsolutePath(screenshotfile)) {
			cmd << wxT(" ") << utils::ConvertPathForCommand(screenshotfile + counter + wxT(".png"));
		} else {
			cmd << wxT(" -prefix ") << screenshotfile;
		}
	}
	m_controller.WriteCommand(cmd,
		[&](const wxString&, const wxString&) { UpdateScreenshotCounter(); });
}

static int FindFirstFreeScreenshotFile(wxString prefix)
{
	int counter = 0;
	bool found = false;
	do {
		auto count = wxString::Format(wxT("%04d"), ++counter);
		if (!wxFileExists(prefix + count + wxT(".png"))) {
			found = true;
		}
	} while ((counter < 9999) && !found);
	return counter;
}

void VideoControlPage::OnChangeScreenshotFilename(wxCommandEvent& event)
{
	m_screenShotCounter->Enable(false);
	if (m_screenShotFile->GetValue().IsEmpty()) {
		m_screenShotCounter->Clear();
	} else {
		if (wxIsAbsolutePath(m_screenShotFile->GetValue())) {
			m_screenShotCounter->Enable(true);
			UpdateScreenshotCounter();
		} else {
			m_screenShotCounter->Clear();
		}
	}
}

void VideoControlPage::UpdateScreenshotCounter()
{
	wxString prefix = m_screenShotFile->GetValue();
	if (!prefix.IsEmpty() && wxIsAbsolutePath(prefix)) {
		auto count = wxString::Format(
			wxT("%04d"), FindFirstFreeScreenshotFile(prefix));
		m_screenShotCounter->SetValue(count);
	}
}

void VideoControlPage::setNewRenderersAndScalers()
{
	m_rendererList->Clear();
	m_scalerAlgoList->Clear();
	m_scalerFactorList->Clear();
	m_videoSourceList->Clear();
}
