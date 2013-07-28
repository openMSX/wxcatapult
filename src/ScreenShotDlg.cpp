#include "ScreenShotDlg.h"
#include "ConfigurationData.h"
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

IMPLEMENT_CLASS(ScreenshotDlg, wxDialog)
BEGIN_EVENT_TABLE(ScreenshotDlg, wxDialog)
	EVT_BUTTON(XRCID("OkButton"), ScreenshotDlg::OnOk)
END_EVENT_TABLE()


ScreenshotDlg::ScreenshotDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("ScreenshotDialog"));
}

void ScreenshotDlg::OnOk(wxCommandEvent& event)
{
	auto& config = ConfigurationData::instance();
	auto* box = (wxCheckBox*)FindWindowByName(wxT("DoNotShow"));
	long notwarn = 0;
	if (box->IsChecked()) {
		notwarn = 1;
	}
	config.SetParameter(ConfigurationData::CD_SCREENSHOTINFO, notwarn);
	config.SaveData();
	EndModal(wxID_OK);
}
