#include "InputPage.h"
#include "wxCatapultApp.h"
#include "openMSXController.h"
#include "utils.h"
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

InputPage::InputPage(wxWindow* parent, openMSXController& controller)
	: m_controller(controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("InputPage"));
	m_inputtext = (wxTextCtrl*)FindWindowByName(wxT("InputText"));
	m_typeTextButton  = (wxButton*)FindWindowByName(wxT("TypeTextButton"));
	m_clearTextButton = (wxButton*)FindWindowByName(wxT("ClearTextButton"));

	m_typeTextButton->Enable(false);
	m_clearTextButton->Enable(false);
}

void InputPage::OnTypeText(wxCommandEvent& event)
{
	wxString text = utils::tclAsEscapedStringWithQuotesAround(m_inputtext->GetValue());
	m_controller.WriteCommand(wxT("type ") + text);
}

void InputPage::OnClearText(wxCommandEvent& event)
{
	m_inputtext->Clear();
}

void InputPage::OnTextChange(wxCommandEvent& event)
{
	bool anyText = !m_inputtext->GetValue().IsEmpty();
	m_clearTextButton->Enable(anyText);
	m_typeTextButton->Enable(m_controller.IsOpenMSXRunning() && anyText);
}

void InputPage::SetControlsOnEnd()
{
	m_typeTextButton->Enable(false);
}

void InputPage::SetControlsOnLaunch()
{
	m_typeTextButton->Enable(!m_inputtext->GetValue().IsEmpty());
}
