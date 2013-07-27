#include "InputPage.h"
#include "wxCatapultApp.h"
#include "openMSXController.h"
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_CLASS(InputPage, wxPanel)
BEGIN_EVENT_TABLE(InputPage, wxPanel)
	EVT_BUTTON(XRCID("TypeTextButton"),  InputPage::OnTypeText)
	EVT_BUTTON(XRCID("ClearTextButton"), InputPage::OnClearText)
	EVT_TEXT(  XRCID("InputText"),       InputPage::OnTextChange)
END_EVENT_TABLE()

InputPage::InputPage(wxWindow* parent, openMSXController* controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("InputPage"));
	m_inputtext = (wxTextCtrl*)FindWindowByName(wxT("InputText"));
	m_typeTextButton  = (wxButton*)FindWindowByName(wxT("TypeTextButton"));
	m_clearTextButton = (wxButton*)FindWindowByName(wxT("ClearTextButton"));
	m_controller = controller;

	m_typeTextButton->Enable(false);
	m_clearTextButton->Enable(false);

	launched = false;
}

void InputPage::OnTypeText(wxCommandEvent& event)
{
	wxString test = m_inputtext->GetValue();
	test.Replace(wxT("\\"), wxT("\\\\"), true);
	test.Replace(wxT("\n"), wxT("\\r"),  true);
	test.Replace(wxT("$"),  wxT("\\$"),  true);
	test.Replace(wxT("\""), wxT("\\\""), true);
	test.Replace(wxT("["),  wxT("\\["),  true);
	test.Replace(wxT("]"),  wxT("\\]"),  true);
	test.Replace(wxT("}"),  wxT("\\}"),  true);
	test.Replace(wxT("{"),  wxT("\\{"),  true);
	m_controller->WriteCommand(wxString(wxT("type \"")) + test + wxT("\""));
}

void InputPage::OnClearText(wxCommandEvent& event)
{
	m_inputtext->Clear();
}

void InputPage::OnTextChange(wxCommandEvent& event)
{
	m_clearTextButton->Enable(m_inputtext->GetValue().Length() != 0);
	m_typeTextButton->Enable(launched &&
				m_inputtext->GetValue().Length() != 0);
}

void InputPage::SetControlsOnEnd()
{
	launched = false;
	m_typeTextButton->Enable(false);
}

void InputPage::SetControlsOnLaunch()
{
	launched = true;
	m_typeTextButton->Enable(m_inputtext->GetValue().Length() != 0);
}
