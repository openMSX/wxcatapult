// RomTypeDlg.h: interface for the RomTypeDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROMTYPEDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_)
#define AFX_ROMTYPEDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RomTypeDlg : public wxDialog  
{
public:
	RomTypeDlg(wxWindow * parent = NULL);
	void OnCancel(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	virtual ~RomTypeDlg();
	wxString GetSelectedType();
	wxString GetSelectedFullTypeName();
	int FindRomType (wxString type);
	int FindRomFullTypeName (wxString name);
	void AddRomType (wxString type);
	void SetRomTypeFullName (wxString type, wxString name);
	wxString ConvertRomType (wxString source, bool backwards);

private:
	wxListBox * m_romTypeList;
	wxArrayString m_romTypes;
	wxString m_selectedType;
	wxString m_selectedFullTypeName;
	DECLARE_CLASS(RomTypeDlg)
	// any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_ROMTYPEDLG_H__09B17758_A791_4F43_AE15_88269673D1D2__INCLUDED_)
