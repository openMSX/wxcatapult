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

void StatusPage::UpdateLed(wxString msg)
{
	wxString baseDir = ::wxPathOnly(::wxGetApp().argv[0]);
	wxStaticBitmap * led = NULL;
	if (msg.Find(_("Power ")) != -1) led = m_powerLed;
	if (msg.Find(_("Caps ")) != -1) led = m_capsLed;
	if (msg.Find(_("Kana ")) != -1) led = m_kanaLed;
	if (msg.Find(_("Pause ")) != -1) led = m_pauseLed;
	if (msg.Find(_("Turbo ")) != -1) led = m_turboLed;
	if (msg.Find(_("FDD ")) != -1) led = m_fddLed;

	if (msg.Find(_(" LED OFF")) != -1) led->SetBitmap(wxBitmap(baseDir + _("/../resources/bitmaps/ledoff.bmp"),wxBITMAP_TYPE_BMP));
	if (msg.Find(_(" LED ON")) != -1) led->SetBitmap(wxBitmap(baseDir + _("/../resources/bitmaps/ledon.bmp"),wxBITMAP_TYPE_BMP));
}
