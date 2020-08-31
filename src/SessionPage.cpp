#include "SessionPage.h"
#include "wxCatapultFrm.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "MiscControlPage.h"
#include "VideoControlPage.h"
#include "openMSXController.h"
#include "RomTypeDlg.h"
#include "IPSSelectionDlg.h"
#include "utils.h"
#include <algorithm>
#include <map>
#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/clntdata.h>
#include <wx/combobox.h>
#include <wx/dcmemory.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/dnd.h>
#include <wx/tooltip.h>

#ifdef __WXMSW__
#ifndef __VISUALC__
#define _WIN32_IE 0x400	// to be able to use shell objects
#endif
#include <shlobj.h>
#endif

enum {
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
	Cas_MotorControl,
	Cas_AutoCreateFile,
	HardDisk_Browse_File,
	HardDisk_Eject
};

IMPLEMENT_CLASS(SessionPage, wxPanel)
BEGIN_EVENT_TABLE(SessionPage, wxPanel)
	EVT_COMBOBOX    (XRCID("DiskAContents"),    SessionPage::OnClickDiskACombo)
	EVT_COMBOBOX    (XRCID("DiskBContents"),    SessionPage::OnClickDiskBCombo)
	EVT_COMBOBOX    (XRCID("CartAContents"),    SessionPage::OnClickCartACombo)
	EVT_COMBOBOX    (XRCID("CartBContents"),    SessionPage::OnClickCartBCombo)
	EVT_COMBOBOX    (XRCID("CassetteContents"), SessionPage::OnClickCassetteCombo)
	EVT_COMBOBOX    (XRCID("HardDiskContents"), SessionPage::OnClickHardDiskCombo)
	EVT_TOGGLEBUTTON(XRCID("PlayButton"),       SessionPage::OnModePlay)
	EVT_TOGGLEBUTTON(XRCID("RecordButton"),     SessionPage::OnModeRecord)
	EVT_BUTTON      (XRCID("DiskA_Button"),     SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("DiskB_Button"),     SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("CartA_Button"),     SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("CartB_Button"),     SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("CassetteButton"),   SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("HardDiskButton"),   SessionPage::OnClickMediaMenu)
	EVT_BUTTON      (XRCID("BrowseDiskA"),      SessionPage::OnBrowseDiskA)
	EVT_BUTTON      (XRCID("BrowseDiskB"),      SessionPage::OnBrowseDiskB)
	EVT_BUTTON      (XRCID("BrowseCartA"),      SessionPage::OnBrowseCartA)
	EVT_BUTTON      (XRCID("BrowseCartB"),      SessionPage::OnBrowseCartB)
	EVT_BUTTON      (XRCID("BrowseCassette"),   SessionPage::OnBrowseCassette)
	EVT_BUTTON      (XRCID("BrowseHardDisk"),   SessionPage::OnBrowseHardDisk)
	EVT_BUTTON      (XRCID("ClearDiskA"),       SessionPage::OnEjectDiskA)
	EVT_BUTTON      (XRCID("ClearDiskB"),       SessionPage::OnEjectDiskB)
	EVT_BUTTON      (XRCID("ClearCartA"),       SessionPage::OnEjectCartA)
	EVT_BUTTON      (XRCID("ClearCartB"),       SessionPage::OnEjectCartB)
	EVT_BUTTON      (XRCID("ClearCassette"),    SessionPage::OnClearCassette)
	EVT_BUTTON      (XRCID("ClearHardDisk"),    SessionPage::OnEjectHardDisk)
	EVT_BUTTON      (XRCID("RewindButton"),     SessionPage::OnRewind)
	EVT_COMBOBOX    (XRCID("MachineList"),      SessionPage::OnMachineOrExtensionListChanged)
	EVT_LISTBOX     (XRCID("ExtensionList"),    SessionPage::OnMachineOrExtensionListChanged)
	EVT_MENU(Disk_Insert_New,      SessionPage::OnInsertEmptyDiskByMenu)
	EVT_MENU(Disk_Browse_File,     SessionPage::OnBrowseDiskByMenu)
	EVT_MENU(Disk_Browse_Dir,      SessionPage::OnBrowseDiskDirByMenu)
	EVT_MENU(Disk_Browse_Ips,      SessionPage::OnBrowseIps)
	EVT_MENU(Disk_Eject,           SessionPage::OnEjectByMenu)
	EVT_MENU(Cart_Browse_File,     SessionPage::OnBrowseCartByMenu)
	EVT_MENU(Cart_Eject,           SessionPage::OnEjectByMenu)
	EVT_MENU(Cart_Select_Mapper,   SessionPage::OnSelectMapper)
	EVT_MENU(Cart_Browse_Ips,      SessionPage::OnBrowseIps)
	EVT_MENU(Cas_Browse_File,      SessionPage::OnBrowseCassette)
	EVT_MENU(Cas_Eject,            SessionPage::OnClearCassette)
	EVT_MENU(Cas_Rewind,           SessionPage::OnRewind)
	EVT_MENU(Cas_MotorControl,     SessionPage::OnMotorControl)
	EVT_MENU(Cas_AutoCreateFile,   SessionPage::OnAutoCassettefile)
	EVT_MENU(HardDisk_Browse_File, SessionPage::OnBrowseHardDisk)
	EVT_MENU(HardDisk_Eject,       SessionPage::OnEjectByMenu)
END_EVENT_TABLE()

SessionPage::SessionPage(wxWindow* parent, openMSXController& controller)
	: m_controller(controller)
{
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("SessionPage"));

	for (int j = 0; j < 2; ++j) {
		m_diskMenu[j] = new wxMenu(wxT(""), 0);
//		m_diskMenu[j]->Append(Disk_Insert_New, wxT("Insert empty disk"), wxT(""), wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_File, wxT("Browse for disk image"), wxT(""), wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_Dir, wxT("Browse for disk folder (DirAsDisk)"), wxT(""), wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Eject, wxT("Eject disk"), wxT(""), wxITEM_NORMAL);
		m_diskMenu[j]->Append(Disk_Browse_Ips, wxT("Select IPS Patches (None selected)"), wxT(""), wxITEM_NORMAL);

		m_cartMenu[j] = new wxMenu(wxT(""), 0);
		m_cartMenu[j]->Append(Cart_Browse_File, wxT("Browse ROM image"), wxT(""), wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Eject, wxT("Eject ROM"), wxT(""), wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Select_Mapper, wxT("Select ROM type (AUTO)"), wxT(""), wxITEM_NORMAL);
		m_cartMenu[j]->Append(Cart_Browse_Ips, wxT("Select IPS Patches (None selected)"), wxT(""), wxITEM_NORMAL);
	}
	m_casMenu = new wxMenu(wxT(""), 0);
	m_casMenu->Append(Cas_Browse_File, wxT("Browse cassette image"), wxT(""), wxITEM_NORMAL);
	m_casMenu->Append(Cas_Eject, wxT("Eject cassette"), wxT(""), wxITEM_NORMAL);
	m_casMenu->Append(Cas_Rewind, wxT("Rewind cassette"), wxT(""), wxITEM_NORMAL);
	m_casMenu->Append(Cas_MotorControl, wxT("Motor control"), wxT(""), wxITEM_CHECK);
	m_casMenu->Append(Cas_AutoCreateFile, wxT("Auto create Cassette file for recording"), wxT(""), wxITEM_CHECK);

	m_hardDiskMenu = new wxMenu(wxT(""), 0);
	m_hardDiskMenu->Append(HardDisk_Browse_File, wxT("Browse for hard disk image"), wxT(""), wxITEM_NORMAL);
	m_hardDiskMenu->Append(HardDisk_Eject, wxT("Eject hard disk"), wxT(""), wxITEM_NORMAL);

	m_extensionList  = (wxListBox*)FindWindowByName(wxT("ExtensionList"));
	m_machineList    = (wxComboBox*)FindWindowByName(wxT("MachineList"));
	m_browseCassette = (wxBitmapButton*)FindWindowByName(wxT("BrowseCassette"));
	m_clearCassette  = (wxBitmapButton*)FindWindowByName(wxT("ClearCassette"));
	m_browseHardDisk = (wxBitmapButton*)FindWindowByName(wxT("BrowseHardDisk"));
	m_clearHardDisk  = (wxBitmapButton*)FindWindowByName(wxT("ClearHardDisk"));
	m_playButton     = (wxToggleButton*)FindWindowByName(wxT("PlayButton"));
	m_recordButton   = (wxToggleButton*)FindWindowByName(wxT("RecordButton"));
	m_rewindButton   = (wxButton*)FindWindowByName(wxT("RewindButton"));
	m_diskAButton    = (wxButton*)FindWindowByName(wxT("DiskA_Button"));
	m_diskBButton    = (wxButton*)FindWindowByName(wxT("DiskB_Button"));
	m_cartAButton    = (wxButton*)FindWindowByName(wxT("CartA_Button"));
	m_cartBButton    = (wxButton*)FindWindowByName(wxT("CartB_Button"));
	m_cassetteButton = (wxButton*)FindWindowByName(wxT("CassetteButton"));
	m_hardDiskButton = (wxButton*)FindWindowByName(wxT("HardDiskButton"));
	m_machineListLabel   = (wxStaticText*)FindWindowByName(wxT("MachineListLabel"));
	m_extensionListLabel = (wxStaticText*)FindWindowByName(wxT("ExtensionLabel"));
	//SetupHardware(true, false); // No need to do this, it's done in wxCatapultFrm's constructor

	m_hasCassettePort = true; // avoid UMR
	m_cassetteControl = true; // see comments in OnMotorControl()
	m_romTypeDialog.reset(new RomTypeDlg(wxGetTopLevelParent(this)));
	m_ipsDialog.reset(new IPSSelectionDlg(wxGetTopLevelParent(this)));
	media[DISKA].reset(new MediaInfo(
		*m_diskMenu[0], wxT("diska"), wxT("DiskAContents"),
		ConfigurationData::MB_DISKA, ConfigurationData::CD_HISTDISKA,
		ConfigurationData::CD_NONE, ConfigurationData::CD_IPSDISKA,
		m_diskAButton, DISKETTE, Disk_Browse_Ips, 0));
	media[DISKB].reset(new MediaInfo(
		*m_diskMenu[1], wxT("diskb"), wxT("DiskBContents"),
		ConfigurationData::MB_DISKB, ConfigurationData::CD_HISTDISKB,
		ConfigurationData::CD_NONE, ConfigurationData::CD_IPSDISKB,
		m_diskBButton, DISKETTE, Disk_Browse_Ips, 0));
	media[CARTA].reset(new MediaInfo(
		*m_cartMenu[0], wxT("carta"), wxT("CartAContents"),
		ConfigurationData::MB_CARTA, ConfigurationData::CD_HISTCARTA,
		ConfigurationData::CD_TYPEHISTCARTA, ConfigurationData::CD_IPSCARTA,
		m_cartAButton, CARTRIDGE, Cart_Browse_Ips, Cart_Select_Mapper));
	media[CARTB].reset(new MediaInfo(
		*m_cartMenu[1], wxT("cartb"), wxT("CartBContents"),
		ConfigurationData::MB_CARTB, ConfigurationData::CD_HISTCARTB,
		ConfigurationData::CD_TYPEHISTCARTB, ConfigurationData::CD_IPSCARTB,
		m_cartBButton, CARTRIDGE, Cart_Browse_Ips, Cart_Select_Mapper));
	media[CAS].reset(new MediaInfo(
		*m_casMenu, wxT("cassetteplayer"), wxT("CassetteContents"),
		ConfigurationData::MB_CASSETTE, ConfigurationData::CD_HISTCASSETTE,
		ConfigurationData::CD_NONE, ConfigurationData::CD_NONE,
		m_cassetteButton, CASSETTE, 0, 0));
	media[HDD].reset(new MediaInfo(
		*m_hardDiskMenu, wxT("hda"), wxT("HardDiskContents"),
		ConfigurationData::MB_HDD, ConfigurationData::CD_HISTHDD,
		ConfigurationData::CD_NONE, ConfigurationData::CD_NONE,
		m_hardDiskButton, HARDDISK, 0, 0));
	for (auto& m : media) {
		m->control.SetDropTarget(new SessionDropTarget(m.get(), this));
	}

	int autorecord;
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_AUTORECORD, &autorecord);
	m_cassetteAutoCreate = autorecord == 1;
	SetCassetteControl();
	SetHardDiskControl();
}

void SessionPage::FixLayout()
{
	// Needs to be called AFTER setuphardware!
	// Adjust the minimum size of the extension and listbox
	wxMemoryDC tempDC;
	tempDC.SetFont(m_machineList->GetFont());
	int dx, dy;
	m_machineList->GetSize(&dx, &dy); //default size
	int wMax = dx;
	int items = m_machineList->GetCount();
	for (int i = 0; i < items; ++i) {
		int w, h;
		tempDC.GetTextExtent(m_machineList->GetString(i), &w, &h);
		wMax = std::max(wMax, w);
	}
	m_machineList->SetSizeHints(wMax, -1);

	tempDC.SetFont(m_extensionList->GetFont());
	m_extensionList->GetSize(&dx, &dy); //default size
	wMax = dx;
	items = m_extensionList->GetCount();
	for (int i = 0; i < items; ++i) {
		int w, h;
		tempDC.GetTextExtent(m_extensionList->GetString(i) + wxT("W"), &w, &h); // no idea why we need the extra W...
		wMax = std::max(wMax, w);
	}
	wMax = std::min(wMax, 300); // just to have some limit
	m_extensionList->SetSizeHints(wMax + wxSystemSettings::GetMetric(wxSYS_VSCROLL_X), -1);
}

void SessionPage::OnEjectDiskA(wxCommandEvent& event)
{
	EjectMedia(*media[DISKA]);
}
void SessionPage::OnEjectDiskB(wxCommandEvent& event)
{
	EjectMedia(*media[DISKB]);
}
void SessionPage::OnEjectCartA(wxCommandEvent& event)
{
	EjectMedia(*media[CARTA]);
}
void SessionPage::OnEjectCartB(wxCommandEvent& event)
{
	EjectMedia(*media[CARTB]);
}
void SessionPage::OnClearCassette(wxCommandEvent& event)
{
	EjectMedia(*media[CAS]);
}
void SessionPage::OnEjectHardDisk(wxCommandEvent& event)
{
	EjectMedia(*media[HDD]);
}
void SessionPage::OnEjectByMenu(wxCommandEvent& event)
{
	if (auto* target = GetLastMenuTarget()) {
		EjectMedia(*target);
	}
}

void SessionPage::EjectMedia(MediaInfo& m)
{
	m.control.SetValue(wxT(""));
	m.control.SetSelection(wxNOT_FOUND);
	insertMediaClear(m);
}

void SessionPage::OnRewind(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("cassetteplayer rewind"));
}

void SessionPage::OnModePlay(wxCommandEvent& event)
{
	m_controller.WriteCommand(wxT("cassetteplayer play"));
}

void SessionPage::OnModeRecord(wxCommandEvent& event)
{
	wxString cmd = wxT("cassetteplayer new");
	if (!m_cassetteAutoCreate) {
		wxString path = wxT("Cassette files (*.wav)|*.wav|All files|*.*||");
		wxString defaultpath = ::wxPathOnly(media[CAS]->control.GetValue());
		wxFileDialog filedlg(this, wxT("Select Cassettefile to save to"),
		                     defaultpath, wxT(""), path,
		                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (filedlg.ShowModal() != wxID_OK) return;
		cmd += wxT(" ") + utils::ConvertPathForCommand(filedlg.GetPath());
	}
	m_controller.WriteCommand(cmd);
}

void SessionPage::OnMotorControl(wxCommandEvent& event)
{
	// TODO m_cassetteControl should be queried from openmsx
	//    -> before this patch it was not even initialized
	//    -> if this is changed in openMSX itself catapult gets out-of-sync
	// Would it be a good idea to just drop this function from catapult? Is
	// this really something that's useful for typical catapult users?
	m_cassetteControl = !m_cassetteControl;
	if (m_cassetteControl) {
		m_controller.WriteCommand(wxT("cassetteplayer motorcontrol on"));
	} else {
		m_controller.WriteCommand(wxT("cassetteplayer motorcontrol off"));
	}
}

void SessionPage::OnAutoCassettefile(wxCommandEvent& event)
{
	m_cassetteAutoCreate = !m_cassetteAutoCreate;
	auto& config = ConfigurationData::instance();
	config.SetParameter(ConfigurationData::CD_AUTORECORD, (long)(m_cassetteAutoCreate ? 1 : 0));
	config.SaveData();
}

void SessionPage::OnBrowseDiskA(wxCommandEvent& event)
{
	BrowseDisk(*media[DISKA]);
}
void SessionPage::OnBrowseDiskB(wxCommandEvent& event)
{
	BrowseDisk(*media[DISKB]);
}
void SessionPage::OnBrowseCartA(wxCommandEvent& event)
{
	BrowseCart(*media[CARTA]);
}
void SessionPage::OnBrowseCartB(wxCommandEvent& event)
{
	BrowseCart(*media[CARTB]);
}

void SessionPage::BrowseDisk(MediaInfo& m)
{
	BrowseMedia(m,
		wxT("All known disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.dmk;*.DMK;*.zip;*.ZIP;*.gz;*.GZ;*.di1;*.DI1;*.di2;*.DI2;*.fd1;*.FD1;*.fd2;*.FD2|Uncompressed disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.di1;*.DI1;*.di2;*.DI2;*.fd1;*.FD1;*.fd2;*.FD2|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||"),
		wxT("Select disk image"));
}

void SessionPage::BrowseCart(MediaInfo& m)
{
	BrowseMedia(m,
		wxT("All known cartridge files|*.rom;*.ROM;*.ri;*.RI;*.mx1;*.MX1;*.mx2;*.MX2;*.zip;*.ZIP;*.gz;*.GZ|Uncompressed cartridge files|*.rom;*.ROM;*.ri;*.RI;*.mx1;*.MX1;*.mx2;*.MX2|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||"),
		wxT("Select ROM image"));
}

void SessionPage::OnBrowseCassette(wxCommandEvent& event)
{
	BrowseMedia(*media[CAS],
		wxT("All known cassette files|*.zip;*.ZIP;*.gz;*.GZ;*.wav;*.WAV;*.cas;*.CAS|Uncompressed cassette files|*.wav;*.WAV;*.cas;*.CAS|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||"),
		wxT("Select cassette image"));
}

void SessionPage::OnBrowseHardDisk(wxCommandEvent& event)
{
	BrowseMedia(*media[HDD],
		wxT("All known hard disk files|*.dsk;*.DSK;*.zip;*.ZIP;*.gz;*.GZ;*.hdd;*.HDD;|Uncompressed disk files|*.dsk;*.DSK;*.hdd;*.HDD;|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||"),
		wxT("Select hard disk image"));
}
void SessionPage::BrowseMedia(MediaInfo& m, const wxString& path, const wxString title)
{
	wxString defaultpath = ::wxPathOnly(m.control.GetValue());
	wxFileDialog filedlg(this, title, defaultpath, wxT(""), path, wxFD_OPEN);
	if (filedlg.ShowModal() == wxID_OK) {
		m.control.SetValue(filedlg.GetPath());
		insertMediaClear(m);
	}
}

void SessionPage::OnClickDiskACombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[DISKA]);
}
void SessionPage::OnClickDiskBCombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[DISKB]);
}
void SessionPage::OnClickCartACombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[CARTA]);
}
void SessionPage::OnClickCartBCombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[CARTB]);
}
void SessionPage::OnClickCassetteCombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[CAS]);
}
void SessionPage::OnClickHardDiskCombo(wxCommandEvent& event)
{
	ClickMediaCombo(event, *media[HDD]);
}
void SessionPage::ClickMediaCombo(wxCommandEvent& event, MediaInfo& m)
{
	OnClickCombo(event);
	auto* h = static_cast<HistoryData*>(m.control.GetClientObject(event.GetInt()));
	if (m.mediaType == CARTRIDGE) SetMapperType(m, h->type);
	if (m.ipsLabel)               SetIpsList   (m, h->ips);
	insertMedia(m);
}

void SessionPage::SetIpsList(MediaInfo& m, const wxArrayString& ips)
{
	m.ips = ips;
	size_t count = m.ips.GetCount();
	m.menu.SetLabel(m.ipsLabel, (count > 0)
		? wxString::Format(wxT("Select IPS Patches (%d selected)"), count)
		: wxString(wxT("Select IPS Patches (None selected)")));
}

void SessionPage::SetMapperType(MediaInfo& m, const wxString& type)
{
	m.type = type;
	m.menu.SetLabel(m.typeLabel,
		(!type.IsEmpty() && (type.Upper() != wxT("AUTO")))
		? wxT("Select cartridge type (") + type + wxT(")")
		: wxT("Select cartridge type (AUTO)"));
}

static int CompareCaseInsensitive(const wxString& first, const wxString& second)
{
	int result = first.CmpNoCase(second);
	if (result != 0) return result;
	return first.Cmp(second);
}

static wxArrayString split(const wxString& str, wxString separator = wxT("::"))
{
	wxArrayString result;
	int pos = 0;
	while (true) {
		size_t pos2 = str.find(separator, pos);
		if (pos2 == wxString::npos) return result;
		result.Add(str.Mid(pos, pos2 - pos));
		pos = pos2 + 2;
	}
}

void SessionPage::SetupHardware(bool initial, bool reset)
{
	wxString currentMachine;
	wxArrayString currentExtensions;
	if (!initial) {
		currentMachine = m_machineList->GetValue();
		wxArrayInt sel;
		m_extensionList->GetSelections(sel);
		for (int i : sel) {
			currentExtensions.Add(m_extensionList->GetString(i));
		}
	}
	auto& config = ConfigurationData::instance();
	m_machineList->Clear();
	m_machineList->Append(wxT(" <default> "));
	m_extensionList->Clear();
	wxString checkedMachines;
	wxString checkedExtensions;
	config.GetParameter(ConfigurationData::CD_MACHINES, checkedMachines);
	config.GetParameter(ConfigurationData::CD_EXTENSIONS, checkedExtensions);
	if (!checkedMachines.IsEmpty() && !checkedExtensions.IsEmpty() && !reset) {
		m_machineArray   = split(checkedMachines);
		m_extensionArray = split(checkedExtensions);
	} else {
		prepareExtensions();
		prepareMachines();
	}
	m_extensionArray.Sort(CompareCaseInsensitive);
	m_machineArray.Sort(CompareCaseInsensitive);
	for (auto tmp : m_extensionArray) {
		tmp.Replace(wxT("_"), wxT(" "));
		m_extensionList->Append(tmp);
	}
	for (auto tmp : m_machineArray) {
		tmp.Replace(wxT("_"), wxT(" "));
		m_machineList->Append(tmp);
	}
	if (!initial) {
		int sel = m_machineList->FindString(currentMachine);
		if (sel != wxNOT_FOUND) {
			m_machineList->SetSelection(sel);
		} else {
			m_machineList->SetSelection(0);
		}
		for (auto& extension : currentExtensions) {
			sel = m_extensionList->FindString(extension);
			if (sel != wxNOT_FOUND) {
				m_extensionList->SetSelection(sel);
			}
		}
	}
}

void SessionPage::prepareExtensions()
{
	m_extensionArray.Clear();

	wxString cmd;
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_EXECPATH, cmd);

	wxArrayString output;
	int code = wxExecute(cmd + wxT(" -bash -ext"), output);
	if (code != -1) {
		m_extensionArray = output;
	}
}

void SessionPage::prepareMachines()
{
	m_machineArray.Clear();

	wxString cmd;
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_EXECPATH, cmd);

	wxArrayString output;
	int code = wxExecute(cmd + wxT(" -bash -machine"), output);
	if (code != -1) {
		m_machineArray = output;
	}
}

void SessionPage::HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus)
{
	checkLooseFocus(oldFocus, *media[DISKA]);
	checkLooseFocus(oldFocus, *media[DISKB]);
	checkLooseFocus(oldFocus, *media[CARTA]);
	checkLooseFocus(oldFocus, *media[CARTB]);
	checkLooseFocus(oldFocus, *media[CAS]);
}
void SessionPage::checkLooseFocus(wxWindow* oldFocus, MediaInfo& m)
{
	if (oldFocus != &m.control) return;
	if (m.control.GetValue() == m.lastContents) return;
	insertMediaClear(m);
}

// clear both IPS list and mapper-type (if appropriate)
// before inserting media
void SessionPage::insertMediaClear(MediaInfo& m)
{
	if (m.ipsLabel) SetIpsList(m, wxArrayString());
	if (m.mediaType == CARTRIDGE) SetMapperType(m, wxT(""));
	insertMedia(m);
}

// insert media, but leave IPS list and mapper-type intact
void SessionPage::insertMedia(MediaInfo& m)
{
	if (m.mediaType == CASSETTE) SetCassetteMode(wxT("play"));
	wxString contents = m.control.GetValue();
	m.lastContents = contents;
	wxString cmd = m.deviceName + wxT(" ");
	if (!contents.IsEmpty()) {
		cmd += utils::ConvertPathForCommand(contents);
		if (m.mediaType == CARTRIDGE) {
			if (!m.type.IsEmpty()) {
				cmd += wxT(" -romtype ") + m.type;
			}
		}
		for (auto& ips : m.ips) {
			cmd += wxT(" -ips ") + utils::ConvertPathForCommand(ips);
		}
		AddHistory(m);
	} else {
		cmd += wxT("eject");
	}
	m_controller.WriteCommand(cmd);
	SaveHistory();
}

void SessionPage::SetControlsOnLaunch()
{
	m_machineList->Enable(false);
	m_extensionList->Enable(false);
	m_extensionListLabel->Enable(false);
	m_machineListLabel->Enable(false);
	// we have to call it explicitly with a parameter here,
	// because the conditions that SetHardDiskControl relies
	// on aren't properly set here (isOpenMsxRuning is not true yet)
	SetHardDiskControl(true);
}

void SessionPage::SetControlsOnEnd()
{
	m_machineList->Enable(true);
	m_extensionList->Enable(true);
	m_extensionListLabel->Enable(true);
	m_machineListLabel->Enable(true);
	m_playButton->SetValue(false);
	m_recordButton->SetValue(false);
	if (auto* temp = FindWindowByLabel(wxT("Cartridge Slots"))) {
		temp->Enable(true);
	}
	SetCassetteControl();
	SetHardDiskControl();
}

// This method should be called whenever one of these conditions changes:
//  - m_controller.IsOpenMSXRunning()
//  - m_hasCassettePort
//  - media[CAS]->contents.IsEmpty()
//  - play/record button changes state
void SessionPage::SetCassetteControl()
{
	bool enable1 = m_controller.IsOpenMSXRunning() && m_hasCassettePort;
	m_recordButton->Enable(enable1 && !m_recordButton->GetValue());

	bool enable2 = enable1 && !media[CAS]->control.GetValue().IsEmpty();
	m_playButton  ->Enable(enable2 && !m_playButton->GetValue());
	m_rewindButton->Enable(enable2);

	bool enable3 = !m_controller.IsOpenMSXRunning() || m_hasCassettePort;
	m_cassetteButton  ->Enable(enable3);
	m_clearCassette   ->Enable(enable3);
	m_browseCassette  ->Enable(enable3);
	media[CAS]->control.Enable(enable3);
	if (auto* temp = FindWindowByLabel(wxT("Cassette Player"))) {
		temp->Enable(enable3);
	}
}

void SessionPage::SetHardDiskControl(bool forcedDisable)
{
	bool enable = (!m_controller.IsOpenMSXRunning()) && !forcedDisable;
	m_clearHardDisk ->Enable(enable);
	m_browseHardDisk->Enable(enable);
	media[HDD]->control.Enable(enable);
}

wxArrayString SessionPage::getHardware() const
{
	wxArrayString result;
	int pos = m_machineList->GetSelection();
	result.Add(pos <= 0
		? wxT(" <default> ")
		: utils::ConvertPathNoSlash(m_machineArray[pos - 1]));

	wxArrayInt sel;
	m_extensionList->GetSelections(sel);
	for (int i : sel) {
		result.Add(m_extensionArray[i]);
	}
	return result;
}

void SessionPage::OnMachineOrExtensionListChanged(wxCommandEvent& event)
{
	SaveHistory();
}

void SessionPage::AddHistory(MediaInfo& m)
{
	wxString currentItem = m.control.GetValue();
#ifdef __WXMSW__
	currentItem.Replace(wxT("/"), wxT("\\"));
#else
	currentItem.Replace(wxT("\\"), wxT("/"));
#endif

	int pos = m.control.FindString(currentItem);
	if (pos != wxNOT_FOUND) {
		m.control.Delete(pos);
	}
	m.control.Insert(currentItem, 0, new HistoryData(m.type, m.ips));
	while (m.control.GetCount() > HISTORY_SIZE) {
		m.control.Delete(HISTORY_SIZE);
	}
	m.control.SetSelection(0);
}

void SessionPage::RestoreHistory()
{
	auto& config = ConfigurationData::instance();
	int insertedMedia;
	config.GetParameter(ConfigurationData::CD_MEDIAINSERTED, &insertedMedia);
	for (auto& m : media) {
		m->control.Clear();
		wxString value;
		config.GetParameter(m->confId, value);
		m->control.Append(split(value));

		wxString typesStr;
		if (m->typeId) config.GetParameter(m->typeId, typesStr);
		wxArrayString types = split(typesStr);

		wxString ipsListStr;
		if (m->ipsId) config.GetParameter(m->ipsId, ipsListStr);
		wxArrayString ipsList = split(ipsListStr, wxT("::")); // 1st-level

		for (unsigned i = 0; i < m->control.GetCount(); ++i) {
			wxString type = (i < types  .GetCount()) ? types  [i] : wxT("");
			wxString ips  = (i < ipsList.GetCount()) ? ipsList[i] : wxT("");
			m->control.SetClientObject(i, new HistoryData(
				type,
				split(ips, wxT(";;")))); // 2nd-level
		}
		if ((insertedMedia & m->mediaBits) && !m->control.IsListEmpty()) {
			m->control.SetSelection(0);
			auto* h = static_cast<HistoryData*>(m->control.GetClientObject(0));
			if (m->mediaType == CARTRIDGE) SetMapperType(*m, h->type);
			if (m->ipsLabel)               SetIpsList   (*m, h->ips);
		} else {
			m->control.SetValue(wxT(""));
		}
	}
	wxString usedMachine;
	config.GetParameter(ConfigurationData::CD_USEDMACHINE, usedMachine);
	if (!usedMachine.IsEmpty()) {
		usedMachine.Replace(wxT("_"), wxT(" "));
		usedMachine.Replace(wxT("\""), wxT(""));
		int pos = m_machineList->FindString(usedMachine);
		if (pos != wxNOT_FOUND) {
			m_machineList->SetSelection(pos);
		} else {
			m_machineList->SetSelection(0);
		}
	}
	wxString usedExtensions;
	config.GetParameter(ConfigurationData::CD_USEDEXTENSIONS, usedExtensions);
	for (auto& temp : split(usedExtensions)) {
		temp.Replace(wxT("_"), wxT(" "));
		if (m_extensionList->FindString(temp) != wxNOT_FOUND) {
			m_extensionList->SetStringSelection(temp);
		}
	}
}

void SessionPage::SaveHistory()
{
	auto& config = ConfigurationData::instance();
	int insertedMedia = 0;
	for (auto& m : media) {
		wxString files, types, ipsLists;
		for (unsigned j = 0; j < m->control.GetCount(); ++j) {
			files << m->control.GetString(j) << wxT("::");
			auto* h = static_cast<HistoryData*>(m->control.GetClientObject(j));
			types << h->type << wxT("::");
			for (auto& ips : h->ips) {
				ipsLists << ips << wxT(";;"); // 2nd-level
			}
			ipsLists << wxT("::"); // 1st-level
		}
		config.SetParameter(m->confId, files);
		if (m->typeId) config.SetParameter(m->typeId, types);
		if (m->ipsId)  config.SetParameter(m->ipsId, ipsLists);
		if (!m->control.GetValue().IsEmpty()) insertedMedia |= m->mediaBits;
	}
	config.SetParameter(ConfigurationData::CD_MEDIAINSERTED, (long)insertedMedia);

	wxArrayString hardware = getHardware();
	wxString usedExtensions;
	for (size_t i = 1; i < hardware.GetCount(); ++i) {
		usedExtensions << hardware[i] << wxT("::");
	}
	config.SetParameter(ConfigurationData::CD_USEDMACHINE, hardware[0]);
	config.SetParameter(ConfigurationData::CD_USEDEXTENSIONS, usedExtensions);
	if (!config.SaveData()) {
		wxMessageBox(wxT("Error saving configuration data"));
	}
}

void SessionPage::EnableCassettePort(wxString data)
{
	// result of   catch {cassetteplayer}
	//   0 -> cassetteplayer command exists,         enable  cassette stuff
	//   1 -> cassetteplayer command does not exist, disbale cassette stuff
	m_hasCassettePort = data == wxT("0");
	SetCassetteControl();
}

void SessionPage::SetCassetteMode(wxString data)
{
	m_playButton->SetValue(!media[CAS]->control.GetValue().IsEmpty() &&
	                       (data == wxT("play")));
	m_recordButton->SetValue(data == wxT("record"));
	SetCassetteControl();
}

SessionPage::MediaInfo* SessionPage::GetLastMenuTarget() const
{
	for (auto& m : media) {
		if (m_lastUsedPopup == m->button) return m.get();
	}
	return nullptr;
}

void SessionPage::OnClickMediaMenu(wxCommandEvent& event)
{
	m_lastUsedPopup = (wxButton*)event.GetEventObject();
	auto* m = GetLastMenuTarget(); assert(m);
	bool enable = !m->control.GetValue().IsEmpty();
	if (m->ipsLabel)  m->menu.Enable(m->ipsLabel,  enable);
	if (m->typeLabel) m->menu.Enable(m->typeLabel, enable);
	if (m->mediaType == CASSETTE) {
		m->menu.Enable(Cas_Rewind, m_rewindButton->IsEnabled());
		bool enable = m_controller.IsOpenMSXRunning() && m_hasCassettePort;
		m->menu.Enable(Cas_MotorControl, enable);
		m->menu.Check(Cas_MotorControl, m_cassetteControl);
		m->menu.Check(Cas_AutoCreateFile, m_cassetteAutoCreate);
	}
	if (m->mediaType == HARDDISK) {
		m->menu.Enable(HardDisk_Browse_File, !m_controller.IsOpenMSXRunning());
		m->menu.Enable(HardDisk_Eject, !m_controller.IsOpenMSXRunning());
	}
	wxRect myRect = m_lastUsedPopup->GetRect();
	PopupMenu(&m->menu, myRect.GetLeft(), myRect.GetBottom());
}

void SessionPage::OnInsertEmptyDiskByMenu(wxCommandEvent& event)
{
	if (auto* target = GetLastMenuTarget()) {
		wxString defaultpath = ::wxPathOnly(target->control.GetValue());
		wxFileDialog filedlg(this, wxT("Create disk image"),
		                     defaultpath, wxT(""), wxT("*.*"),
		                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (filedlg.ShowModal() == wxID_OK) {
			target->control.SetValue(filedlg.GetPath());
			insertMediaClear(*target);
		}
	}
}

void SessionPage::OnBrowseCartByMenu(wxCommandEvent& event)
{
	if (m_lastUsedPopup == m_cartAButton) {
		OnBrowseCartA(event);
	} else if (m_lastUsedPopup == m_cartBButton) {
		OnBrowseCartB(event);
	}
}

void SessionPage::OnBrowseDiskByMenu(wxCommandEvent& event)
{
	if (m_lastUsedPopup == m_diskAButton) {
		OnBrowseDiskA(event);
	} else if (m_lastUsedPopup == m_diskBButton) {
		OnBrowseDiskB(event);
	}
}

void SessionPage::OnBrowseDiskDirByMenu(wxCommandEvent& event)
{
	if (auto* target = GetLastMenuTarget()) {
		wxString defaultpath = ::wxPathOnly(target->control.GetValue());
		wxDirDialog dirdlg(this, wxT("Select Directory to use as disk"), defaultpath);
		if (dirdlg.ShowModal() == wxID_OK) {
			target->control.SetValue(dirdlg.GetPath());
			insertMediaClear(*target);
		}
	}
}

void SessionPage::OnSelectMapper(wxCommandEvent& event)
{
	if (auto* target = GetLastMenuTarget()) {
		m_romTypeDialog->CenterOnParent();
		if (m_romTypeDialog->Present(target->type) == wxID_OK) {
			SetMapperType(*target, m_romTypeDialog->GetSelectedType());
			insertMedia(*target);
		}
	}
}

void SessionPage::OnBrowseIps(wxCommandEvent& event)
{
	if (auto* target = GetLastMenuTarget()) {
		m_ipsDialog->CenterOnParent();
		if (m_ipsDialog->Present(target->ips, target->ipsdir) == wxID_OK) {
			SetIpsList(*target, m_ipsDialog->GetIPSList());
			target->ipsdir = m_ipsDialog->GetLastBrowseLocation();
			insertMedia(*target);
		}
	}
}

void SessionPage::GetRomTypes()
{
	// TODO get the descriptions from openMSX

	wxString cmd;
	ConfigurationData::instance().GetParameter(ConfigurationData::CD_EXECPATH, cmd);

	wxArrayString output;
	int code = wxExecute(cmd + wxT(" -bash -romtype"), output);
	assert(code != -1);
	if (code) {}; // avoid warning in build with asserts disabled

	output.Sort(CompareCaseInsensitive);
	output.Insert(wxT(""), 0);
	output.Insert(wxT("AUTO"), 1);

	std::map<std::string, std::string> descriptions = {
		{"", "Autodetect type"},
		{"AUTO", "Autodetect type"},
		{"Page0", "Plain 16 kB Page 0"},
		{"Page1", "Plain 16 kB Page 1"},
		{"Page2", "Plain 16 kB Page 2 (Basic)"},
		{"Normal8000", "Plain 16 kB Page 2 (Basic)"},
		{"Page3", "Plain 16 kB Page 3"},
		{"Page01", "Plain 32 kB Page 0-1"},
		{"Page12", "Plain 32 kB Page 1-2"},
		{"Page23", "Plain 32 kB Page 2-3"},
		{"Page012", "Plain 48 kB Page 0-2"},
		{"Page123", "Plain 48 kB Page 1-3"},
		{"Page0123", "Plain 64 kB"},
		{"Mirrored", "Plain (Any size)"},
		{"8kB", "Generic 8kB"},
		{"MSXDOS2", "MSX-DOS 2"},
		{"Konami", "Konami without SCC"},
		{"KonamiSCC", "Konami with SCC"},
		{"ASCII8", "Ascii 8kB"},
		{"ASCII16", "Ascii 16kB"},
		{"Padial8", "Padial 8kB"},
		{"Padial16", "Padial 16kB"},
		{"R-Type", "R-Type"},
		{"CrossBlaim", "Cross Blaim"},
		{"MSX-AUDIO", "MSX-Audio"},
		{"HarryFox", "Harry Fox"},
		{"Halnote", "Halnote"},
		{"Playball", "Playball"},
		{"Zemina80in1", "Zemina Multigame (80 in 1)"},
		{"Zemina90in1", "Zemina Multigame (90 in 1)"},
		{"Zemina126in1", "Zemina Multigame (126 in 1)"},
		{"HolyQuran", "Holy Qu'ran"},
		{"ASCII16SRAM2", "Ascii 16kB with SRAM"},
		{"ASCII8SRAM8", "Ascii 8kB with SRAM"},
		{"KoeiSRAM8", "Koei with 8kB SRAM"},
		{"KoeiSRAM32", "Koei with 32kB SRAM"},
		{"Wizardry", "Wizardry"},
		{"GameMaster2", "Konami Game Master 2"},
		{"Synthesizer", "Konami Synthesizer"},
		{"Majutsushi", "Hai no Majutsushi"},
		{"KeyboardMaster", "Konami Keyboard Master"}
	};

	for (wxString& type : output) {
		wxString description = type;
		std::string stdType = std::string(type.mb_str());
		if (descriptions.find(stdType) != descriptions.end()) {
			description = wxString(descriptions[stdType].c_str(), wxConvUTF8);
		}
		SetupRomType(type, description);
	}
}

void SessionPage::SetupRomType(wxString type, wxString fullname)
{
	m_romTypeDialog->AddRomType(type);
	m_romTypeDialog->SetRomTypeFullName(type, fullname);
}

const wxArrayString& SessionPage::GetDetectedMachines() const
{
	return m_machineArray;
}
const wxArrayString& SessionPage::GetDetectedExtensions() const
{
	return m_extensionArray;
}

SessionDropTarget::SessionDropTarget(SessionPage::MediaInfo* target, SessionPage* sessionPage)
	: m_target(target)
	, m_sessionPage(sessionPage)
{
}

bool SessionDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	if (!filenames.IsEmpty()) {
		m_target->control.SetValue(filenames[0]); // just the first for starters
		m_sessionPage->insertMediaClear(*m_target);
	}
	return true;
}

wxString SessionPage::getStartupCommandLineOptions() const
{
	wxString result;

	// EXTENSIONS start from index 1 in hardware!
	wxArrayString hardware = getHardware();

	// Ooooww.... we're using representation here as useful data! :(
	if (hardware[0] != wxT(" <default> ")) {
		result += wxT(" -machine ") + hardware[0];
	}

	// Here's a hack to make sure that slotexpander is always mentioned
	// first. Note that this will break terribly if someone renames the
	// slotexpander extension. Hence the 'hack' remark.
	int pos = hardware.Index(wxT("slotexpander"), false);
	if (pos != wxNOT_FOUND) {
		// put it first (at item 1)
		hardware.RemoveAt(pos);
		hardware.Insert(wxT("slotexpander"), 1);
	}
	for (unsigned i = 1; i < hardware.GetCount(); ++i) {
		result += wxT(" -ext ") + hardware[i];
	}

	for (auto& m : media) {
		wxString image = m->control.GetValue();
		if (image.IsEmpty()) continue;

		wxString option = (m->mediaType != CARTRIDGE)
		                ? m->deviceName
		                : wxT("cart"); // HACK: 'cart' instead of 'carta/b'
		result += wxT(" -") + option + wxT(" \"") + image + wxT("\"");

		if (!m->type.IsEmpty()){
			result += wxT(" -romtype ") + m->type;
		}

		for (auto& ips : m->ips) {
			result += wxT(" -ips \"") + ips + wxT("\"");
		}
	}
	return result;
}
