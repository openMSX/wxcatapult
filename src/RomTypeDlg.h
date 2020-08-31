#ifndef ROMTYPEDLG_H
#define ROMTYPEDLG_H

#include <wx/dialog.h>

class wxListBox;

class RomTypeDlg : public wxDialog
{
public:
	RomTypeDlg(wxWindow* parent);

	wxString GetSelectedType();
	void AddRomType(wxString type);
	void SetRomTypeFullName(wxString type, wxString name);
	int Present(wxString type);

private:
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	int FindRomType(wxString type);
	int FindRomFullTypeName(wxString name);
	wxString ConvertRomType(wxString source, bool backwards);

	wxListBox* m_romTypeList;
	wxArrayString m_romTypes;
	wxString m_selectedType;

	DECLARE_CLASS(RomTypeDlg)
	DECLARE_EVENT_TABLE()
};

#endif
