#include "CatapultPage.h"
#include <wx/combobox.h>

wxString CatapultPage::ConvertPath(wxString path, bool convertSlash, bool backwards)
{
	if (backwards) {
		path.Replace(wxT("\""), wxT(""));
	} else {
		path.Prepend(wxT("\""));
		path.Append(wxT("\""));
	}
	if (convertSlash) {
		if (backwards){
			path.Replace(wxT("/"), wxT("\\"));
		} else {
			path.Replace(wxT("\\"), wxT("/"));
		}
	}
	return path;
}

void CatapultPage::OnClickCombo(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString sel = box->GetString(box->GetSelection());
	if (sel != box->GetValue()) {
		box->SetValue(sel);
	}
}
