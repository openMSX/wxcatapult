// $Id: InputPage.cpp,v 1.1 2004/10/01 16:49:46 h_oudejans Exp $
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
	m_inputtext = (wxTextCtrl *)FindWindow(wxT("InputText"));
	m_controller = controller;
}

InputPage::~InputPage()
{
}

void InputPage::OnTypeText (wxCommandEvent &event)
{
	wxString test = m_inputtext->GetValue();
	test.Replace("\n","\\r",true);
	test.Replace("$","\\$",true);
	test.Replace("\"","\\\"",true);
	test.Replace("[","\\[",true);
	test.Replace("]","\\]",true);
	test.Replace("}","\\}",true);
	test.Replace("{","\\{",true);
	m_controller->WriteCommand(wxString("type \"") + test + "\"");	
	
}

void InputPage::OnClearText(wxCommandEvent &event)
{
	m_inputtext->Clear();
}
