#include "AboutDlg.h"
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_CLASS(AboutDlg, wxDialog)
BEGIN_EVENT_TABLE(AboutDlg, wxDialog)
	EVT_BUTTON(XRCID("AboutCloseButton"), AboutDlg::OnClose)
END_EVENT_TABLE()

AboutDlg::AboutDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("AboutDialog"));
}

void AboutDlg::OnClose(wxCommandEvent& event)
{
	EndModal(wxID_OK);
}
