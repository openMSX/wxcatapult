// $Id: StatusPage.cpp,v 1.3 2004/02/05 20:02:35 h_oudejans Exp $
// StatusPage.cpp: implementation of the StatusPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxCatapultApp.h"
#include "StatusPage.h"

	IMPLEMENT_CLASS(StatusPage, wxPanel)
BEGIN_EVENT_TABLE(StatusPage, wxPanel)

END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

StatusPage::StatusPage(wxWindow * parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("StatusPage"));

	m_outputtext = (wxTextCtrl *)FindWindow(_("OutputText"));
	m_powerLed = (wxStaticBitmap *)FindWindow(_("PowerLed"));
	m_capsLed = (wxStaticBitmap *)FindWindow(_("CapsLockLed"));
	m_kanaLed = (wxStaticBitmap *)FindWindow(_("KanaLed"));
	m_pauseLed = (wxStaticBitmap *)FindWindow(_("PauseLed"));
	m_turboLed = (wxStaticBitmap *)FindWindow(_("TurboLed"));
	m_fddLed = (wxStaticBitmap *)FindWindow(_("FDDLed"));
}

StatusPage::~StatusPage()
{
}

void StatusPage::UpdateLed(wxString ledname, wxString ledstate)
{
	wxString resourceDir = ((wxCatapultApp &)wxGetApp()).GetResourceDir();
	wxStaticBitmap * led = NULL;
	if (ledname == _("power")) led = m_powerLed;
	if (ledname == _("caps")) led = m_capsLed;
	if (ledname == _("kana")) led = m_kanaLed;
	if (ledname == _("pause")) led = m_pauseLed;
	if (ledname == _("turbo")) led = m_turboLed;
	if (ledname == _("fdd")) led = m_fddLed;

	if (ledstate == _("off")) led->SetBitmap(wxBitmap(resourceDir + _("/bitmaps/ledoff.bmp"),wxBITMAP_TYPE_BMP));
	if (ledstate == _("on")) led->SetBitmap(wxBitmap(resourceDir + _("/bitmaps/ledon.bmp"),wxBITMAP_TYPE_BMP));
}
