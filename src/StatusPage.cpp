#include "StatusPage.h"
#include "wxCatapultApp.h"
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_CLASS(StatusPage, wxPanel)
BEGIN_EVENT_TABLE(StatusPage, wxPanel)
END_EVENT_TABLE()

StatusPage::StatusPage(wxWindow* parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("StatusPage"));
	m_outputtext = (wxTextCtrl*)FindWindowByName(wxT("OutputText"));
}

void StatusPage::Clear()
{
	m_outputtext->Clear();
}

void StatusPage::Add(const wxColour& col, const wxString& str)
{
	m_outputtext->SetDefaultStyle(wxTextAttr(
		col, wxNullColour,
		wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)));
	m_outputtext->AppendText(str);
}
