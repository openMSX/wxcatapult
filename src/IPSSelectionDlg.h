#ifndef IPSSELECTIONDLG_H
#define IPSSELECTIONDLG_H

#include <wx/dialog.h>

class wxListBox;
class wxButton;

class IPSSelectionDlg : public wxDialog
{
public:
	IPSSelectionDlg(wxWindow* parent);

	wxArrayString GetIPSList() const;
	wxString GetLastBrowseLocation();
	int Present(const wxArrayString& patches, const wxString& targetDir);

private:
	void CheckSelections();
	void OnCancel   (wxCommandEvent& event);
	void OnOk       (wxCommandEvent& event);
	void OnAddIPS   (wxCommandEvent& event);
	void OnRemoveIPS(wxCommandEvent& event);
	void OnSelectIPS(wxCommandEvent& event);
	void OnMoveUp   (wxCommandEvent& event);
	void OnMoveDown (wxCommandEvent& event);

	wxListBox* m_ipsDisplay;
	wxString m_lastBrowseLocation;
	wxButton* m_ipsRemoveButton;
	wxButton* m_ipsMoveUpButton;
	wxButton* m_ipsMoveDownButton;

	DECLARE_CLASS(IPSSelectionDlg)
	DECLARE_EVENT_TABLE()
};

#endif
