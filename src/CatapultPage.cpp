#include "CatapultPage.h"
#include <wx/combobox.h>

void CatapultPage::OnClickCombo(wxCommandEvent& event)
{
	auto* box = (wxComboBox*)event.GetEventObject();
	wxString sel = box->GetString(box->GetSelection());
	if (sel != box->GetValue()) {
		box->SetValue(sel);
	}
}
