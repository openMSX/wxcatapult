// $Id: VideoControlPage.cpp,v 1.29 2004/12/03 18:38:20 h_oudejans Exp $
// VideoControlPage.cpp: implementation of the VideoControlPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#ifdef __WXMSW__
#include "openMSXWindowsController.h"
#else
#include "openMSXController.h"
#endif

#include "StatusPage.h"
#include "VideoControlPage.h"
#include "wxCatapultFrm.h"
#include "FullScreenDlg.h"
#include "ConfigurationData.h"

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
	EVT_TOGGLEBUTTON(XRCID("FullScreenButton"),VideoControlPage::OnFullScreen)
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
	EVT_BUTTON(XRCID("BrowseScreenShotButton"),VideoControlPage::OnBrowseScreenShot)
	EVT_BUTTON(XRCID("ScreenShotButton"),VideoControlPage::OnTakeScreenShot)
	EVT_TEXT(XRCID("ScreenShotFilename"),VideoControlPage::OnChangeScreenshotFilename)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

VideoControlPage::VideoControlPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("VideoControlPage"));
	m_controller = controller;
	m_rendererList = (wxComboBox *)FindWindowByName(wxT("RendererSelector"));;
	m_scalerList = (wxComboBox *)FindWindowByName(wxT("ScalerSelector"));
	m_accuracyList = (wxComboBox*)FindWindowByName(wxT("AccuracySelector"));
	m_deinterlaceButton = (wxToggleButton*)FindWindowByName(wxT("DeInterlaceButton"));
	m_limitSpritesButton = (wxToggleButton*)FindWindowByName(wxT("LimitSpriteButton"));
	m_fullscreenButton = (wxToggleButton*)FindWindowByName(wxT("FullScreenButton"));
	m_blurSlider = (wxSlider*)FindWindowByName(wxT("BlurSlider"));
	m_blurSlider->SetTickFreq(5,1);
	m_glowSlider = (wxSlider*)FindWindowByName(wxT("GlowSlider"));
	m_glowSlider->SetTickFreq(5,1);
	m_gammaSlider = (wxSlider*)FindWindowByName(wxT("GammaSlider"));
	m_gammaSlider->SetTickFreq(25,1);
	m_scanlineSlider = (wxSlider*)FindWindowByName(wxT("ScanLineSlider"));
	m_scanlineSlider->SetTickFreq(5,1);
	m_blurIndicator = (wxTextCtrl*)FindWindowByName(wxT("BlurIndicator"));
	m_glowIndicator = (wxTextCtrl*)FindWindowByName(wxT("GlowIndicator"));
	m_gammaIndicator = (wxTextCtrl*)FindWindowByName(wxT("GammaIndicator"));
	m_scanlineIndicator =(wxTextCtrl*)FindWindowByName(wxT("ScanlineIndicator"));
	m_defaultBlurButton = (wxButton*)FindWindowByName(wxT("ZeroBlurButton"));
	m_defaultGlowButton = (wxButton*)FindWindowByName(wxT("ZeroGlowButton"));
	m_defaultGammaButton = (wxButton*)FindWindowByName(wxT("DefaultGammaButton"));
	m_defaultScanlineButton = (wxButton*)FindWindowByName(wxT("ZeroScanlineButton"));
	m_screenShotFile = (wxTextCtrl*)FindWindowByName(wxT("ScreenShotFilename"));
	m_screenShotCounter = (wxTextCtrl*)FindWindowByName(wxT("ScreenShotCounter"));
	m_screenShotButton = (wxButton*)FindWindowByName(wxT("ScreenShotButton"));
	m_browseScreenshotButton = (wxBitmapButton *)FindWindowByName(wxT("BrowseScreenShotButton"));
	m_defaultBlur = wxT("");
	m_defaultGlow = wxT("");
	m_defaultGamma = wxT("");
	m_defaultScanline = wxT("");
	
	m_rendererLabel = (wxStaticText *)FindWindowByName(wxT("RendererLabel"));
	m_scalerLabel = (wxStaticText *)FindWindowByName(wxT("ScalerLabel"));
	m_accuracyLabel = (wxStaticText *)FindWindowByName(wxT("AccuracyLabel"));
	m_deinterlaceLabel = (wxStaticText *)FindWindowByName(wxT("DeInterlaceLabel"));
	m_limitSpritesLabel = (wxStaticText *)FindWindowByName(wxT("LimitSpriteLabel"));
	m_fullScreenLabel = (wxStaticText *)FindWindowByName(wxT("FullScreenLabel"));
	m_scanLineLabel = (wxStaticText *)FindWindowByName(wxT("ScanLineLabel"));
	m_blurLabel = (wxStaticText *)FindWindowByName(wxT("BlurLabel"));
	m_glowLabel = (wxStaticText *)FindWindowByName(wxT("GlowLabel"));
	m_gammaLabel = (wxStaticText *)FindWindowByName(wxT("GammaLabel"));

	wxBitmap & tempBmp = m_browseScreenshotButton->GetBitmapLabel();
	tempBmp.SetMask(new wxMask(tempBmp,wxColour(255,0,255)));
	m_browseScreenshotButton->Enable(false);
	m_browseScreenshotButton->Enable(true);

}

VideoControlPage::~VideoControlPage()
{
}

void VideoControlPage::OnChangeRenderer(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(wxT("set renderer ") + m_rendererList->GetValue()));
}

void VideoControlPage::OnChangeScaler(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(wxT("set scaler ") + m_scalerList->GetValue().Lower()));
}

void VideoControlPage::OnChangeAccuracy(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxString(wxT("set accuracy ") + m_accuracyList->GetValue().Lower()));
} 

void VideoControlPage::OnDeInterlace(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (wxT("set deinterlace on"));
		button->SetLabel(wxT("On"));
	}
	else
	{
		m_controller->WriteCommand (wxT("set deinterlace off"));
		button->SetLabel(wxT("Off"));
	}			
}

void VideoControlPage::OnLimitSprites(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	if (button->GetValue())
	{
		m_controller->WriteCommand (wxT("set limitsprites on"));
		button->SetLabel(wxT("On"));
	}
	else
	{
		m_controller->WriteCommand (wxT("set limitsprites off"));
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnFullScreen(wxCommandEvent &event)
{
	wxToggleButton * button = (wxToggleButton *)event.m_eventObject;
	FullScreenDlg dlg;
	dlg.Center();
	ConfigurationData * config = ConfigurationData::instance();
	int notwarn;
	config->GetParameter(ConfigurationData::CD_FULLSCREENWARN,&notwarn);
	bool doIt = false;
	if (!notwarn) {
		if (dlg.ShowModal() == wxID_OK) {
			doIt = true;
		}
	}
	else{
		doIt = true;
	}
	
	if (doIt) {
		if (button->GetValue())
		{
#ifdef __WXMSW__
			((openMSXWindowsController *)m_controller)->RaiseOpenMSX();
#endif
			m_controller->WriteCommand (wxT("set fullscreen on"));
			button->SetLabel(wxT("On"));
		}
		else
		{
		
			m_controller->WriteCommand (wxT("set fullscreen off"));
			button->SetLabel(wxT("Off"));
#ifdef __WXMSW__
			Sleep(500);
			((openMSXWindowsController *)m_controller)->RestoreOpenMSX();
#endif
		}
	}
	else{
		button->SetValue(0);
		button->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::OnChangeBlur(wxScrollEvent &event)
{
	wxString text;
	text.sprintf(wxT("%ld"), event.GetInt());
	m_blurIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(wxT("set blur ")) + text);	
}

void VideoControlPage::OnChangeGlow(wxScrollEvent &event)
{
	wxString text;
	text.sprintf(wxT("%ld"), event.GetInt());
	m_glowIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(wxT("set glow ")) + text);
}

void VideoControlPage::OnChangeGamma(wxScrollEvent &event)
{
	wxString text;
	text.sprintf(wxT("%ld.%02ld"), event.GetInt() / 100, event.GetInt() % 100);
	m_gammaIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(wxT("set gamma ")) + text);
}

void VideoControlPage::OnChangeScanlines(wxScrollEvent &event)
{
	wxString text;
	text.sprintf(wxT("%ld"), event.GetInt());
	m_scanlineIndicator->SetValue(text);
	m_controller->WriteCommand (wxString(wxT("set scanline ")) + text);
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
		if (num > 100) {
			num = 100;
			text = wxT("100");
			m_blurIndicator->SetValue(text);
		}
		if (num >= 0) {
			m_controller->WriteCommand (wxString(wxT("set blur ")) + text);
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
			text = wxT("100");
			m_glowIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString(wxT("set glow ")) + text);	
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
			text = wxT("5.00");
			m_gammaIndicator->SetValue(text);
		}
		if (num >= 10)
		{
			m_controller->WriteCommand (wxString(wxT("set gamma ")) + text);
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
			text = wxT("100");
			m_scanlineIndicator->SetValue(text);
		}
		if (num >= 0)
		{
			m_controller->WriteCommand (wxString(wxT("set scanline ")) + text);
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
	m_fullscreenButton->Enable(true);
	m_screenShotButton->Enable(true);
	m_rendererLabel->Enable(true);
	m_scalerLabel->Enable(true);
	m_accuracyLabel->Enable(true);
	m_deinterlaceLabel->Enable(true);
	m_limitSpritesLabel->Enable(true);
	m_fullScreenLabel->Enable(true);
	m_scanLineLabel->Enable(true);
	m_blurLabel->Enable(true);
	m_glowLabel->Enable(true);
	m_gammaLabel->Enable(true);
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
	m_fullscreenButton->Enable(false);
	m_screenShotButton->Enable(false);
	m_rendererLabel->Enable(false);
	m_scalerLabel->Enable(false);
	m_accuracyLabel->Enable(false);
	m_deinterlaceLabel->Enable(false);
	m_limitSpritesLabel->Enable(false);
	m_fullScreenLabel->Enable(false);
	m_scanLineLabel->Enable(false);
	m_blurLabel->Enable(false);
	m_glowLabel->Enable(false);
	m_gammaLabel->Enable(false);
}

void VideoControlPage::FillRenderers(wxString renderers)
{
	FillComboBox (wxT("RendererSelector"), renderers);
}

void VideoControlPage::FillScalers(wxString scalers)
{
	FillComboBox (wxT("ScalerSelector"), scalers);
}

void VideoControlPage::FillAccuracy(wxString accuracy)
{
	FillComboBox (wxT("AccuracySelector"), accuracy);
}


void VideoControlPage::FillComboBox (wxString control, wxString contents)
{	
	wxComboBox * box = (wxComboBox *)FindWindowByName(control);
	int pos;
	box->Clear();
	wxString temp = contents;
	do
	{
		pos = temp.Find(wxT("\n"));
		if (pos != -1)
		{
			box->Append(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) { // not everything parsed ?
		box->Append(temp);
	}
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
	if (value == wxT("on")) {
		m_deinterlaceButton->SetValue(true);
		m_deinterlaceButton->SetLabel(wxT("On"));
	}
	else {
		m_deinterlaceButton->SetValue(false);
		m_deinterlaceButton->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::SetLimitSprites(wxString value)
{
	if (value == wxT("on")) {
		m_limitSpritesButton->SetValue(true);
		m_limitSpritesButton->SetLabel(wxT("On"));
	}
	else {
		m_limitSpritesButton->SetValue(false);
		m_limitSpritesButton->SetLabel(wxT("Off"));
	}
}

void VideoControlPage::SetSliderDefaults()
{
	m_defaultBlur = m_blurIndicator->GetValue();
	m_defaultGlow = m_glowIndicator->GetValue();
	m_defaultGamma = m_gammaIndicator->GetValue();
	m_defaultScanline = m_scanlineIndicator->GetValue();
}

void VideoControlPage::OnBrowseScreenShot(wxCommandEvent &event)
{
	wxString screenshotpath;
	wxString screenshotfile;
	int countersize = 0;
	wxString path;
#ifndef __MOTIF__
	path = wxT("Screenshot Files(*.png)|*.png;*.PNG|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,wxT("Choose file to save screenshot to"),wxT(""), wxT(""), path ,wxSAVE);
	if (filedlg.ShowModal() == wxID_OK)
	{
		screenshotpath = filedlg.GetPath();
		screenshotpath = screenshotpath.Left(screenshotpath.Length()-4); // remove extension
		screenshotfile = filedlg.GetFilename();
		screenshotfile = screenshotfile.Left(screenshotfile.Length()-4); // idem
	
		while ((screenshotfile.Right(countersize+1).IsNumber()) && (countersize <4)){
			countersize++;			
		}
		wxString currentcounter = screenshotpath.Right(countersize);
		currentcounter.Prepend(wxT("0000"));
		currentcounter = currentcounter.Right(4);		
		
		m_screenShotFile->SetValue(screenshotpath.Left(screenshotpath.Length()-countersize));
//		m_screenShotCounter->SetValue(currentcounter);

	}	
}

void VideoControlPage::OnTakeScreenShot(wxCommandEvent &event)
{
	wxString screenshotfile = m_screenShotFile->GetValue();
	wxString counter = m_screenShotCounter->GetValue();
	if ((screenshotfile == wxT("")) && (counter == wxT(""))){
		m_controller->WriteCommand(wxT("screenshot"));
	}
	else{
		m_controller->WriteCommand(wxString(wxT("screenshot ")) + ConvertPath(screenshotfile + counter +wxT(".png"),true));
	}
}


int VideoControlPage::FindFirstFreeScreenshotFile(wxString prefix)
{
	int counter = 0;
	wxString countString;
	bool found = false;
	do{
		counter++;
		countString.sprintf(wxT("0000%d"),counter);
		countString = countString.Right(4);
		if (!wxFileExists(prefix + countString +wxT(".png"))){
			found = true;			
		}
	}while ((counter<9999) && (!found));
	return counter;
}

void VideoControlPage::OnChangeScreenshotFilename(wxCommandEvent & event)
{
	if (m_screenShotFile->GetValue() == wxT("")){
		m_screenShotCounter->Clear();
	}
	else{
		UpdateScreenshotCounter();
	}
}

void VideoControlPage::UpdateScreenshotCounter()
{
	wxString prefix = m_screenShotFile->GetValue();
	if (prefix != wxT("")){
		wxString countString;
		countString.sprintf(wxT("0000%d"),FindFirstFreeScreenshotFile(prefix));
		countString = countString.Right(4);
		m_screenShotCounter->SetValue(countString);
	}
}
