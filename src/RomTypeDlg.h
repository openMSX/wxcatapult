#ifndef ROMTYPEDLG_H
#define ROMTYPEDLG_H

#include <wx/dialog.h>

class wxListBox;

class RomTypeDlg : public wxDialog
{
public:
	RomTypeDlg(wxWindow* parent);

	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void SetSelectedType(wxString type);
	wxString GetSelectedType();
	wxString GetSelectedFullTypeName();
	int FindRomType(wxString type);
	int FindRomFullTypeName(wxString name);
	void AddRomType(wxString type);
	void SetRomTypeFullName(wxString type, wxString name);
	wxString ConvertRomType(wxString source, bool backwards);
	int ShowModal(wxString type);

private:
	wxListBox* m_romTypeList;
	wxArrayString m_romTypes;
	wxString m_selectedType;
	wxString m_selectedFullTypeName;

	DECLARE_CLASS(RomTypeDlg)
	DECLARE_EVENT_TABLE()
};

#endif
