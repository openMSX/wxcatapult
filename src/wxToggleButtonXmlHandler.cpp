// $Id$
// wxToggleButtonXmlHandler.cpp: implementation of the wxToggleButtonXmlHandler class.
//
//////////////////////////////////////////////////////////////////////
#include <wx/tglbtn.h>
#include "wxToggleButtonXmlHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

wxToggleButtonXmlHandler::wxToggleButtonXmlHandler()
:wxXmlResourceHandler()
{
}

wxToggleButtonXmlHandler::~wxToggleButtonXmlHandler()
{
}

wxObject * wxToggleButtonXmlHandler::DoCreateResource()
{
	XRC_MAKE_INSTANCE(button, wxToggleButton)
		button->Create(m_parentAsWindow,
				GetID(),
				GetText(wxT("label")),
				GetPosition(), GetSize(),
				GetStyle(),
				wxDefaultValidator,
				GetName());

	SetupWindow(button);

	return button;
}

bool wxToggleButtonXmlHandler::CanHandle(wxXmlNode *node)
{
	return IsOfClass(node, wxT("wxToggleButton"));
}
