// $Id: SessionPage.cpp,v 1.71 2005/10/23 12:24:09 manuelbi Exp $
// SessionPage.cpp: implementation of the SessionPage class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dir.h>
#include <wx/filedlg.h>
#include <wx/dnd.h>
#include <wx/tooltip.h>

#include "SessionPage.h"
#include "ConfigurationData.h"
#include "wxCatapultFrm.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "MiscControlPage.h"
#include "VideoControlPage.h"
#include "openMSXController.h"
#include "RomTypeDlg.h"
#include "IPSSelectionDlg.h"

#ifdef __WXMSW__
#ifndef __VISUALC__
#define _WIN32_IE 0x400	// to be able to use shell objects
#endif
#include <shlobj.h>
#endif

enum
{
	// menu items
	Disk_Insert_New = 10,
	Disk_Browse_File,
	Disk_Browse_Dir,
	Disk_Browse_Ips,
	Disk_Eject,
	Cart_Browse_File,
	Cart_Eject,
	Cart_Select_Mapper,
	Cart_Browse_Ips,
	Cas_Browse_File,
	Cas_Eject,
	Cas_Rewind,
	Cas_ForcePlay
};

IMPLEMENT_CLASS(SessionPage, wxPanel)
BEGIN_EVENT_TABLE(SessionPage, wxPanel)
	EVT_COMBOBOX(XRCID("DiskAContents"),SessionPage::OnClickDiskACombo)
	EVT_COMBOBOX(XRCID("DiskBContents"),SessionPage::OnClickDiskBCombo)
	EVT_COMBOBOX(XRCID("CartAContents"),SessionPage::OnClickCartACombo)
	EVT_COMBOBOX(XRCID("CartBContents"),SessionPage::OnClickCartBCombo)
	EVT_COMBOBOX(XRCID("CassetteContents"),SessionPage::OnClickCassetteCombo)
	EVT_BUTTON(XRCID("DiskA_Button"),SessionPage::OnClickDiskMenu)
	EVT_BUTTON(XRCID("DiskB_Button"),SessionPage::OnClickDiskMenu)
	EVT_BUTTON(XRCID("CartA_Button"),SessionPage::OnClickCartMenu)
	EVT_BUTTON(XRCID("CartB_Button"),SessionPage::OnClickCartMenu)
	EVT_BUTTON(XRCID("CassetteButton"),SessionPage::OnClickCasMenu)
	EVT_BUTTON(XRCID("BrowseDiskA"),SessionPage::OnBrowseDiskA)
	EVT_BUTTON(XRCID("BrowseDiskB"),SessionPage::OnBrowseDiskB)
	EVT_BUTTON(XRCID("BrowseCartA"),SessionPage::OnBrowseCartA)
	EVT_BUTTON(XRCID("BrowseCartB"),SessionPage::OnBrowseCartB)
	EVT_BUTTON(XRCID("BrowseCassette"),SessionPage::OnBrowseCassette)
	EVT_BUTTON(XRCID("ClearDiskA"),SessionPage::OnClearDiskA)
	EVT_BUTTON(XRCID("ClearDiskB"),SessionPage::OnClearDiskB)
	EVT_BUTTON(XRCID("ClearCartA"),SessionPage::OnClearCartA)
	EVT_BUTTON(XRCID("ClearCartB"),SessionPage::OnClearCartB)
	EVT_BUTTON(XRCID("ClearCassette"),SessionPage::OnClearCassette)
	EVT_BUTTON(XRCID("RewindButton"),SessionPage::OnRewind)
	EVT_TOGGLEBUTTON(XRCID("ForcePlayButton"),SessionPage::OnForcePlay)
	EVT_TEXT(XRCID("DiskAContents"),SessionPage::OnChangeDiskAContents)
	EVT_TEXT(XRCID("DiskBContents"),SessionPage::OnChangeDiskBContents)
	EVT_TEXT(XRCID("CartAContents"),SessionPage::OnChangeCartAContents)
	EVT_TEXT(XRCID("CartBContents"),SessionPage::OnChangeCartBContents)
	EVT_TEXT(XRCID("CassetteContents"),SessionPage::OnChangeCassetteContents)
	EVT_MENU(Disk_Insert_New,SessionPage::OnInsertEmptyDiskByMenu)
	EVT_MENU(Disk_Browse_File,SessionPage::OnBrowseDiskByMenu)
	EVT_MENU(Disk_Browse_Dir,SessionPage::OnBrowseDiskDirByMenu)
	EVT_MENU(Disk_Browse_Ips,SessionPage::OnBrowseDiskIps)
	EVT_MENU(Disk_Eject,SessionPage::OnEjectDiskByMenu)
	EVT_MENU(Cart_Browse_File,SessionPage::OnBrowseCartByMenu)
	EVT_MENU(Cart_Eject,SessionPage::OnEjectCartByMenu)
	EVT_MENU(Cart_Select_Mapper,SessionPage::OnSelectMapper)
	EVT_MENU(Cart_Browse_Ips,SessionPage::OnSelectIPS)
	EVT_MENU(Cas_Browse_File,SessionPage::OnBrowseCassette)
	EVT_MENU(Cas_Eject,SessionPage::OnClearCassette)
	EVT_MENU(Cas_Rewind,SessionPage::OnRewind)
	EVT_MENU(Cas_ForcePlay,SessionPage::OnForcePlayByMenu)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

SessionPage::SessionPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	int j;
	m_controller = controller;
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("SessionPage"));

	for (j=0;j<2;j++)
	{
		m_diskMenu[j] = new wxMenu(wxT(""),0);
//		m_diskMenu[j]->Append(Disk_Insert_New,wxT("Insert empty disk"),wxT(""),wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_File,wxT("Browse for disk image"),wxT(""),wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_Dir,wxT("Browse for disk folder (DirAsDisk)"),wxT(""),wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Eject,wxT("Eject disk"),wxT(""),wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"),wxT(""),wxITEM_NORMAL);

		m_cartMenu[j] = new wxMenu(wxT(""),0);
		m_cartMenu[j]->Append(Cart_Browse_File,wxT("Browse ROM image"),wxT(""),wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Eject,wxT("Eject ROM"),wxT(""),wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Select_Mapper,wxT("Select ROM type (AUTO)"),wxT(""),wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"),wxT(""),wxITEM_NORMAL);
	}
	m_casMenu = new wxMenu(wxT(""),0);
	m_casMenu->Append(Cas_Browse_File,wxT("Browse cassette image"),wxT(""),wxITEM_NORMAL);
	m_casMenu->Append(Cas_Eject,wxT("Eject cassette"),wxT(""),wxITEM_NORMAL);
	m_casMenu->Append(Cas_Rewind,wxT("Rewind cassette"),wxT(""),wxITEM_NORMAL);
	m_casMenu->Append(Cas_ForcePlay,wxT("Force play"),wxT(""),wxITEM_CHECK);


	m_diskA = new mediaInfo(m_diskMenu[0]);
	m_diskB = new mediaInfo(m_diskMenu[1]);
	m_cartA = new mediaInfo(m_cartMenu[0]);
	m_cartB = new mediaInfo(m_cartMenu[1]);
	m_cassette = new mediaInfo(m_casMenu);

	m_parent = (wxCatapultFrame *)parent->GetParent()->GetParent();
	m_diskA->control = (wxComboBox *)FindWindowByName(wxT("DiskAContents"));
	m_diskB->control = (wxComboBox *)FindWindowByName(wxT("DiskBContents"));
	m_cartA->control = (wxComboBox *)FindWindowByName(wxT("CartAContents"));
	m_cartB->control = (wxComboBox *)FindWindowByName(wxT("CartBContents"));
	m_cassette->control = (wxComboBox *)FindWindowByName(wxT("CassetteContents"));
	m_extensionList = (wxListBox *)FindWindowByName(wxT("ExtensionList"));
	m_machineList = (wxComboBox *)FindWindowByName(wxT("MachineList"));
	m_browseDiskA = (wxBitmapButton *)FindWindowByName(wxT("BrowseDiskA"));
	m_browseDiskB = (wxBitmapButton *)FindWindowByName(wxT("BrowseDiskB"));
	m_browseCartA = (wxBitmapButton *)FindWindowByName(wxT("BrowseCartA"));
	m_browseCartB = (wxBitmapButton *)FindWindowByName(wxT("BrowseCartB"));
	m_browseCassette = (wxBitmapButton *)FindWindowByName(wxT("BrowseCassette"));
	m_clearDiskA = (wxBitmapButton *)FindWindowByName(wxT("ClearDiskA"));
	m_clearDiskB = (wxBitmapButton *)FindWindowByName(wxT("ClearDiskB"));
	m_clearCartA = (wxBitmapButton *)FindWindowByName(wxT("ClearCartA"));
	m_clearCartB = (wxBitmapButton *)FindWindowByName(wxT("ClearCartB"));
	m_clearCassette = (wxBitmapButton *)FindWindowByName(wxT("ClearCassette"));
    m_forcePlayButton = (wxToggleButton *)FindWindowByName(wxT("ForcePlayButton"));
	m_rewindButton = (wxButton *)FindWindowByName(wxT("RewindButton"));
	m_diskAButton = (wxButton *)FindWindowByName(wxT("DiskA_Button"));
	m_diskBButton = (wxButton *)FindWindowByName(wxT("DiskB_Button"));
	m_cartAButton = (wxButton *)FindWindowByName(wxT("CartA_Button"));
	m_cartBButton = (wxButton *)FindWindowByName(wxT("CartB_Button"));
	m_cassetteButton = (wxButton *)FindWindowByName(wxT("CassetteButton"));
	m_machineListLabel = (wxStaticText *)FindWindowByName(wxT("MachineListLabel"));
	m_extensionListLabel = (wxStaticText *)FindWindowByName(wxT("ExtensionLabel"));
	m_lastDiskA = wxT("");
	m_lastDiskB = wxT("");
	m_lastCassette = wxT("");

	SetupHardware(true,false);


	// Adjust the minimum size of the extension and listbox

	wxFont myFont = m_machineList->GetFont();
	wxMemoryDC * tempDC= new wxMemoryDC();
	tempDC->SetFont(myFont);
	int dx,dy,index;
	m_machineList->GetSize(&dx,&dy); //default size
	int w,h,wMax=dx;
	int items = m_machineList->GetCount();
		for (index=0;index<items;index++){
			tempDC->GetTextExtent(m_machineList->GetString(index),&w,&h);
			if (w > wMax){
				wMax = w;
			}
		}
	m_machineList->SetSizeHints(wMax,-1);

	myFont = m_extensionList->GetFont();
	tempDC->SetFont(myFont);
	m_extensionList->GetSize(&dx,&dy); //default size
	wMax = dx;
	items = m_extensionList->GetCount();
	for (index=0;index<items;index++){
		tempDC->GetTextExtent(wxString(m_extensionList->GetString(index) + wxT("W")),&w,&h); // no idea why we need the extra W...
		if (w > wMax){
			wMax = w;
		}
	}
	if (wMax>300) { wMax = 300; } // just to have some limit
	m_extensionList->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X),-1);
	delete tempDC;

	m_cassettePortState = wxT("disabled");
	m_romTypeDialog = new RomTypeDlg (wxGetTopLevelParent(this));
	GetRomTypes();
	m_ipsDialog = new IPSSelectionDlg (wxGetTopLevelParent(this));
	m_diskA->control->SetDropTarget(new SessionDropTarget(m_diskA->control));
	m_diskB->control->SetDropTarget(new SessionDropTarget(m_diskB->control));
	m_cartA->control->SetDropTarget(new SessionDropTarget(m_cartA->control));
	m_cartB->control->SetDropTarget(new SessionDropTarget(m_cartB->control));
	m_cassette->control->SetDropTarget(new SessionDropTarget(m_cassette->control));
}

SessionPage::~SessionPage()
{
	delete m_romTypeDialog;
	delete m_diskA;
	delete m_diskB;
}

void SessionPage::OnClearDiskA(wxCommandEvent &event)
{
	m_diskA->control->SetValue(wxT(""));
	m_diskA->contents = wxT("");
	m_diskA->ips.Clear();
	m_diskA->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_lastDiskA = wxT("");
	m_controller->WriteCommand(wxT("diska eject"));
}

void SessionPage::OnClearDiskB(wxCommandEvent &event)
{
	m_diskB->control->SetValue(wxT(""));
	m_diskB->contents = wxT("");
	m_diskB->ips.Clear();
	m_diskB->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_lastDiskB = wxT("");
	m_controller->WriteCommand(wxT("diskb eject"));
}

void SessionPage::OnClearCartA(wxCommandEvent &event)
{
	m_cartA->contents = wxT("");
	m_cartA->ips.Clear();
	m_cartA->type = wxT("");
	m_cartA->control->SetValue(wxT(""));
	m_cartA->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_cartA->mmenu->SetLabel(Cart_Select_Mapper,wxT("Select cartridge type (AUTO)"));
}

void SessionPage::OnClearCartB(wxCommandEvent &event)
{
	m_cartB->contents = wxT("");
	m_cartB->ips.Clear();
	m_cartB->type = wxT("");
	m_cartB->control->SetValue(wxT(""));
	m_cartB->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_cartB->mmenu->SetLabel(Cart_Select_Mapper,wxT("Select cartridge type (AUTO)"));
}

void SessionPage::OnClearCassette(wxCommandEvent &event)
{
	m_cassette->contents = wxT("");
	m_cassette->control->SetValue(wxT(""));
	m_lastCassette = wxT("");
	m_controller->WriteCommand(wxT("cassetteplayer eject"));
#ifdef __WXMSW__
	// Bug in wxMSW? On wxGTK this is not necessary
	OnChangeCassetteContents(event);
#endif
}

void SessionPage::OnRewind(wxCommandEvent &event)
{
	m_controller->WriteCommand(wxT("cassetteplayer rewind"));
}

void SessionPage::OnForcePlay(wxCommandEvent &event)
{
	if (m_forcePlayButton->GetValue()){
		m_controller->WriteCommand(wxT("cassetteplayer force_play"));
	}
	else {
		m_controller->WriteCommand(wxT("cassetteplayer no_force_play"));
	}
}

void SessionPage::OnForcePlayByMenu(wxCommandEvent & event)
{
	m_forcePlayButton->SetValue(!m_forcePlayButton->GetValue());
	OnForcePlay(event);
}

void SessionPage::OnBrowseDiskA(wxCommandEvent &event)
{
	if (BrowseDisk (m_diskA, wxT("diska"), ::wxPathOnly(m_diskA->contents))) {
		m_lastDiskA = m_diskA->contents;
	}
}

void SessionPage::OnBrowseDiskB(wxCommandEvent &event)
{
	if (BrowseDisk (m_diskB, wxT("diskb"), ::wxPathOnly(m_diskB->contents))) {
		m_lastDiskB = m_diskB->contents;
	}
}

bool SessionPage::BrowseDisk(mediaInfo *target, wxString devicename, wxString defaultpath)
{
	wxString path;
#ifndef __MOTIF__
	path = wxT("All known disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.zip;*.ZIP;*.gz;*.GZ;*.di1;*.DI1;*.di2;*.DI2|Uncompressed disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.di1;*.DI1;*.di2;*.DI2|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

 	wxFileDialog filedlg(this,wxT("Select disk image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		target->contents = filedlg.GetPath();
		target->control->SetValue (target->contents);
		m_controller->WriteCommand(wxString(devicename +wxT(" eject")));
		if (!target->contents.IsEmpty()) {
			m_controller->WriteCommand(devicename +wxT(" ") + ConvertPath(target->contents,true));
			if (m_controller->IsOpenMSXRunning()) {
				AddHistory(target);
				SaveHistory();
				target->ips.Clear();
				target->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
			}
		}
		return true;
	}
	return false;
}

void SessionPage::OnBrowseCartA(wxCommandEvent &event)
{
	BrowseCart (m_cartA,::wxPathOnly(m_cartA->contents));
}

void SessionPage::OnBrowseCartB(wxCommandEvent &event)
{
	BrowseCart (m_cartB,::wxPathOnly(m_cartB->contents));
}

void SessionPage::BrowseCart(mediaInfo *target, wxString defaultpath)
{
	wxString path;
#ifndef __MOTIF__
	path = wxT("All known cartridge files|*.rom;*.ROM;*.ri;*.RI;*.zip;*.ZIP;*.gz;*.GZ|Uncompressed cartridge files|*.rom;*.ROM;*.ri;*.RI|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,wxT("Select ROM image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		target->contents = filedlg.GetPath();
		target->control->SetValue (target->contents);
		target->ips.Clear();
		target->type = wxT("");
		target->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
		target->mmenu->SetLabel(Cart_Select_Mapper,wxT("Select cartridge type (AUTO)"));
	}
}

void SessionPage::OnBrowseCassette(wxCommandEvent &event)
{
	wxString defaultpath = ::wxPathOnly(m_cassette->contents);
	wxString path;
#ifndef __MOTIF__
	path = wxT("All known cassette files|*.zip;*.ZIP;*.gz;*.GZ;*.wav;*.WAV;*.cas;*.CAS|Uncompressed cassette files|*.wav;*.WAV;*.cas;*.CAS|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,wxT("Select cassette image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_cassette->contents = filedlg.GetPath();
		m_cassette->control->SetValue (m_cassette->contents);
		m_controller->WriteCommand(wxT("cassetteplayer eject"));
		if (!m_cassette->contents.IsEmpty()) {
			if ((m_cassettePortState != wxT("disabled")) &&
				(m_cassettePortState != wxT("cassetteplayer"))) {
					m_controller->WriteCommand(wxT("plug cassetteport cassetteplayer"));
			}
			m_controller->WriteCommand(wxT("cassetteplayer ") + ConvertPath(m_cassette->contents,true));
			if (m_controller->IsOpenMSXRunning()) {
				AddHistory(m_cassette);
				SaveHistory();
			}
		}
		m_lastCassette = m_cassette->contents;
		OnChangeCassetteContents(event);
	}
}

void SessionPage::OnClickDiskACombo(wxCommandEvent & event)
{
	OnClickCombo(event);
	OnChangeDiskAContents(event);
	if (m_controller->IsOpenMSXRunning()) {
		m_controller->WriteCommand(wxT("diska eject"));
		if (!m_diskA->contents.IsEmpty()) {
			m_controller->WriteCommand(wxT("diska ") + ConvertPath(m_diskA->contents,true));
		}
	}
}

void SessionPage::OnChangeDiskAContents(wxCommandEvent & event)
{
	m_diskA->ips.Clear();
	m_diskA->contents = m_diskA->control->GetValue();
	m_diskA->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
}

void SessionPage::OnClickDiskBCombo(wxCommandEvent & event)
{
	OnClickCombo(event);
	OnChangeDiskBContents(event);
	if (m_controller->IsOpenMSXRunning()) {
		m_controller->WriteCommand(wxT("diskb eject"));
		if (!m_diskB->contents.IsEmpty()) {
			m_controller->WriteCommand(wxT("diskb ") + ConvertPath(m_diskB->contents,true));
		}
	}
}

void SessionPage::OnChangeDiskBContents(wxCommandEvent & event)
{
	m_diskA->ips.Clear();
	m_diskB->contents = m_diskB->control->GetValue();
	m_diskB->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
}

void SessionPage::UpdateMenuMapperLabel(mediaInfo * target)
{
	wxString item = wxT("Select cartridge type (AUTO)");
	if ((target->type != wxT("")) && (target->type.Upper() != wxT("AUTO"))){
		item = wxT("Select cartridge type (") + target->type + wxT(")");
	}
	target->mmenu->SetLabel(Cart_Select_Mapper,item);
}

void SessionPage::OnClickCartACombo(wxCommandEvent & event)
{
	OnClickCombo(event);
	m_cartA->ips.Clear();
	m_cartA->contents = m_cartA->control->GetValue();
	m_cartA->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_cartA->type = m_cartA->typehistory[event.GetInt()];
	UpdateMenuMapperLabel(m_cartA);
	m_cartA->avoid_evt = true;
}

void SessionPage::OnChangeCartAContents(wxCommandEvent & event)
{
	if (!m_cartA->avoid_evt){
		;
		m_cartA->ips.Clear();
		m_cartA->type = wxT("");
		m_cartA->contents = m_cartA->control->GetValue();
		m_cartA->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
		UpdateMenuMapperLabel(m_cartA);
	}
	m_cartA->avoid_evt = false;
}

void SessionPage::OnClickCartBCombo(wxCommandEvent & event)
{
	OnClickCombo(event);
	m_cartB->ips.Clear();
	m_cartB->contents = m_cartB->control->GetValue();
	m_cartB->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	m_cartB->type = m_cartB->typehistory[event.GetInt()];
	UpdateMenuMapperLabel(m_cartB);
	m_cartB->avoid_evt = true;
}

void SessionPage::OnChangeCartBContents(wxCommandEvent & event)
{
	if (!m_cartB->avoid_evt){
		m_cartB->ips.Clear();
		m_cartB->type=wxT("");
		m_cartB->contents = m_cartB->control->GetValue();
		m_cartB->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
		UpdateMenuMapperLabel(m_cartA);
	}
	m_cartB->avoid_evt = false;
}

void SessionPage::OnClickCassetteCombo(wxCommandEvent & event)
{
	OnClickCombo(event);
	OnChangeCassetteContents(event);
}

void SessionPage::OnChangeCassetteContents(wxCommandEvent &event)
{
	m_cassette->contents = m_cassette->control->GetValue();
	if ((m_cassettePortState != wxT("disabled")) &&
		(m_cassettePortState != wxT("cassetteplayers")) &&
		(m_cassette->contents != wxT("")))
	{
		m_rewindButton->Enable(true);
		m_forcePlayButton->Enable(true);
	}
	else
	{
		m_rewindButton->Enable(false);
		m_forcePlayButton->Enable(false);
	}
}

void SessionPage::SetupHardware (bool initial, bool reset)
{
	wxString currentMachine;
	wxArrayString currentExtensions;
	if (!initial) {
		wxArrayInt sel;
		currentMachine  = m_machineList->GetValue();
		currentExtensions.Clear();
		if (m_extensionList->GetSelections(sel) >0) {
			for (unsigned int i=0;i<sel.GetCount();i++) {
				currentExtensions.Add(m_extensionList->GetString(sel[i]));
			}
		}
	}
	ConfigurationData * config = ConfigurationData::instance();
	m_machineArray.Clear();
	m_extensionArray.Clear();
	m_machineList->Clear();
	m_machineList->Append(wxT(" <default> "));
	m_extensionList->Clear();
	wxString checkedMachines;
	wxString checkedExtensions;
	config->GetParameter(ConfigurationData::CD_MACHINES, checkedMachines);
	config->GetParameter(ConfigurationData::CD_EXTENSIONS, checkedExtensions);
	if (!checkedMachines.IsEmpty() && !checkedExtensions.IsEmpty() && !reset){
		int pos;
		do
		{
			pos = checkedMachines.Find(wxT("::"));
			if (pos != -1)
			{
				m_machineArray.Add(checkedMachines.Left(pos));
				checkedMachines = checkedMachines.Mid(pos + 2);
			}
		}while (pos !=-1);	
		do
		{
			pos = checkedExtensions.Find(wxT("::"));
			if (pos != -1)
			{
				m_extensionArray.Add(checkedExtensions.Left(pos));
				checkedExtensions = checkedExtensions.Mid(pos + 2);
			}
		}while (pos !=-1);	
	}
	else{
		wxString sharepath;
		config->GetParameter(ConfigurationData::CD_SHAREPATH,sharepath);
		prepareExtensions (sharepath, m_extensionArray);
		prepareMachines (sharepath, m_machineArray);
		wxString personalShare;
#ifdef __UNIX__
		personalShare = ::wxGetHomeDir() +wxT("/.openMSX/share");
#else
		TCHAR temp[MAX_PATH + 1];
		SHGetSpecialFolderPath(0,temp,CSIDL_PERSONAL, 1);
		personalShare = wxString((const wxChar *)temp) +wxT("/openMSX/share");
#endif
		prepareExtensions (personalShare, m_extensionArray, true);
		prepareMachines (personalShare, m_machineArray, true);
	}	
	m_extensionArray.Sort(SessionPage::CompareCaseInsensitive);
	m_machineArray.Sort(SessionPage::CompareCaseInsensitive);
	fillExtensions (m_extensionArray);
	fillMachines (m_machineArray);
	if (!initial) {
		int sel = m_machineList->FindString(currentMachine);
		if (sel != -1) {
			m_machineList->SetSelection(sel);
		}
		else{
			m_machineList->SetSelection(0);
		}
		for (unsigned i=0;i<currentExtensions.GetCount();i++) {
			sel = m_extensionList->FindString(currentExtensions[i]);
			if (sel != -1) {
				m_extensionList->SetSelection(sel);
			}
		}
	}
}


int SessionPage::CompareCaseInsensitive(const wxString& first, const wxString& second)
{
	int result = first.CmpNoCase(second);
	if (result != 0) return result;

	return first.Cmp(second);
}

void SessionPage::prepareExtensions(wxString sharepath, wxArrayString & extensionArray, bool optional)
{
	if (!::wxDirExists(sharepath + wxT("/extensions"))) {
		if (!optional) {
			wxString msg;
			msg.sprintf(wxT("Directory: %s does not exist"), (char *)wxString(sharepath + wxT("/extensions")).c_str());
			wxMessageBox (msg);
		}
		return;
	}
	wxDir sharedir (sharepath + wxT("/extensions"));
	wxString extension;
	bool succes = sharedir.GetFirst(&extension);
	while (succes)
	{
		if (::wxFileExists(sharepath + wxT("/extensions/") + extension + wxT("/hardwareconfig.xml"))) {
			if (extensionArray.Index(extension,true) == wxNOT_FOUND){
				extensionArray.Add(extension);
			}
		}
		succes = sharedir.GetNext(&extension);
	}
}

void SessionPage::fillExtensions (wxArrayString & extensionArray)
{
	wxString temp;
	for (unsigned int i=0;i<extensionArray.GetCount();i++){
		temp = extensionArray[i];
		temp.Replace(wxT("_"),wxT(" "),true);
		m_extensionList->Append(temp);
	}

}

void SessionPage::prepareMachines(wxString sharepath, wxArrayString & machineArray, bool optional)
{
	wxString cmd;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	if (!::wxDirExists(sharepath + wxT("/machines"))) {
		if (!optional) {
			wxString msg;
			msg.sprintf(wxT("Directory: %s does not exist"), (char *)wxString(sharepath + wxT("/machines")).c_str());
			wxMessageBox (msg);
		}
		return;
	}
	wxDir sharedir (sharepath + wxT("/machines"));
	wxString machine;
	bool succes = sharedir.GetFirst(&machine);
	while (succes)
	{
		if (::wxFileExists(sharepath + wxT("/machines/") + machine + wxT("/hardwareconfig.xml"))) {
			if (machineArray.Index(machine,true) == wxNOT_FOUND){
				machineArray.Add(machine);
			}
		}
		succes = sharedir.GetNext(&machine);
	}
	m_machineList->SetSelection(0);
}

void SessionPage::fillMachines (wxArrayString & machineArray)
{
	wxString temp;
	for (unsigned int i=0;i<machineArray.GetCount();i++)
	{
		temp = machineArray[i];
		temp.Replace(wxT("_"),wxT(" "),true);
		m_machineList->Append(temp);
	}
}

void SessionPage::HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)
{
	wxString contents;
	if (m_controller->IsOpenMSXRunning()){
		if (oldFocus == m_diskA->control) {
			contents = m_diskA->contents;
			if (contents != m_lastDiskA) {
				m_controller->WriteCommand(wxT("diska eject"));
				if (!contents.IsEmpty()) {
					m_controller->WriteCommand(wxT("diska ") + ConvertPath(m_diskA->contents,true));
					AddHistory(m_diskA);
				}
				m_lastDiskA = m_diskA->contents;
			}
		}
		else if (oldFocus == m_diskB->control) {
			contents = m_diskB->contents;
			if (contents != m_lastDiskB) {
				m_controller->WriteCommand(wxT("diskb eject"));
				if (!contents.IsEmpty()) {
					m_controller->WriteCommand(wxT("diskb ") + ConvertPath(m_diskB->contents,true));
					AddHistory(m_diskB);
				}
				m_lastDiskB = m_diskB->contents;
			}
		}
		else if (oldFocus == m_cassette->control) {
			if (m_cassette->contents != m_lastCassette) {
				contents = m_cassette->contents;
				m_controller->WriteCommand(wxT("cassetteplayer eject"));
				if (!contents.IsEmpty()) {
					m_controller->WriteCommand(wxT("cassetteplayer ") + ConvertPath(m_cassette->contents,true));
					AddHistory(m_cassette);
				}
				m_lastCassette = m_cassette->contents;
			}
		}
		SaveHistory();
	}
}

void SessionPage::SetControlsOnLaunch()
{
	wxWindow * temp;
	m_machineList->Enable(false);
	m_extensionList->Enable(false);
	m_cartA->control->Enable(false);
	m_cartB->control->Enable(false);
	m_clearCartA->Enable(false);
	m_clearCartB->Enable(false);
	m_browseCartA->Enable(false);
	m_browseCartB->Enable(false);
	m_extensionListLabel->Enable(false);
	m_machineListLabel->Enable(false);
	m_cartAButton->Enable(false);
	m_cartBButton->Enable(false);
	temp = FindWindowByLabel(wxT("Cartridge Slots"));
	if (temp){
		temp->Enable(false);
	}
}

void SessionPage::SetControlsOnEnd()
{
	wxWindow * temp;
	m_machineList->Enable(true);
	m_extensionList->Enable(true);
	m_cartA->control->Enable(true);
	m_cartB->control->Enable(true);
	m_clearCartA->Enable(true);
	m_clearCartB->Enable(true);
	m_browseCartA->Enable(true);
	m_browseCartB->Enable(true);
	m_cassettePortState = wxT("disabled");
	m_extensionListLabel->Enable(true);
	m_machineListLabel->Enable(true);
	m_cartAButton->Enable(true);
	m_cartBButton->Enable(true);
	m_rewindButton->Enable(false);
	m_forcePlayButton->Enable(false);
	m_cassetteButton->Enable(true);
	m_clearCassette->Enable(true);
	m_browseCassette->Enable(true);
	m_cassette->control->Enable(true);
	temp = FindWindowByLabel(wxT("Cartridge Slots"));
	if (temp){
		temp->Enable(true);
	}
	temp = FindWindowByLabel(wxT("Cassette Player"));
	if (temp){
		temp->Enable(true);
	}
}

void SessionPage::SetCassetteControl()
{
	bool state;
	if ((m_cassettePortState != wxT("disabled")) &&
		(m_cassettePortState != wxT("cassetteplayers"))) {
		if (m_cassette->contents != wxT("")) {
			m_rewindButton->Enable(true);
			m_forcePlayButton->Enable(true);
		}
		else{
			m_rewindButton->Enable(false);
			m_forcePlayButton->Enable(false);
		}
		state = true;
	}
	else {
		state = false;
		m_rewindButton->Enable(false);
		m_forcePlayButton->Enable(false);
	}
	m_cassetteButton->Enable(state);
	m_clearCassette->Enable(state);
	m_browseCassette->Enable(state);
	m_cassette->control->Enable(state);
	wxWindow * temp;
	temp = FindWindowByLabel(wxT("Cassette Player"));
	if (temp){
		temp->Enable(state);
	}


}

void SessionPage::getMedia(wxArrayString & parameters)
{
	wxString value;
	mediaInfo * media [5] = {m_diskA,m_diskB,m_cartA,m_cartB,m_cassette};
	parameters.Clear();
	unsigned int i=0;
	FOREACH(i,media){
		value = media[i]->contents;
		parameters.Add(value);
	}
}

void SessionPage::getTypes(wxArrayString & parameters)
{
	wxString value;
	mediaInfo * media [5] = {m_diskA,m_diskB,m_cartA,m_cartB,m_cassette};
	parameters.Clear();
	unsigned int i=0;
	FOREACH(i,media){
		value = media[i]->type;
		parameters.Add(value);
	}
}

void SessionPage::getPatches(wxArrayString * parameters)
{
	mediaInfo * media [5] = {m_diskA,m_diskB,m_cartA,m_cartB,m_cassette};

	unsigned int i=0;
	FOREACH(i,media){
		parameters[i] = media[i]->ips;
	}
}

void SessionPage::getHardware(wxArrayString & parameters)
{
	parameters.Clear();
	int pos = m_machineList->GetSelection();
	if (pos == 0){
		parameters.Add(wxT(" <default> "));
	}
	else{
		parameters.Add(ConvertPath(m_machineArray[m_machineList->GetSelection()-1]));
	}

	wxArrayInt sel;
	if (m_extensionList->GetSelections(sel) >0) {
		for (unsigned int i=0;i<sel.GetCount();i++) {
			parameters.Add(m_extensionArray[sel[i]]);
		}
	}
}

void SessionPage::UpdateSessionData()
{
	mediaInfo * media [5] = {m_diskA, m_diskB, m_cartA, m_cartB, m_cassette};
	int flags [5] = {ConfigurationData::MB_DISKA,
		ConfigurationData::MB_DISKB,
		ConfigurationData::MB_CARTA,ConfigurationData::MB_CARTB,
		ConfigurationData::MB_CASSETTE};
	unsigned int i;
	m_InsertedMedia = 0;
	i=0;
	FOREACH(i,media){
		if (!media[i]->contents.IsEmpty()) {
			AddHistory (media[i]);
			m_InsertedMedia |= flags [i];
		}
	}

	wxArrayString hardware;
	getHardware(hardware);
	m_usedMachine = hardware[0];
	m_usedExtensions.Clear();
	if (hardware.GetCount()>1)
	{
		for (i=1;i<hardware.GetCount();i++) {
			m_usedExtensions += hardware[i] + wxT("::");
		}
	}
	SaveHistory();
}

void SessionPage::AddHistory(mediaInfo *media)
{
	// wxWindows 2.4 does not support insertion in a wxComboBox
	// so this is gonna be replaced as soon as 2.6 is stable
	unsigned int i;
	wxString currentItem = media->contents;
	wxString currentType = media->type;
	bool isCart = ((media == m_cartA) || (media == m_cartB));
#ifdef __WXMSW__
	currentItem.Replace(wxT("/"),wxT("\\"),true);
#else
	currentItem.Replace(wxT("\\"),wxT("/"),true);
#endif
	int pos = media->history.Index(currentItem);
	if (pos != wxNOT_FOUND)
	{
		media->history.RemoveAt(pos);
		if (isCart){
			media->typehistory.RemoveAt(pos);
		}
	}
	media->history.Insert(currentItem,0);
	if (isCart){
		media->typehistory.Insert(currentType,0);
	}
	while (media->history.GetCount() > HISTORY_SIZE)
	{
		media->history.RemoveAt(HISTORY_SIZE);
		if (isCart){
			media->history.RemoveAt(HISTORY_SIZE);
		}
	}
	media->control->Clear();
	for (i=0;i<media->history.GetCount();i++){
		media->control->Append(media->history[i]);
	}
//	media->control->SetValue(currentItem);
	media->control->SetSelection(0);
}

void SessionPage::RestoreHistory()
{
	wxString temp;
	mediaInfo * media[5] = {m_diskA, m_diskB, m_cartA, m_cartB, m_cassette};
	ConfigurationData::ID id[5] = {ConfigurationData::CD_HISTDISKA,
		ConfigurationData::CD_HISTDISKB,
		ConfigurationData::CD_HISTCARTA,
		ConfigurationData::CD_HISTCARTB,
		ConfigurationData::CD_HISTCASSETTE};
	ConfigurationData::ID typeID[2] = {ConfigurationData::CD_TYPEHISTCARTA, ConfigurationData::CD_TYPEHISTCARTB};
	ConfigurationData * config = ConfigurationData::instance();
	config->GetParameter(ConfigurationData::CD_MEDIAINSERTED, &m_InsertedMedia);
	wxString value;
	wxString types;
	int pos;
	unsigned int i=0;
	int hist=-1;
	FOREACH(i,media){
		media[i]->control->Clear();
		config->GetParameter(id[i],value);
		do
		{
			pos = value.Find(wxT("::"));
			if (pos != -1)
			{
				media[i]->history.Add(value.Left(pos));
				media[i]->control->Append(value.Left(pos));
				value = value.Mid(pos + 2);
			}
		}while (pos !=-1);
		if ((media[i]==m_cartA) || (media[i]==m_cartB)){
			hist++;
			config->GetParameter(typeID[hist],types);
			do
			{
				pos = types.Find(wxT("::"));
				if (pos != -1)
				{
					media[i]->typehistory.Add(types.Left(pos));
					types = types.Mid(pos + 2);
				}
			}while (pos !=-1);
			while (media[i]->typehistory.GetCount() < media[i]->history.GetCount()){
				media[i]->typehistory.Add(wxT("auto"));
			}
		}
		if ((m_InsertedMedia & (1 << i)) && (media[i]->history.GetCount() >0)) {
			media[i]->control->SetSelection(0);
			media[i]->contents=media[i]->history[0];
			if ((media[i]==m_cartA) || (media[i]==m_cartB)){
				media[i]->type=media[i]->typehistory[0];
				UpdateMenuMapperLabel(media[i]);
			}
		}
		else {
			media[i]->control->SetValue(wxT(""));
			media[i]->contents=wxT("");
		}
	}
		config->GetParameter(ConfigurationData::CD_USEDMACHINE, m_usedMachine);
	if (!m_usedMachine.IsEmpty()){
		temp = m_usedMachine;
		temp.Replace(wxT("_"),wxT(" "),true);
		temp.Replace(wxT("\""),wxT(""),true);
		int pos = m_machineList->FindString(temp);

		if (pos != -1){
			m_machineList->SetSelection (pos);
		}
		else {
			m_machineList->SetSelection (0);
		}
	}
	config->GetParameter(ConfigurationData::CD_USEDEXTENSIONS,value);
	m_usedExtensions = value;
	do
	{
		pos = value.Find(wxT("::"));
		if (pos != -1){
			temp = value.Left(pos);
			temp.Replace(wxT("_"),wxT(" "),true);
			if (m_extensionList->FindString(temp) != -1){
				m_extensionList->SetStringSelection (temp);
			}
			value = value.Mid(pos + 2);
		}
	} while (pos !=-1);
}

void SessionPage::SaveHistory()
{
	mediaInfo * media[5] = {m_diskA, m_diskB, m_cartA, m_cartB, m_cassette};
	ConfigurationData::ID id[5] = {ConfigurationData::CD_HISTDISKA,
			ConfigurationData::CD_HISTDISKB, ConfigurationData::CD_HISTCARTA,
			ConfigurationData::CD_HISTCARTB, ConfigurationData::CD_HISTCASSETTE
	};
	ConfigurationData::ID typeID[2] = {ConfigurationData::CD_TYPEHISTCARTA,
									   ConfigurationData::CD_TYPEHISTCARTB
	};
	ConfigurationData * config = ConfigurationData::instance();
	wxString temp;
	unsigned int i=0;
	unsigned int j;
	int hist=-1;
	FOREACH(i,media){
		temp.Clear();
		for (j=0;j<media[i]->history.GetCount();j++) {
			temp += media[i]->history[j];
			temp += wxT("::");
		}
		config->SetParameter(id[i],temp);
		if ((media[i] == m_cartA) || (media[i] == m_cartB)){
			hist++;
			temp.Clear();
			for (j=0;j<media[i]->typehistory.GetCount();j++) {
				if (media[i]->typehistory[j] == wxT("")){
					temp += wxT("auto");
				}
				else{
					temp += media[i]->typehistory[j];
				}
				temp += wxT("::");
			}
			config->SetParameter(typeID[hist],temp);
		}


	}
	config->SetParameter(ConfigurationData::CD_MEDIAINSERTED,(long) m_InsertedMedia);
	config->SetParameter(ConfigurationData::CD_USEDMACHINE,m_usedMachine);
	config->SetParameter(ConfigurationData::CD_USEDEXTENSIONS,m_usedExtensions);
	bool result;
	result = ConfigurationData::instance()->SaveData();
#if !OPENMSX_DEMO_CD_VERSION
	if (!result) {
		wxMessageBox (wxT("Error saving configuration data"));
	}
#endif
}

void SessionPage::EnableCassettePort (wxString data)
{
	m_cassettePortState = data;
}

void SessionPage::AutoPlugCassette ()
{
	if (m_cassette->contents != wxT("")) {
		if ((m_cassettePortState != wxT("disabled")) &&
			(m_cassettePortState != wxT("cassetteplayers"))) {
			m_controller->WriteCommand (wxT("plug cassetteport cassetteplayer"));
		}
	}
}

void SessionPage::OnClickDiskMenu (wxCommandEvent & event)
{
	bool enable=true;
	int index;
	m_lastUsedPopup = (wxButton *)event.GetEventObject();
	if (m_lastUsedPopup == m_diskAButton){
		index = 0;
	}
	else{
		index = 1;
	}
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		if (target->contents.IsEmpty()){
			enable = false;
		}
		m_diskMenu[index]->Enable(Disk_Browse_Ips,enable);
	}
	wxRect myRect = ((wxWindow *)event.GetEventObject())->GetRect();
	PopupMenu(m_diskMenu[index],myRect.GetLeft(),myRect.GetBottom());
}

void SessionPage::OnClickCasMenu (wxCommandEvent & event)
{
	m_lastUsedPopup = (wxButton *)event.GetEventObject();
	wxRect myRect = ((wxWindow *)event.GetEventObject())->GetRect();
	m_casMenu->Enable(Cas_Rewind,m_rewindButton->IsEnabled());
	m_casMenu->Enable(Cas_ForcePlay,m_forcePlayButton->IsEnabled());
	m_casMenu->Check(Cas_ForcePlay,m_forcePlayButton->GetValue());
	PopupMenu(m_casMenu,myRect.GetLeft(),myRect.GetBottom());
}

SessionPage::mediaInfo * SessionPage::GetLastMenuTarget()
{
	mediaInfo * retVal = NULL;
	wxButton * button[]={m_diskAButton,m_diskBButton,m_cartAButton,m_cartBButton};
	mediaInfo * media[]={m_diskA,m_diskB,m_cartA,m_cartB};
	unsigned int i;
	FOREACH(i,media)
	{
		if (m_lastUsedPopup == button[i]){
			retVal = media[i];
		}
	}
	return retVal;
}

void SessionPage::OnClickCartMenu(wxCommandEvent & event)
{
	int index;
	bool enable = true;
	m_lastUsedPopup = (wxButton *)event.GetEventObject();
	if (m_lastUsedPopup == m_cartAButton){
		index = 0;
	}
	else{
		index = 1;
	}
	wxRect myRect = ((wxWindow *)event.GetEventObject())->GetRect();
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		if (target->contents.IsEmpty()){
			enable = false;
		}
		m_cartMenu[index]->Enable(Cart_Select_Mapper,enable);
		m_cartMenu[index]->Enable(Cart_Browse_Ips,enable);
		PopupMenu(m_cartMenu[index],myRect.GetLeft(),myRect.GetBottom());
	}
}

void SessionPage::OnInsertEmptyDiskByMenu(wxCommandEvent & event)
{
	wxString devicename;
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL ){
		devicename = m_lastUsedPopup->GetLabel();
		devicename.Replace(wxT(" "),wxT(""),TRUE);
		devicename.LowerCase();
		wxFileDialog filedlg(this,wxT("Create disk image"), ::wxPathOnly(target->contents), wxT(""), wxT("*.*"),wxSAVE|wxOVERWRITE_PROMPT);
		if (filedlg.ShowModal() == wxID_OK){
			target->contents = filedlg.GetPath();
			target->control->SetValue (target->contents);
			if (m_controller->IsOpenMSXRunning()) {
				m_controller->WriteCommand(wxString(devicename +wxT(" eject")));
				if (!target->contents.IsEmpty()) {
					m_controller->WriteCommand(devicename +wxT(" ") + ConvertPath(target->contents,true));
				}
				AddHistory(target);
				SaveHistory();
			}
		}
	}
}

void SessionPage::OnBrowseCartByMenu(wxCommandEvent &event)
{
	if (m_lastUsedPopup == m_cartAButton){
		OnBrowseCartA(event);
	}
	else if (m_lastUsedPopup == m_cartBButton){
		OnBrowseCartB(event);
	}
}

void SessionPage::OnBrowseDiskByMenu(wxCommandEvent &event)
{
	if (m_lastUsedPopup == m_diskAButton){
		OnBrowseDiskA(event);
	}
	else if (m_lastUsedPopup == m_diskBButton){
		OnBrowseDiskB(event);
	}
}

void SessionPage::OnBrowseDiskDirByMenu(wxCommandEvent &event)
{
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		wxString defaultpath = ::wxPathOnly(target->contents);
		wxDirDialog dirdlg(this,wxT("Select Directory to use as disk"), defaultpath);
		if (dirdlg.ShowModal() == wxID_OK)
		{
			target->contents = dirdlg.GetPath();
			target->control->SetValue(target->contents);
		}
	}
}

void SessionPage::OnEjectDiskByMenu(wxCommandEvent & event)
{
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		target->contents = wxT("");
		target->control->Clear();
		target->mmenu->SetLabel(Disk_Browse_Ips,wxT("Select IPS Patches (None selected)"));
	}
}

void SessionPage::OnEjectCartByMenu (wxCommandEvent & event)
{
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		target->contents = wxT("");
		target->control->SetValue(wxT("")); // Uing Clear() doesn't seem to do the trick properly (?)
		target->ips.Clear();
		target->mmenu->SetLabel(Cart_Browse_Ips,wxT("Select IPS Patches (None selected)"));
		target->mmenu->SetLabel(Cart_Select_Mapper,wxT("Select cartridge type (AUTO)"));
	}
}

void SessionPage::OnSelectMapper(wxCommandEvent & event)
{
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		wxString value = target->type;
		m_romTypeDialog->CenterOnParent();
		if (m_romTypeDialog->ShowModal(value) == wxID_OK){
			target->type = m_romTypeDialog->GetSelectedType();
			UpdateMenuMapperLabel(target);
		}
	}
}

void SessionPage::OnSelectIPS(wxCommandEvent & event)
{
	wxString item = wxT("Select IPS Patches (None selected)");
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		m_ipsDialog->CenterOnParent();
		if (m_ipsDialog->ShowModal(target->ips,target->ipsdir) == wxID_OK){
			m_ipsDialog->GetIPSList(target->ips);
			int count = target->ips.GetCount();
			if (count > 0){
				item.sprintf(wxT("Select IPS Patches (%d selected)"),count);
			}
			target->mmenu->SetLabel(Cart_Browse_Ips,item);
			target->ipsdir = m_ipsDialog->GetLastBrowseLocation();
		}
	}
}

void SessionPage::OnBrowseDiskIps(wxCommandEvent & event)
{
	wxString item = wxT("Select IPS Patches (None selected)");
	mediaInfo * target = GetLastMenuTarget();
	if (target != NULL){
		m_ipsDialog->CenterOnParent();
		if (m_ipsDialog->ShowModal(target->ips,target->ipsdir) == wxID_OK){
			m_ipsDialog->GetIPSList(target->ips);
			int count = target->ips.GetCount();
			if (count > 0){
				item.sprintf(wxT("Select IPS Patches (%d selected)"),count);
			}
			target->mmenu->SetLabel(Disk_Browse_Ips,item);
			if (m_controller->IsOpenMSXRunning())
			{
				wxString devicename;
				if (m_lastUsedPopup == m_diskAButton){
					devicename = wxT("diska ");
				}
				else if (m_lastUsedPopup == m_diskBButton){
					devicename = wxT("diskb ");
				}
				wxString command = devicename +wxT(" ") + ConvertPath(target->contents,true) + wxT(" ");
				for (unsigned int i=0;i<target->ips.GetCount();i++)
				{
					command += ConvertPath(target->ips[i],true);
				}
				m_controller->WriteCommand(command);
			}
			target->ipsdir = m_ipsDialog->GetLastBrowseLocation();
		}
	}
}

void SessionPage::GetRomTypes ()
{
	// TODO get it from openMSX
	SetupRomType(wxT(""),wxT("Autodetect type"));
	SetupRomType(wxT("AUTO"),wxT("Autodetect type"));
	SetupRomType(wxT("PAGE0"),wxT("Plain 16 kB Page 0"));
	SetupRomType(wxT("PAGE1"),wxT("Plain 16 kB Page 1"));
	SetupRomType(wxT("PAGE2"),wxT("Plain 16 kB Page 2 (Basic)"));
	SetupRomType(wxT("ROMBAS"),wxT("Plain 16 kB Page 2 (Basic)"));
	SetupRomType(wxT("PAGE3"),wxT("Plain 16 kB Page 3"));
	SetupRomType(wxT("PAGE01"),wxT("Plain 32 kB Page 0-1"));
	SetupRomType(wxT("PAGE12"),wxT("Plain 32 kB Page 1-2"));
	SetupRomType(wxT("PAGE23"),wxT("Plain 32 kB Page 2-3"));
	SetupRomType(wxT("PAGE012"),wxT("Plain 48 kB Page 0-2"));
	SetupRomType(wxT("PAGE123"),wxT("Plain 48 kB Page 1-3"));
	SetupRomType(wxT("PAGE0123"),wxT("Plain 64 kB"));
	SetupRomType(wxT("PLAIN"),wxT("Plain (Any size)"));
	SetupRomType(wxT("64kB"),wxT("Plain (Any size)"));
//	SetupRomType(wxT("DRAM"),wxT("DRAM"));
	SetupRomType(wxT("8kB"),wxT("Generic 8kB"));
	SetupRomType(wxT("0"),wxT("Generic 8kB"));
	SetupRomType(wxT("16kB"),wxT("Generic 16kB"));
	SetupRomType(wxT("1"),wxT("Generic 16kB"));
	SetupRomType(wxT("MSXDOS2"),wxT("Generic 16kB"));
	SetupRomType(wxT("KONAMI4"),wxT("Konami without SCC"));
	SetupRomType(wxT("3"),wxT("Konami without SCC"));
	SetupRomType(wxT("KONAMI5"),wxT("Konami with SCC"));
	SetupRomType(wxT("SCC"),wxT("Konami with SCC"));
	SetupRomType(wxT("2"),wxT("Konami with SCC"));
	SetupRomType(wxT("ASCII8"),wxT("Ascii 8kB"));
	SetupRomType(wxT("4"),wxT("Ascii 8kB"));
	SetupRomType(wxT("ASCII16"),wxT("Ascii 16kB"));
	SetupRomType(wxT("5"),wxT("Ascii 16kB"));
	SetupRomType(wxT("RTYPE"),wxT("R-Type"));
	SetupRomType(wxT("CROSSBLAIM"),wxT("Cross Blaim"));
//	SetupRomType(wxT("PANASONIC"),wxT("Panasonic"));
//	SetupRomType(wxT("NATIONAL"),wxT("National"));
	SetupRomType(wxT("MSX-AUDIO"),wxT("MSX-Audio"));
	SetupRomType(wxT("HARRYFOX"),wxT("Harry Fox"));
	SetupRomType(wxT("HALNOTE"),wxT("Halnote"));
	SetupRomType(wxT("KOREAN80IN1"),wxT("Korean Multigame (80 in 1)"));
	SetupRomType(wxT("KOREAN90IN1"),wxT("Korean Multigame (90 in 1)"));
	SetupRomType(wxT("KOREAN126IN1"),wxT("Korean Multigame (126 in 1)"));
	SetupRomType(wxT("HOLYQURAN"),wxT("Holy Qu'ran"));
//	SetupRomType(wxT("FSA1FM1"),wxT("Panasonic FS-A1FM Rom1"));
//	SetupRomType(wxT("FSA1FM2"),wxT("Panasonic FS-A1FM Rom2"));
	SetupRomType(wxT("ASCII16-2"),wxT("Ascii 16kB with SRAM"));
	SetupRomType(wxT("HYDLIDE2"),wxT("Ascii 16kB with SRAM"));
	SetupRomType(wxT("ASCII8-8"),wxT("Ascii 8kB with SRAM"));
	SetupRomType(wxT("KOEI-8"),wxT("Koei with 8kB SRAM"));
	SetupRomType(wxT("KOEI-32"),wxT("Koei with 32kB SRAM"));
	SetupRomType(wxT("Wizardry"),wxT("Wizardry"));
	SetupRomType(wxT("GAMEMASTER2"),wxT("Konami Game Master 2"));
	SetupRomType(wxT("RC755"),wxT("Konami Game Master 2"));
	SetupRomType(wxT("SYNTHESIZER"),wxT("Konami Synthesizer"));
	SetupRomType(wxT("MAJUTSUSHI"),wxT("Hai no Majutsushi"));
}

void SessionPage::AddRomType(wxString type)
{
	m_romTypeDialog->AddRomType(type);
}

void SessionPage::SetRomTypeFullName(wxString type, wxString fullname)
{
	m_romTypeDialog->SetRomTypeFullName (type, fullname);
}

void SessionPage::SetupRomType(wxString type, wxString fullname)
{
	AddRomType (type);
	SetRomTypeFullName(type,fullname);
}

wxArrayString& SessionPage::GetDetectedMachines ()
{
	return m_machineArray;
}

wxArrayString& SessionPage::GetDetectedExtensions ()
{
	return m_extensionArray;
}

SessionDropTarget::SessionDropTarget(wxComboBox * target)
{
	m_target = target;
}

SessionDropTarget::~SessionDropTarget()
{
}

bool SessionDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	if (filenames.GetCount() != 0)
	{
		m_target->Append(filenames[0]); // just the first for starters
	}
	return true;
}
