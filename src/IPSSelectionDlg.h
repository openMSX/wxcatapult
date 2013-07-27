#ifndef IPSSELECTIONDLG_H
#define IPSSELECTIONDLG_H

class IPSSelectionDlg : public wxDialog
{
public:
	IPSSelectionDlg(wxWindow * parent = NULL);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnAddIPS(wxCommandEvent &event);
	void OnRemoveIPS(wxCommandEvent & event);
	void OnSelectIPS(wxCommandEvent & event);
	void OnMoveUp(wxCommandEvent & event);
	void OnMoveDown(wxCommandEvent & event);
	virtual ~IPSSelectionDlg();
	void GetIPSList(wxArrayString & ipsList);
	wxString GetLastBrowseLocation ();
	int ShowModal(wxArrayString & patches,wxString targetDir);
	void CheckSelections();

private:
	wxListBox * m_ipsDisplay;
	wxArrayString m_ipsList;
	wxString m_lastBrowseLocation;
	wxButton * m_ipsRemoveButton;
	wxButton * m_ipsMoveUpButton;
	wxButton * m_ipsMoveDownButton;
	DECLARE_CLASS(IPSSelectionDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
