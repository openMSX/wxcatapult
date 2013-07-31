#include "RomTypeDlg.h"
#include <algorithm>
#include <wx/listbox.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_CLASS(RomTypeDlg, wxDialog)
BEGIN_EVENT_TABLE(RomTypeDlg, wxDialog)
	EVT_BUTTON(        XRCID("RomTypeOkButton"),     RomTypeDlg::OnOk)
	EVT_LISTBOX_DCLICK(XRCID("RomTypeList"),         RomTypeDlg::OnOk)
	EVT_BUTTON(        XRCID("RomTypeCancelButton"), RomTypeDlg::OnCancel)
END_EVENT_TABLE()

RomTypeDlg::RomTypeDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("RomTypeDialog"));
	m_romTypeList = (wxListBox*)FindWindowByName(wxT("RomTypeList"));
	m_romTypeList->Clear();
}

int RomTypeDlg::ShowModal(wxString type)
{
	wxMemoryDC tempDC;
	tempDC.SetFont(m_romTypeList->GetFont());
	int dx, dy;
	m_romTypeList->GetSize(&dx, &dy); //default size
	int wMax = dx;
	int items = m_romTypeList->GetCount();
	for (int i = 0; i < items; ++i) {
		int w, h;
		tempDC.GetTextExtent(m_romTypeList->GetString(i) + wxT("W"), &w, &h);
		wMax = std::max(wMax, w);
	}
	m_romTypeList->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X), 280);
	if (!type.IsEmpty()) {
		wxString fullName = ConvertRomType(type, false);
		int pos = m_romTypeList->FindString(fullName);
		if (pos != -1) {
			m_romTypeList->SetSelection(pos, true);
		}
	} else {
		m_romTypeList->SetSelection(0, true);
	}
	Fit();
	return wxDialog::ShowModal();
}

void RomTypeDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

void RomTypeDlg::OnOk(wxCommandEvent& event)
{
	wxString fullTypeName = m_romTypeList->GetStringSelection();
	m_selectedType = ConvertRomType(fullTypeName, true);
	EndModal(wxID_OK);
}

wxString RomTypeDlg::GetSelectedType()
{
	return m_selectedType;
}

void RomTypeDlg::AddRomType(wxString type)
{
	int index = FindRomType(type);
	if (index == -1) {
		m_romTypes.Add(type);
		m_romTypes.Add(wxT("_")); // empty name
	}
}

void RomTypeDlg::SetRomTypeFullName(wxString type, wxString name)
{
	int index = FindRomType(type);
	if (index != -1) {
		m_romTypes[index + 1] = name;
		if (m_romTypeList->FindString(name) == -1) {
			m_romTypeList->Append(name);
		}
	}
}

int RomTypeDlg::FindRomType(wxString type)
{
	int index = -1;
	size_t numberOfRomTypes = m_romTypes.GetCount();
	size_t counter = 0;
	while ((index == -1) && (counter < numberOfRomTypes)) {
		if (m_romTypes[counter] == type) {
			index = counter;
		}
		counter += 2;
	}
	return index;
}

int RomTypeDlg::FindRomFullTypeName(wxString name)
{
	int index = -1;
	size_t numberOfRomTypes = m_romTypes.GetCount();
	size_t counter = 1;
	while ((index == -1) && (counter < numberOfRomTypes)) {
		if (m_romTypes[counter] == name) {
			index = counter;
		}
		counter += 2;
	}
	return index;
}

wxString RomTypeDlg::ConvertRomType(wxString source, bool backwards)
{
	int index = backwards
	          ? FindRomFullTypeName(source) - 1
	          : FindRomType(source) + 1;
	return ((index >= 0) && (index < (int)m_romTypes.GetCount()))
		? m_romTypes[index]
		: wxString();
}
