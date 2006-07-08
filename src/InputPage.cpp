// $Id: InputPage.cpp,v 1.13 2005/12/17 13:27:32 m9710797 Exp $
// InputPage.cpp: implementation of the InputPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxCatapultApp.h"
#include "InputPage.h"
#include "openMSXController.h"

IMPLEMENT_CLASS(InputPage, wxPanel)
BEGIN_EVENT_TABLE(InputPage, wxPanel)
	EVT_BUTTON(XRCID("TypeTextButton"),InputPage::OnTypeText)
	EVT_BUTTON(XRCID("ClearTextButton"),InputPage::OnClearText)
	EVT_TEXT(XRCID("InputText"),InputPage::OnTextChange)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

InputPage::InputPage(wxWindow * parent, openMSXController * controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("InputPage"));
	m_inputtext = (wxTextCtrl *)FindWindowByName(wxT("InputText"));
	m_typeTextButton = (wxButton *)FindWindowByName(wxT("TypeTextButton"));
	m_clearTextButton = (wxButton *)FindWindowByName(wxT("ClearTextButton"));
	m_controller = controller;

	m_typeTextButton->Enable(false);
	m_clearTextButton->Enable(false);

	launched = false;
}

InputPage::~InputPage()
{
}

void InputPage::OnTypeText (wxCommandEvent &event)
{
	wxString test = m_inputtext->GetValue();
	test.Replace(wxT("\\"),wxT("\\\\"),true);
	test.Replace(wxT("\n"),wxT("\\r"),true);
	test.Replace(wxT("$"),wxT("\\$"),true);
	test.Replace(wxT("\""),wxT("\\\""),true);
	test.Replace(wxT("["),wxT("\\["),true);
	test.Replace(wxT("]"),wxT("\\]"),true);
	test.Replace(wxT("}"),wxT("\\}"),true);
	test.Replace(wxT("{"),wxT("\\{"),true);

	// remove chars >= 128
	wxString tmp;
	for (size_t i = 0; i < test.length(); ++i) {
		signed char c = test[i];
		if (c > 0) {
			tmp += c;
		}
	}

	m_controller->WriteCommand(wxString(wxT("type \"")) + tmp + wxT("\""));
}

void InputPage::OnClearText(wxCommandEvent &event)
{
	m_inputtext->Clear();
}

void InputPage::OnTextChange(wxCommandEvent &event)
{
	m_clearTextButton->Enable(m_inputtext->GetValue().Length() != 0);
	m_typeTextButton->Enable(launched &&
				m_inputtext->GetValue().Length() != 0);
}

void InputPage::SetControlsOnEnd()
{
	launched=false;
	m_typeTextButton->Enable(false);
}

void InputPage::SetControlsOnLaunch()
{
	launched=true;
	m_typeTextButton->Enable(m_inputtext->GetValue().Length() != 0);
}
