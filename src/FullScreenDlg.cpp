// FullScreenDlg.cpp: implementation of the FullScreenDlg class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ConfigurationData.h"
#include "FullScreenDlg.h"

IMPLEMENT_CLASS(FullScreenDlg, wxDialog)
BEGIN_EVENT_TABLE(FullScreenDlg, wxDialog)
	EVT_BUTTON(XRCID("OkButton"),FullScreenDlg::OnOk)
	EVT_BUTTON(XRCID("CancelButton"),FullScreenDlg::OnCancel)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FullScreenDlg::FullScreenDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("FullScreenDialog"));
}

FullScreenDlg::~FullScreenDlg()
{

}

void FullScreenDlg::OnCancel(wxCommandEvent &event)
{
	EndModal(wxID_CANCEL);
}

void FullScreenDlg::OnOk(wxCommandEvent &event)
{
	ConfigurationData * config = ConfigurationData::instance();
	wxCheckBox * box = (wxCheckBox *)FindWindow(_("DoNotShow"));
	long notwarn = 0;
	if (box->IsChecked()){
		notwarn = 1;
	}
	config->SetParameter(ConfigurationData::CD_FULLSCREENWARN,notwarn);
	config->SaveData();
	EndModal (wxID_OK);
}

