// AboutDlg.cpp: implementation of the AboutDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "AboutDlg.h"

IMPLEMENT_CLASS(AboutDlg, wxDialog)
BEGIN_EVENT_TABLE(AboutDlg, wxDialog)
	EVT_BUTTON(XRCID("AboutCloseButton"),AboutDlg::OnClose)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AboutDlg::AboutDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("AboutDialog"));
}

AboutDlg::~AboutDlg()
{

}

void AboutDlg::OnClose(wxCommandEvent &event)
{
	EndModal (wxID_OK);
}
