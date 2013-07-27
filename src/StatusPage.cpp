#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
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
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("StatusPage"));
	m_outputtext = (wxTextCtrl *)FindWindowByName(wxT("OutputText"));
}

StatusPage::~StatusPage()
{
}
