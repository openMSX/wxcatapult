#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "RomTypeDlg.h"

IMPLEMENT_CLASS(RomTypeDlg, wxDialog)
BEGIN_EVENT_TABLE(RomTypeDlg, wxDialog)
	EVT_BUTTON(XRCID("RomTypeOkButton"),RomTypeDlg::OnOk)
	EVT_LISTBOX_DCLICK(XRCID("RomTypeList"),RomTypeDlg::OnOk)
	EVT_BUTTON(XRCID("RomTypeCancelButton"),RomTypeDlg::OnCancel)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RomTypeDlg::RomTypeDlg(wxWindow * parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("RomTypeDialog"));
	m_romTypeList = (wxListBox *)FindWindowByName(wxT("RomTypeList"));
	m_romTypeList->Clear();
}

RomTypeDlg::~RomTypeDlg()
{

}

int RomTypeDlg::ShowModal(wxString type)
{
	int pos;
	wxFont myFont = m_romTypeList->GetFont();
	wxMemoryDC * tempDC= new wxMemoryDC();
	tempDC->SetFont(myFont);
	int dx,dy,index;
	m_romTypeList->GetSize(&dx,&dy); //default size
	int w,h,wMax=dx;
	int items = m_romTypeList->GetCount();
		for (index=0;index<items;index++){
			tempDC->GetTextExtent(wxString(m_romTypeList->GetString(index) + wxT("W")),&w,&h);
			if (w > wMax){
				wMax = w;
			}
		}
	m_romTypeList->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X),280);
	wxString fullName;
	if (!type.IsEmpty()){
		fullName = ConvertRomType(type,false);
		pos = m_romTypeList->FindString(fullName);
		if (pos != -1){
			m_romTypeList->SetSelection(pos,true);
		}
	}
	else{
		m_romTypeList->SetSelection(0,true);
	}
	this->Fit();
	return wxDialog::ShowModal();
}

void RomTypeDlg::OnCancel(wxCommandEvent &event)
{
	EndModal(wxID_CANCEL);
}

void RomTypeDlg::OnOk(wxCommandEvent &event)
{
	m_selectedFullTypeName = m_romTypeList->GetStringSelection();
	m_selectedType = ConvertRomType(m_selectedFullTypeName,true);
	EndModal (wxID_OK);
}

wxString RomTypeDlg::GetSelectedType()
{
	return wxString(m_selectedType);
}

wxString RomTypeDlg::GetSelectedFullTypeName()
{
	return wxString(m_selectedFullTypeName);
}

void RomTypeDlg::AddRomType(wxString type)
{
	int index = FindRomType(type);
	if (index == -1){
		m_romTypes.Add(type);
		m_romTypes.Add(wxT("_")); // empty name
	}
}

void RomTypeDlg::SetRomTypeFullName(wxString type, wxString name)
{
	int index = FindRomType(type);
	if (index != -1){
		m_romTypes[index+1] = name;
		if (m_romTypeList->FindString(name) == -1){
			m_romTypeList->Append(name);
		}
	}
}

int RomTypeDlg::FindRomType(wxString type)
{
	int index = -1;
	size_t numberOfRomTypes = m_romTypes.GetCount();
	size_t counter = 0;
	while ((index == -1) && (counter < numberOfRomTypes))
	{
		if (m_romTypes[counter] == type)
		{
			index = counter;
		}
		counter+=2;
	}
	return index;
}

int RomTypeDlg::FindRomFullTypeName(wxString name)
{
	int index = -1;
	size_t numberOfRomTypes = m_romTypes.GetCount();
	size_t counter = 1;
	while ((index == -1) && (counter < numberOfRomTypes))
	{
		if (m_romTypes[counter] == name)
		{
			index = counter;
		}
		counter+=2;
	}
	return index;
}

wxString RomTypeDlg::ConvertRomType(wxString source, bool backwards)
{
	wxString retVal=wxT("");
	int offset;
	int index;
	if (backwards){
		offset = -1;
		index = FindRomFullTypeName(source);
	}
	else{
		offset = 1;
		index = FindRomType(source);
	}
	index += offset;
	if ((index >=0) && (index <(int)m_romTypes.GetCount()))
	{
		retVal = m_romTypes[index];
	}
	return retVal;
}
