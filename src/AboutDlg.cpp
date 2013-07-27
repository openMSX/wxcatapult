#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "AboutDlg.h"
#include <wx/image.h>

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
#if (wxCHECK_VERSION(2,6,0) && defined(__WXMSW__))
//	wxBitmapButton * aboutButton = (wxBitmapButton *)FindWindowByName(wxT("AboutButton"));
	wxStaticBitmap * aboutBitmap = (wxStaticBitmap *)FindWindowByName(wxT("AboutBitmap"));
//	wxBitmap bmp = aboutButton->GetBitmapLabel();
	wxBitmap bmp = aboutBitmap->GetBitmap();
	wxImage img = bmp.ConvertToImage();
	img.ConvertAlphaToMask(100);
	wxBitmap bmp2(img);
	aboutBitmap->SetBitmap(bmp2);
#endif
}

AboutDlg::~AboutDlg()
{

}

void AboutDlg::OnClose(wxCommandEvent &event)
{
	EndModal (wxID_OK);
}
