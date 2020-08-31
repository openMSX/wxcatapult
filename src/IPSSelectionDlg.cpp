#include "IPSSelectionDlg.h"
#include <algorithm>
#include <wx/button.h>
#include <wx/dcmemory.h>
#include <wx/listbox.h>
#include <wx/filedlg.h>
#include <wx/settings.h>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>

IMPLEMENT_CLASS(IPSSelectionDlg, wxDialog)
BEGIN_EVENT_TABLE(IPSSelectionDlg, wxDialog)
	EVT_BUTTON( XRCID("IPSOkButton"),      IPSSelectionDlg::OnOk)
	EVT_BUTTON( XRCID("IPSCancelButton"),  IPSSelectionDlg::OnCancel)
	EVT_BUTTON( XRCID("AddIPSButton"),     IPSSelectionDlg::OnAddIPS)
	EVT_BUTTON( XRCID("RemoveIPSButton"),  IPSSelectionDlg::OnRemoveIPS)
	EVT_BUTTON( XRCID("MoveIPSUpButton"),  IPSSelectionDlg::OnMoveUp)
	EVT_BUTTON( XRCID("MoveIPSDownButton"),IPSSelectionDlg::OnMoveDown)
	EVT_LISTBOX(XRCID("IPSList"),          IPSSelectionDlg::OnSelectIPS)
END_EVENT_TABLE()

IPSSelectionDlg::IPSSelectionDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("IPSSelectionDialog"));
	m_ipsDisplay = (wxListBox*)FindWindowByName(wxT("IPSList"));
	m_ipsRemoveButton   = (wxButton*)FindWindowByName(wxT("RemoveIPSButton"));
	m_ipsMoveDownButton = (wxButton*)FindWindowByName(wxT("MoveIPSDownButton"));
	m_ipsMoveUpButton   = (wxButton*)FindWindowByName(wxT("MoveIPSUpButton"));
	m_ipsDisplay->Clear();
	m_ipsMoveUpButton  ->Enable(false);
	m_ipsMoveDownButton->Enable(false);
	m_ipsRemoveButton  ->Enable(false);
}

int IPSSelectionDlg::Present(const wxArrayString& patches, const wxString& targetDir)
{
	m_lastBrowseLocation = targetDir;
	m_ipsDisplay->Clear();
	for (auto& p : patches) {
		m_ipsDisplay->Append(p);
	}
	wxMemoryDC tempDC;
	tempDC.SetFont(m_ipsDisplay->GetFont());
	int wMax = 392;
	int items = m_ipsDisplay->GetCount();
	for (int index = 0; index < items; ++index) {
		int w, h;
		tempDC.GetTextExtent(m_ipsDisplay->GetString(index) + wxT("W"), &w, &h);
		wMax = std::max(wMax, w);
	}
	m_ipsDisplay->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X), 118);
	Fit();
	return ShowModal();
}

void IPSSelectionDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

void IPSSelectionDlg::OnOk(wxCommandEvent& event)
{
	EndModal (wxID_OK);
}

void IPSSelectionDlg::OnAddIPS(wxCommandEvent& event)
{
	wxString path = wxT("All known patchfiles|*.ips;*.IPS;*.gz;*.GZ;*.zip;*.ZIP|Uncompressed patchfiles (*.ips)|*.ips;*.IPS|Compressed patchfiles|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
	wxFileDialog filedlg(this, wxT("Select ips patchfile"), m_lastBrowseLocation,
	                     wxT(""), path, wxFD_OPEN | wxFD_MULTIPLE);
	if (filedlg.ShowModal() == wxID_OK) {
		wxArrayString results;
		filedlg.GetPaths(results);
		for (auto& path: results) {
			m_ipsDisplay->Append(path);
			m_lastBrowseLocation = ::wxPathOnly(path);
		}
		CheckSelections();
	}
}

void IPSSelectionDlg::OnRemoveIPS(wxCommandEvent& event)
{
	wxArrayInt selections;
	int num = m_ipsDisplay->GetSelections(selections);
	for (int i = num - 1; i >= 0; --i) {
		m_ipsDisplay->Delete(selections[i]);
	}
	CheckSelections();
}

void IPSSelectionDlg::OnMoveUp(wxCommandEvent& event)
{
	wxArrayInt selections;
	int num = m_ipsDisplay->GetSelections(selections);
	for (int i = 0; i < num; ++i) {
		wxString temp = m_ipsDisplay->GetString(selections[i]);
		m_ipsDisplay->Delete(selections[i]);
		m_ipsDisplay->InsertItems(1, &temp, selections[i] - 1);
		m_ipsDisplay->SetSelection(selections[i] - 1, true);
	}
	CheckSelections();
}

void IPSSelectionDlg::OnMoveDown(wxCommandEvent& event)
{
	wxArrayInt selections;
	int num = m_ipsDisplay->GetSelections(selections);
	for (int i = num - 1; i >= 0; --i) {
		wxString temp = m_ipsDisplay->GetString(selections[i]);
		m_ipsDisplay->Delete(selections[i]);
		m_ipsDisplay->InsertItems(1, &temp, selections[i] + 1);
		m_ipsDisplay->SetSelection(selections[i] + 1, true);
	}
	CheckSelections();
}

void IPSSelectionDlg::OnSelectIPS(wxCommandEvent& event)
{
	CheckSelections();
}

wxArrayString IPSSelectionDlg::GetIPSList() const
{
	wxArrayString result;
	for (size_t i = 0; i < m_ipsDisplay->GetCount(); ++i) {
		result.Add(m_ipsDisplay->GetString(i));
	}
	return result;
}

wxString IPSSelectionDlg::GetLastBrowseLocation()
{
	return m_lastBrowseLocation;
}

void IPSSelectionDlg::CheckSelections()
{
	wxArrayInt selections;
	int num = m_ipsDisplay->GetSelections(selections);
	if (num == 0) {
		m_ipsRemoveButton->Enable(false);
		m_ipsMoveUpButton->Enable(false);
		m_ipsMoveDownButton->Enable(false);
	} else {
		m_ipsRemoveButton->Enable(true);
		m_ipsMoveUpButton->Enable(selections[0] != 0);
		m_ipsMoveDownButton->Enable(
			selections[num - 1] != int(m_ipsDisplay->GetCount()) - 1);
	}
}
