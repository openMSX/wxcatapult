// $Id$
// SessionPage.cpp: implementation of the SessionPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/dir.h>
#include <wx/filedlg.h>

#include "SessionPage.h"
#include "ConfigurationData.h"
#include "wxCatapultFrm.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "MiscControlPage.h"
#include "VideoControlPage.h"

	IMPLEMENT_CLASS(SessionPage, wxPanel)
BEGIN_EVENT_TABLE(SessionPage, wxPanel)
	EVT_COMBOBOX(XRCID("DiskAContents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("DiskAContents"),SessionPage::OnChangeMedia)
	EVT_COMBOBOX(XRCID("DiskBContents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("DiskBContents"),SessionPage::OnChangeMedia)
	EVT_COMBOBOX(XRCID("CartAContents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("CartAContents"),SessionPage::OnChangeMedia)
	EVT_COMBOBOX(XRCID("CartBContents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("CartBContents"),SessionPage::OnChangeMedia)
	EVT_COMBOBOX(XRCID("Tape1Contents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("Tape1Contents"),SessionPage::OnChangeMedia)
	EVT_COMBOBOX(XRCID("Tape2Contents"),SessionPage::OnChangeMedia)
	EVT_TEXT(XRCID("Tape2Contents"),SessionPage::OnChangeMedia)
	EVT_BUTTON(XRCID("BrowseDiskA"),SessionPage::OnBrowseDiskA)
	EVT_BUTTON(XRCID("BrowseDiskB"),SessionPage::OnBrowseDiskB)
	EVT_BUTTON(XRCID("BrowseCartA"),SessionPage::OnBrowseCartA)
	EVT_BUTTON(XRCID("BrowseCartB"),SessionPage::OnBrowseCartB)
	EVT_BUTTON(XRCID("BrowseNormalTape"),SessionPage::OnBrowseNormalTape)
	EVT_BUTTON(XRCID("BrowseCasPatch"),SessionPage::OnBrowseCasPatch)
	EVT_BUTTON(XRCID("ClearDiskA"),SessionPage::OnClearDiskA)
	EVT_BUTTON(XRCID("ClearDiskB"),SessionPage::OnClearDiskB)
	EVT_BUTTON(XRCID("ClearCartA"),SessionPage::OnClearCartA)
	EVT_BUTTON(XRCID("ClearCartB"),SessionPage::OnClearCartB)
	EVT_BUTTON(XRCID("ClearNormalTape"),SessionPage::OnClearNormalTape)
	EVT_BUTTON(XRCID("ClearCasPatch"),SessionPage::OnClearCasPatch)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

SessionPage::SessionPage(wxWindow * parent)
{
	wxXmlResource::Get()->LoadPanel(this, parent, _("SessionPage"));

	m_parent = (wxCatapultFrame *)parent->GetParent()->GetParent();
	m_diskA = (wxComboBox *)FindWindow(_("DiskAContents"));
	m_diskB = (wxComboBox *)FindWindow(_("DiskBContents"));
	m_cartA = (wxComboBox *)FindWindow(_("CartAContents"));
	m_cartB = (wxComboBox *)FindWindow(_("CartBContents"));
	m_tape1 = (wxComboBox *)FindWindow(_("NormalTapeContents"));
	m_tape2 = (wxComboBox *)FindWindow(_("CasPatchContents"));
	m_extensionList = (wxListBox *)FindWindow(_("ExtensionList"));
	m_machineList = (wxComboBox *)FindWindow(_("MachineList"));
	m_browseDiskA = (wxBitmapButton *)FindWindow(_("BrowseDiskA"));
	m_browseDiskB = (wxBitmapButton *)FindWindow(_("BrowseDiskB"));
	m_browseCartA = (wxBitmapButton *)FindWindow(_("BrowseCartA"));
	m_browseCartB = (wxBitmapButton *)FindWindow(_("BrowseCartB"));
	m_browseTape1 = (wxBitmapButton *)FindWindow(_("BrowseNormalTape"));
	m_browseTape2 = (wxBitmapButton *)FindWindow(_("BrowseCasPatch"));
	m_clearDiskA = (wxBitmapButton *)FindWindow(_("ClearDiskA"));
	m_clearDiskB = (wxBitmapButton *)FindWindow(_("ClearDiskB"));
	m_clearCartA = (wxBitmapButton *)FindWindow(_("ClearCartA"));
	m_clearCartB = (wxBitmapButton *)FindWindow(_("ClearCartB"));
	m_clearTape1 = (wxBitmapButton *)FindWindow(_("ClearNormalTape"));
	m_clearTape2 = (wxBitmapButton *)FindWindow(_("ClearCasPatch"));

	ConfigurationData * config = ConfigurationData::instance();
	wxString sharepath;
	config->GetParameter(ConfigurationData::CD_SHAREPATH,sharepath);
	m_machineList->Clear();
	m_machineList->Append(_(" <default> "));
	m_extensionList->Clear();

	wxArrayString machineArray;
	wxArrayString extensionArray;
	machineArray.Clear();
	extensionArray.Clear();

	prepareExtensions (sharepath, extensionArray);
	prepareMachines (sharepath, machineArray);
#ifdef __UNIX__
	prepareExtensions (::wxGetHomeDir() + "/.openMSX/share/", extensionArray);
	prepareMachines (::wxGetHomeDir() + "/.openMSX/share/", machineArray);
#endif
	extensionArray.Sort(SessionPage::CompareCaseInsensitive);
	machineArray.Sort(SessionPage::CompareCaseInsensitive);
	fillExtensions (extensionArray);
	fillMachines (machineArray);

	// let's put a mask on all the bitmapbuttons

	wxBitmapButton * buttons[] = {m_browseDiskA, m_browseDiskB,	m_browseCartA,
		m_browseCartB,m_browseTape1,m_browseTape2,m_clearDiskA, m_clearDiskB,
		m_clearCartA, m_clearCartB, m_clearTape1, m_clearTape2};


		for (int i=0;i<12;i++)
		{
			wxBitmap & temp = buttons[i]->GetBitmapLabel();
			temp.SetMask(new wxMask(temp,wxColour(255,0,255)));
			buttons[i]->Enable(false);
			buttons[i]->Enable(true);
		}
}

SessionPage::~SessionPage()
{
}

void SessionPage::OnClearDiskA(wxCommandEvent &event)
{
	m_diskA->SetValue(_(""));
}

void SessionPage::OnClearDiskB(wxCommandEvent &event)
{
	m_diskB->SetValue(_(""));
}

void SessionPage::OnClearCartA(wxCommandEvent &event)
{
	m_cartA->SetValue(_(""));
}

void SessionPage::OnClearCartB(wxCommandEvent &event)
{
	m_cartB->SetValue(_(""));
}

void SessionPage::OnClearNormalTape(wxCommandEvent &event)
{
	m_tape1->SetValue(_(""));
}

void SessionPage::OnClearCasPatch(wxCommandEvent &event)
{
	m_tape2->SetValue(_(""));
}

void SessionPage::OnBrowseDiskA(wxCommandEvent &event)
{
	BrowseDisk (m_diskA,::wxPathOnly(m_diskA->GetValue()));	
}

void SessionPage::OnBrowseDiskB(wxCommandEvent &event)
{
	BrowseDisk (m_diskB,::wxPathOnly(m_diskA->GetValue()));
}

void SessionPage::BrowseDisk(wxComboBox *target, wxString defaultpath)
{
	wxString path;
#ifndef __MOTIF__
	path = _("All known disk files|*.dsk;*.xsa;*.zip;*.gz;*.di1;*.di2|Uncompressed disk files|*.dsk;*.xsa;*.di1;*.di2|Compressed files (*.zip;*.gz)|*.gz;*.zip|All files|*.*||");
#else
	path = _("*.*");
#endif

	wxFileDialog filedlg(this,_("Select Diskimage"), defaultpath, _(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		target->SetValue (filedlg.GetPath());	
	}
	CheckMedia();
}

void SessionPage::OnBrowseCartA(wxCommandEvent &event)
{
	BrowseCart (m_cartA,::wxPathOnly(m_cartA->GetValue()));
}

void SessionPage::OnBrowseCartB(wxCommandEvent &event)
{
	BrowseCart (m_cartB,::wxPathOnly(m_cartA->GetValue()));
}

void SessionPage::BrowseCart(wxComboBox *target, wxString defaultpath)
{
	wxString path;
#ifndef __MOTIF__
	path = _("All known cartridge files|*.rom;*.ri;*.zip;*.gz|Uncompressed cartridge files|*.rom;*.ri|Compressed files (*.zip;*.gz)|*.gz;*.zip|All files|*.*||");
#else
	path = _("*.*");
#endif

	wxFileDialog filedlg(this,_("Select Rom-image"), defaultpath, _(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		target->SetValue (filedlg.GetPath());	
	}
}

void SessionPage::OnBrowseNormalTape(wxCommandEvent &event)
{
	wxString defaultpath = ::wxPathOnly(m_tape1->GetValue());
	wxString path;
#ifndef __MOTIF__
	path = _("All known tape (sound) files|*.zip;*.gz;*.wav|Uncompressed tape (sound) files|*.wav|Compressed files (*.zip;*.gz)|*.gz;*.zip|All files|*.*||");
#else
	path = _("*.*");
#endif

	wxFileDialog filedlg(this,_("Select Tape-soundimage"), defaultpath, _(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_tape1->SetValue (filedlg.GetPath());	
	}
	CheckMedia();
}

void SessionPage::OnBrowseCasPatch(wxCommandEvent &event)
{
	wxString defaultpath = ::wxPathOnly(m_tape2->GetValue());
	wxString path;
#ifndef __MOTIF__
	path = _("All known cassette files|*.zip;*.gz;*.cas|Uncompressed cassette files|*.cas|Compressed files (*.zip;*.gz)|*.gz;*.zip|All files|*.*||");
#else
	path = _("*.*");
#endif

	wxFileDialog filedlg(this,_("Select Tape-image"), defaultpath, _(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_tape2->SetValue (filedlg.GetPath());	
	}		
	CheckMedia();
}

int SessionPage::CompareCaseInsensitive(const wxString& first, const wxString& second)
{
	int result = first.CmpNoCase(second.c_str());
	if (result != 0) return result;

	return first.Cmp(second.c_str());
}

void SessionPage::prepareExtensions(wxString sharepath, wxArrayString & extensionArray)
{
	wxDir sharedir (sharepath + _("/extensions"));
	wxString extension;
	bool succes = sharedir.GetFirst(&extension);
	while (succes)
	{
		if (::wxFileExists(sharepath + _("/extensions/") + extension + _("/hardwareconfig.xml"))){
			extensionArray.Add(extension);
		}
		succes = sharedir.GetNext(&extension);
	}
}

void SessionPage::fillExtensions (wxArrayString & extensionArray)
{
	for (unsigned int i=0;i<extensionArray.GetCount();i++)
	{
		if (m_extensionList->FindString (extensionArray[i]) == -1)
			m_extensionList->Append(extensionArray[i]);
	}

}

void SessionPage::prepareMachines(wxString sharepath, wxArrayString & machineArray)
{
	wxDir sharedir (sharepath + _("/machines"));
	wxString machine;
	bool succes = sharedir.GetFirst(&machine);
	while (succes)
	{
		if (::wxFileExists(sharepath + _("/machines/") + machine + _("/hardwareconfig.xml"))){
			machineArray.Add(machine);
		}
		succes = sharedir.GetNext(&machine);
	}
	m_machineList->SetSelection(0);
}

void SessionPage::fillMachines (wxArrayString & machineArray)
{
	for (unsigned int i=0;i<machineArray.GetCount();i++)
	{
		if (m_machineList->FindString (machineArray[i]) == -1)
			m_machineList->Append(machineArray[i]);
	}
}

void SessionPage::OnChangeMedia(wxCommandEvent &event)
{
	CheckMedia();
}

void SessionPage::CheckMedia()
{
	bool enabled = false;
	if (m_parent->m_lastDiskA != m_diskA->GetValue()) enabled = true;
	if (m_parent->m_lastDiskB != m_diskB->GetValue()) enabled = true;
	if (m_parent->m_lastTape1 != m_tape1->GetValue()) enabled = true;
	if (m_parent->m_lastTape2 != m_tape2->GetValue()) enabled = true;
	m_parent->m_applyButton->Enable (enabled);
}
