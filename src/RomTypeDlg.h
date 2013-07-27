#ifndef ROMTYPEDLG_H
#define ROMTYPEDLG_H

class RomTypeDlg : public wxDialog
{
public:
	RomTypeDlg(wxWindow * parent = NULL);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	virtual ~RomTypeDlg();
	void SetSelectedType(wxString type);
	wxString GetSelectedType();
	wxString GetSelectedFullTypeName();
	int FindRomType (wxString type);
	int FindRomFullTypeName (wxString name);
	void AddRomType (wxString type);
	void SetRomTypeFullName (wxString type, wxString name);
	wxString ConvertRomType (wxString source, bool backwards);
	int ShowModal(wxString type);

private:
	wxListBox * m_romTypeList;
	wxArrayString m_romTypes;
	wxString m_selectedType;
	wxString m_selectedFullTypeName;
	DECLARE_CLASS(RomTypeDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif
