// $Id: InputPage.cpp,v 1.6 2004/12/01 20:05:58 h_oudejans Exp $
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
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

InputPage::InputPage(wxWindow * parent, openMSXController * controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("InputPage"));
	m_inputtext = (wxTextCtrl *)FindWindowByName(wxT("InputText"));
	m_typeTextButton = (wxButton *)FindWindowByName(wxT("TypeTextButton"));
	m_controller = controller;
}

InputPage::~InputPage()
{
}

void InputPage::OnTypeText (wxCommandEvent &event)
{
	wxString test = m_inputtext->GetValue();
	test.Replace("\\","\\\\",true);
	test.Replace("\n","\\r",true);
	test.Replace("$","\\$",true);
	test.Replace("\"","\\\"",true);
	test.Replace("[","\\[",true);
	test.Replace("]","\\]",true);
	test.Replace("}","\\}",true);
	test.Replace("{","\\{",true);
	m_controller->WriteCommand(wxString(wxT("type \"")) + test + wxT("\""));	
}

void InputPage::OnClearText(wxCommandEvent &event)
{
	m_inputtext->Clear();
}

void InputPage::SetControlsOnEnd()
{
	m_typeTextButton->Enable(false);
}

void InputPage::SetControlsOnLaunch()
{
	m_typeTextButton->Enable(true);
}


