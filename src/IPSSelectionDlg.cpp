#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "IPSSelectionDlg.h"

IMPLEMENT_CLASS(IPSSelectionDlg, wxDialog)
BEGIN_EVENT_TABLE(IPSSelectionDlg, wxDialog)
	EVT_BUTTON(XRCID("IPSOkButton"),IPSSelectionDlg::OnOk)
	EVT_BUTTON(XRCID("IPSCancelButton"),IPSSelectionDlg::OnCancel)
	EVT_BUTTON(XRCID("AddIPSButton"),IPSSelectionDlg::OnAddIPS)
	EVT_BUTTON(XRCID("RemoveIPSButton"),IPSSelectionDlg::OnRemoveIPS)
	EVT_BUTTON(XRCID("MoveIPSUpButton"),IPSSelectionDlg::OnMoveUp)
	EVT_BUTTON(XRCID("MoveIPSDownButton"),IPSSelectionDlg::OnMoveDown)
	EVT_LISTBOX(XRCID("IPSList"),IPSSelectionDlg::OnSelectIPS)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IPSSelectionDlg::IPSSelectionDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("IPSSelectionDialog"));
	m_ipsDisplay = (wxListBox *)FindWindowByName(wxT("IPSList"));
	m_ipsRemoveButton = (wxButton *)FindWindowByName(wxT("RemoveIPSButton"));
	m_ipsMoveDownButton = (wxButton *)FindWindowByName(wxT("MoveIPSDownButton"));
	m_ipsMoveUpButton = (wxButton *)FindWindowByName(wxT("MoveIPSUpButton"));
	m_ipsDisplay->Clear();
	m_lastBrowseLocation = wxT("");
	m_ipsMoveUpButton->Enable(false);
	m_ipsMoveDownButton->Enable(false);
	m_ipsRemoveButton->Enable(false);
}

IPSSelectionDlg::~IPSSelectionDlg()
{

}

int IPSSelectionDlg::ShowModal(wxArrayString & patches, wxString targetDir)
{
	m_lastBrowseLocation = targetDir;
	m_ipsDisplay->Clear();
	int count = patches.GetCount();
	if (count != 0)
	{
		for (int i=0;i<count;i++)
		{
			m_ipsDisplay->Append(patches[i]);
		}
	}
	wxFont myFont = m_ipsDisplay->GetFont();
	wxMemoryDC * tempDC= new wxMemoryDC();
	tempDC->SetFont(myFont);
	int index;
	int w,h,wMax=392;
	int items = m_ipsDisplay->GetCount();
		for (index=0;index<items;index++){
			tempDC->GetTextExtent(wxString(m_ipsDisplay->GetString(index) + wxT("W")),&w,&h);
			if (w > wMax){
				wMax = w;
			}
		}
	m_ipsDisplay->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X),118);
	this->Fit();
	return wxDialog::ShowModal();
}

void IPSSelectionDlg::OnCancel(wxCommandEvent &event)
{
	EndModal(wxID_CANCEL);
}

void IPSSelectionDlg::OnOk(wxCommandEvent &event)
{
	EndModal (wxID_OK);
}

void IPSSelectionDlg::OnAddIPS(wxCommandEvent &event)
{
	wxString path;
	wxString result;
#ifndef __MOTIF__
	path = wxT("All known patchfiles|*.ips;*.IPS;*.gz;*.GZ;*.zip;*.ZIP|Uncompressed patchfiles (*.ips)|*.ips;*.IPS|Compressed patchfiles|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif
	wxFileDialog filedlg(this,wxT("Select ips patchfile"), m_lastBrowseLocation, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK){
		result = filedlg.GetPath();
		m_ipsDisplay->Append(result);
		m_lastBrowseLocation = ::wxPathOnly(result);
		CheckSelections();
	}
}

void IPSSelectionDlg::OnRemoveIPS(wxCommandEvent & event)
{
	wxArrayInt selections;
	int num;
	num = m_ipsDisplay->GetSelections(selections);
	if (num != 0)
	{
		for (int i=num-1;i>=0;i--){
			m_ipsDisplay->Delete(selections[i]);
		}
	}
	CheckSelections();
}

void IPSSelectionDlg::OnMoveUp(wxCommandEvent & event)
{
	wxString temp;
	wxArrayInt selections;
	int num;
	num = m_ipsDisplay->GetSelections(selections);
	if (num != 0)
	{
		for (int i=0;i<num;i++){
			temp = m_ipsDisplay->GetString(selections[i]);
			m_ipsDisplay->Delete(selections[i]);
			m_ipsDisplay->InsertItems(1,&temp,selections[i]-1);
			m_ipsDisplay->SetSelection(selections[i]-1,true);
		}
	}
	CheckSelections();
}

void IPSSelectionDlg::OnMoveDown(wxCommandEvent & event)
{
	wxArrayInt selections;
	int num = m_ipsDisplay->GetSelections(selections);
	for (int i = num - 1; i >= 0; i--) {
		wxString temp = m_ipsDisplay->GetString(selections[i]);
		m_ipsDisplay->Delete(selections[i]);
		m_ipsDisplay->InsertItems(1, &temp, selections[i] + 1);
		m_ipsDisplay->SetSelection(selections[i] + 1, true);
	}
	CheckSelections();
}

void IPSSelectionDlg::OnSelectIPS(wxCommandEvent & event)
{
	CheckSelections();
}

void IPSSelectionDlg::GetIPSList(wxArrayString & ipsList)
{
	ipsList.Clear();
	int count = m_ipsDisplay->GetCount();
	for (int i = 0; i < count; i++) {
		ipsList.Add(m_ipsDisplay->GetString(i));
	}
}

wxString IPSSelectionDlg::GetLastBrowseLocation()
{
	return m_lastBrowseLocation;
}

void IPSSelectionDlg::CheckSelections()
{
	wxArrayInt selections;
	int num;
	num = m_ipsDisplay->GetSelections(selections);
	if (num == 0) {
		m_ipsRemoveButton->Enable(false);
		m_ipsMoveUpButton->Enable(false);
		m_ipsMoveDownButton->Enable(false);
	} else {
		m_ipsRemoveButton->Enable(true);
		m_ipsMoveUpButton->Enable(selections[0] != 0);
		m_ipsMoveDownButton->Enable(
			selections[num - 1] != int(m_ipsDisplay->GetCount()) - 1
			);
	}
}
