// $Id: SessionPage.cpp,v 1.21 2004/08/26 16:06:29 h_oudejans Exp $
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

#include "SessionPage.h"
#include "ConfigurationData.h"
#include "wxCatapultFrm.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "MiscControlPage.h"
#include "VideoControlPage.h"
#include "openMSXController.h"

#ifdef __WINDOWS__
#include <shlobj.h>
#endif

IMPLEMENT_CLASS(SessionPage, wxPanel)
BEGIN_EVENT_TABLE(SessionPage, wxPanel)
	EVT_COMBOBOX(XRCID("DiskAContents"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("DiskBContents"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("CartAContents"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("CartBContents"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("Tape1Contents"),CatapultPage::OnClickCombo)
	EVT_COMBOBOX(XRCID("Tape2Contents"),CatapultPage::OnClickCombo)
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

SessionPage::SessionPage(wxWindow * parent, openMSXController * controller)
:CatapultPage(parent)
{
	m_controller = controller;
	wxXmlResource::Get()->LoadPanel(this, parent, wxT("SessionPage"));

	m_parent = (wxCatapultFrame *)parent->GetParent()->GetParent();
	m_diskA = (wxComboBox *)FindWindow(wxT("DiskAContents"));
	m_diskB = (wxComboBox *)FindWindow(wxT("DiskBContents"));
	m_cartA = (wxComboBox *)FindWindow(wxT("CartAContents"));
	m_cartB = (wxComboBox *)FindWindow(wxT("CartBContents"));
	m_tape1 = (wxComboBox *)FindWindow(wxT("NormalTapeContents"));
	m_tape2 = (wxComboBox *)FindWindow(wxT("CasPatchContents"));
	m_extensionList = (wxListBox *)FindWindow(wxT("ExtensionList"));
	m_machineList = (wxComboBox *)FindWindow(wxT("MachineList"));
	m_browseDiskA = (wxBitmapButton *)FindWindow(wxT("BrowseDiskA"));
	m_browseDiskB = (wxBitmapButton *)FindWindow(wxT("BrowseDiskB"));
	m_browseCartA = (wxBitmapButton *)FindWindow(wxT("BrowseCartA"));
	m_browseCartB = (wxBitmapButton *)FindWindow(wxT("BrowseCartB"));
	m_browseTape1 = (wxBitmapButton *)FindWindow(wxT("BrowseNormalTape"));
	m_browseTape2 = (wxBitmapButton *)FindWindow(wxT("BrowseCasPatch"));
	m_clearDiskA = (wxBitmapButton *)FindWindow(wxT("ClearDiskA"));
	m_clearDiskB = (wxBitmapButton *)FindWindow(wxT("ClearDiskB"));
	m_clearCartA = (wxBitmapButton *)FindWindow(wxT("ClearCartA"));
	m_clearCartB = (wxBitmapButton *)FindWindow(wxT("ClearCartB"));
	m_clearTape1 = (wxBitmapButton *)FindWindow(wxT("ClearNormalTape"));
	m_clearTape2 = (wxBitmapButton *)FindWindow(wxT("ClearCasPatch"));

	m_lastDiskA = "";
	m_lastDiskB = "";
	m_lastTape1 = "";
	m_lastTape2 = "";

	SetupHardware(true);
	
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
		tempDC->GetTextExtent(wxString(m_extensionList->GetString(index) + "W"),&w,&h);
		if (w > wMax){
			wMax = w;
		}
	}
	m_extensionList->SetSizeHints(wMax + wxSystemSettings::GetSystemMetric(wxSYS_HSCROLL_ARROW_X),-1);
	delete tempDC;



	// let's put a mask on all the bitmapbuttons

	wxBitmapButton * buttons[] = {m_browseDiskA, m_browseDiskB,
		m_browseCartA, m_browseCartB, m_browseTape1, m_browseTape2,
		m_clearDiskA, m_clearDiskB, m_clearCartA, m_clearCartB,
		m_clearTape1, m_clearTape2};

		for (int i=0;i<12;i++)
		{
			wxBitmap & temp = buttons[i]->GetBitmapLabel();
			temp.SetMask(new wxMask(temp,wxColour(255,0,255)));
			buttons[i]->Enable(false);
			buttons[i]->Enable(true);
		}
	RestoreHistory();
	m_cassettePortState = "disabled";
}

SessionPage::~SessionPage()
{
}

void SessionPage::OnClearDiskA(wxCommandEvent &event)
{
	m_diskA->SetValue(wxT(""));
	m_lastDiskA = wxT("");
	m_controller->WriteCommand("diska eject");
}

void SessionPage::OnClearDiskB(wxCommandEvent &event)
{
	m_diskB->SetValue(wxT(""));
	m_lastDiskB = wxT("");
	m_controller->WriteCommand("diskb eject");
}

void SessionPage::OnClearCartA(wxCommandEvent &event)
{
	m_cartA->SetValue(wxT(""));
}

void SessionPage::OnClearCartB(wxCommandEvent &event)
{
	m_cartB->SetValue(wxT(""));
}

void SessionPage::OnClearNormalTape(wxCommandEvent &event)
{
	m_tape1->SetValue(wxT(""));
	m_lastTape1 = wxT("");
	m_controller->WriteCommand("cassetteplayer eject");
}

void SessionPage::OnClearCasPatch(wxCommandEvent &event)
{
	m_tape2->SetValue(wxT(""));
	m_lastTape2 = wxT("");
	m_controller->WriteCommand("cas eject");
}

void SessionPage::OnBrowseDiskA(wxCommandEvent &event)
{
	if (BrowseDisk (m_diskA, wxT("diska"), ::wxPathOnly(m_diskA->GetValue()))) {
		m_lastDiskA = m_diskA->GetValue();
	}
}

void SessionPage::OnBrowseDiskB(wxCommandEvent &event)
{
	if (BrowseDisk (m_diskB, wxT("diskb"), ::wxPathOnly(m_diskA->GetValue()))) {
		m_lastDiskB = m_diskB->GetValue();
	}
}

bool SessionPage::BrowseDisk(wxComboBox *target, wxString devicename, wxString defaultpath)
{
	wxString path;
#ifndef __MOTIF__
	path = wxT("All known disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.zip;*.ZIP;*.gz;*.GZ;*.di1;*.DI1;*.di2;*.DI2|Uncompressed disk files|*.dsk;*.DSK;*.xsa;*.XSA;*.di1;*.DI1;*.di2;*.DI2|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

 	wxFileDialog filedlg(this,_("Select disk image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		target->SetValue (filedlg.GetPath());
		m_controller->WriteCommand(wxString(devicename + " eject"));
		if (!target->GetValue().IsEmpty()) {
			m_controller->WriteCommand(devicename + " " + ConvertPath(target->GetValue(),true));
			if (m_controller->IsOpenMSXRunning()) {
				AddHistory(target);
				SaveHistory();
			}
		}
		return true;
	}
	return false;
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
	path = wxT("All known cartridge files|*.rom;*.ROM;*.ri;*.RI;*.zip;*.ZIP;*.gz;*.GZ|Uncompressed cartridge files|*.rom;*.ROM;*.ri;*.RI|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,_("Select ROM image"), defaultpath, wxT(""), path ,wxOPEN);
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
	path = wxT("All known tape (sound) files|*.zip;*.ZIP;*.gz;*.GZ;*.wav;*.WAV|Uncompressed tape (sound) files|*.wav;*.WAV|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,wxT("Select tape sound image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_tape1->SetValue (filedlg.GetPath());	
		m_controller->WriteCommand("cassetteplayer eject");
		if (!m_tape1->GetValue().IsEmpty()) {
			if ((m_cassettePortState != "disabled") && 
				(m_cassettePortState != "cassetteplayer")) {
					m_controller->WriteCommand("plug cassetteport cassetteplayer");
			}
			m_controller->WriteCommand("cassetteplayer " + ConvertPath(m_tape1->GetValue(),true));
			if (m_controller->IsOpenMSXRunning()) {
				AddHistory(m_tape1);
				SaveHistory();
			}
		}
		m_lastTape1 = m_tape1->GetValue();
	}
}

void SessionPage::OnBrowseCasPatch(wxCommandEvent &event)
{
	wxString defaultpath = ::wxPathOnly(m_tape2->GetValue());
	wxString path;
#ifndef __MOTIF__
	path = wxT("All known cassette files|*.zip;*.ZIP;*.gz;*.GZ;*.cas;*.CAS|Uncompressed cassette files|*.cas;*.CAS|Compressed files (*.zip;*.gz)|*.gz;*.GZ;*.zip;*.ZIP|All files|*.*||");
#else
	path = wxT("*.*");
#endif

	wxFileDialog filedlg(this,_("Select cas image"), defaultpath, wxT(""), path ,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_tape2->SetValue (filedlg.GetPath());
		m_controller->WriteCommand("cas eject");
		if (!m_tape2->GetValue().IsEmpty()) {
			m_controller->WriteCommand("cas " + ConvertPath(m_tape2->GetValue(),true));
			if (m_controller->IsOpenMSXRunning()) {
				AddHistory(m_tape2);
				SaveHistory();
			}
		}
		m_lastTape2 = m_tape2->GetValue();
	}
	
}

void SessionPage::SetupHardware (bool initial)
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
	wxString sharepath;
	config->GetParameter(ConfigurationData::CD_SHAREPATH,sharepath);
	m_machineList->Clear();
	m_machineList->Append(wxT(" <default> "));
	m_extensionList->Clear();

	m_machineArray.Clear();
	m_extensionArray.Clear();

	prepareExtensions (sharepath, m_extensionArray);
	prepareMachines (sharepath, m_machineArray);
	wxString personalShare; 
#ifdef __UNIX__
	personalShare = ::wxGetHomeDir() + "/.openMSX/share";
#else
	char temp[MAX_PATH + 1];
	SHGetSpecialFolderPath(0,temp,CSIDL_PERSONAL, 1);
	personalShare = wxString(temp) + "/openMSX/share";
#endif
	prepareExtensions (personalShare, m_extensionArray, true);
	prepareMachines (personalShare, m_machineArray, true);

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
	int result = first.CmpNoCase(second.c_str());
	if (result != 0) return result;

	return first.Cmp(second.c_str());
}

void SessionPage::prepareExtensions(wxString sharepath, wxArrayString & extensionArray, bool optional)
{
	if (!::wxDirExists(sharepath + wxT("/extensions"))) {
		if (!optional) {
			wxString msg;
			msg.sprintf(_("Directory: %s does not exist"), wxString(sharepath + wxT("/extensions")).c_str());
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
			extensionArray.Add(extension);
		}
		succes = sharedir.GetNext(&extension);
	}
}

void SessionPage::fillExtensions (wxArrayString & extensionArray)
{
	wxString temp;
	for (unsigned int i=0;i<extensionArray.GetCount();i++)
	{
		if (m_extensionList->FindString (extensionArray[i]) == -1)
			temp = extensionArray[i];
			temp.Replace("_"," ",true);
			m_extensionList->Append(temp);
	}

}

void SessionPage::prepareMachines(wxString sharepath, wxArrayString & machineArray, bool optional)
{
	if (!::wxDirExists(sharepath + wxT("/machines"))) {
		if (!optional) {
			wxString msg;
			msg.sprintf(_("Directory: %s does not exist"), wxString(sharepath + wxT("/machines")).c_str());
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
			machineArray.Add(machine);
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
		if (m_machineList->FindString (machineArray[i]) == -1)
			temp = machineArray[i];
			temp.Replace("_"," ",true);
			m_machineList->Append(temp);
	}
}

void SessionPage::HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)
{
	if (oldFocus == m_diskA) {
		if (m_diskA->GetValue() != m_lastDiskA) {
			m_controller->WriteCommand("diska eject");
			if (!m_diskA->GetValue().IsEmpty()) {
				m_controller->WriteCommand("diska " + ConvertPath(m_diskA->GetValue(),true));
				AddHistory(m_diskA);
			}
			m_lastDiskA = m_diskA->GetValue();

		}	
	}
	else if (oldFocus == m_diskB) {
		if (m_diskB->GetValue() != m_lastDiskB) {
			m_controller->WriteCommand("diskb eject");
			if (!m_diskA->GetValue().IsEmpty()) {
				m_controller->WriteCommand("diskb " + ConvertPath(m_diskB->GetValue(),true));
				AddHistory(m_diskB);
			}
			m_lastDiskB = m_diskB->GetValue();
		}
	}
	else if (oldFocus == m_tape1) {
		if (m_tape1->GetValue() != m_lastTape1) {
			m_controller->WriteCommand("cassetteplayer eject");
			if (!m_tape1->GetValue().IsEmpty()) {
				m_controller->WriteCommand("cassetteplayer " + ConvertPath(m_tape1->GetValue(),true));
				AddHistory(m_tape1);
			}
			m_lastTape1 = m_tape1->GetValue();
		}
	}
	else if (oldFocus == m_tape2) {
		if (m_tape2->GetValue() != m_lastTape2) {
			m_controller->WriteCommand("cas eject");
			if (!m_tape2->GetValue().IsEmpty()) {
				m_controller->WriteCommand("cas " + ConvertPath(m_tape2->GetValue(),true));
				AddHistory(m_tape1);
			}
			m_lastTape2 = m_tape2->GetValue();
		}
	}
	SaveHistory();
}

void SessionPage::SetControlsOnLaunch()
{
	m_machineList->Enable(false);
	m_extensionList->Enable(false);
	m_cartA->Enable(false);
	m_cartB->Enable(false);
	m_clearCartA->Enable(false);
	m_clearCartB->Enable(false);
	m_browseCartA->Enable(false);
	m_browseCartB->Enable(false);		
}

void SessionPage::SetControlsOnEnd()
{
	m_machineList->Enable(true);
	m_extensionList->Enable(true);
	m_cartA->Enable(true);
	m_cartB->Enable(true);
	m_clearCartA->Enable(true);
	m_clearCartB->Enable(true);
	m_browseCartA->Enable(true);
	m_browseCartB->Enable(true);
	m_cassettePortState = wxT("disabled");
}

void SessionPage::getMedia(wxArrayString & parameters)
{
	wxComboBox * box [6] = {m_diskA,m_diskB,m_cartA,m_cartB,m_tape1,m_tape2};
	parameters.Clear();
	for (int i=0;i<6;i++) {
		parameters.Add(wxT(""));
		if (!box[i]->GetValue().IsEmpty()) {
			parameters[i] = box[i]->GetValue();
		}
	}
}

void SessionPage::getHardware(wxArrayString & parameters)
{
	parameters.Clear();
	int pos = m_machineList->GetSelection();
	if (pos == 0){
		parameters.Add(ConvertPath(m_machineList->GetValue(),true));
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
	wxComboBox * box [6] = {m_diskA, m_diskB, m_cartA, m_cartB, m_tape1,
		m_tape2};
	int flags [6] = {ConfigurationData::MB_DISKA,
		ConfigurationData::MB_DISKB,
		ConfigurationData::MB_CARTA,ConfigurationData::MB_CARTB,
		ConfigurationData::MB_TAPE1,ConfigurationData::MB_TAPE2};
	unsigned int i;
	m_InsertedMedia = 0;
	for (i=0;i<6;i++) {
		if (!box[i]->GetValue().IsEmpty()) {
			AddHistory (box[i]);
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

void SessionPage::AddHistory(wxComboBox *media)
{
	// wxWindows 2.4 does not support insertion in a wxComboBox
	// so this is gonna be replaced as soon as 2.5 is stable
	wxString currentItem = media->GetValue();
#ifdef __WINDOWS__
	currentItem.Replace("/","\\",true);
#else
	currentItem.Replace("\\","/",true);
#endif	
	wxArrayString items;
	unsigned int pos = media->FindString(currentItem);
	unsigned int num = media->GetCount();		
	unsigned int i;
	if (num == 0) 
	{
		media->Append(currentItem);
	}
	else
	{
		items.Add(currentItem);
		for (i=0;i<num;i++)
		{
			if (i != pos)
				items.Add(media->GetString(i));
		}
		media->Clear();
		for (i=0;i<items.GetCount();i++)
			media->Append(items[i]);
	}
	media->SetValue(currentItem);
}

void SessionPage::RestoreHistory()
{
	wxString temp;
	wxComboBox * field[6] = {m_diskA, m_diskB, m_cartA, m_cartB, m_tape1, m_tape2};
		ConfigurationData::ID id[6] = {ConfigurationData::CD_HISTDISKA,
			ConfigurationData::CD_HISTDISKB,
			ConfigurationData::CD_HISTCARTA,
			ConfigurationData::CD_HISTCARTB,
			ConfigurationData::CD_HISTTAPE1,
			ConfigurationData::CD_HISTTAPE2};
			ConfigurationData * config = ConfigurationData::instance();
			config->GetParameter(ConfigurationData::CD_MEDIAINSERTED, &m_InsertedMedia);
			wxString value;
			int pos;
			for (int i=0;i<6;i++)
			{
				field[i]->Clear();
				config->GetParameter(id[i],value);
				do
				{
					pos = value.Find(wxT("::"));
					if (pos != -1)
					{
						field[i]->Append(value.Left(pos));
						value = value.Mid(pos + 2);	
					}
				}while (pos !=-1);
				if (m_InsertedMedia & (1 << i)) {
					field[i]->SetSelection(0);
				}
				else {
					field[i]->SetValue(wxT(""));
				}
			}

			config->GetParameter(ConfigurationData::CD_USEDMACHINE, m_usedMachine);
			if (!m_usedMachine.IsEmpty())
			{
				temp = m_usedMachine;
				temp.Replace("_"," ",true);
				temp.Replace("\"","",true);
				int pos = m_machineList->FindString(temp);
				
				if (pos != -1)
					m_machineList->SetSelection (pos);
			}

			config->GetParameter(ConfigurationData::CD_USEDEXTENSIONS,value);
			m_usedExtensions = value;
			do
			{
				pos = value.Find(wxT("::"));
				if (pos != -1)
				{
					temp = value.Left(pos);
					temp.Replace("_"," ",true);
					m_extensionList->SetStringSelection (temp);
					value = value.Mid(pos + 2);	
				}
			} while (pos !=-1);
}

void SessionPage::SaveHistory()
{
	wxComboBox * field[6] = {m_diskA, m_diskB, m_cartA, m_cartB, m_tape1, m_tape2};
	ConfigurationData::ID id[6] = {ConfigurationData::CD_HISTDISKA,
			ConfigurationData::CD_HISTDISKB, ConfigurationData::CD_HISTCARTA,
			ConfigurationData::CD_HISTCARTB, ConfigurationData::CD_HISTTAPE1,
			ConfigurationData::CD_HISTTAPE2};
			ConfigurationData * config = ConfigurationData::instance();
	wxString temp;
	wxString current;
	for (int i=0;i<6;i++) {
		temp.Clear();
		current = field[i]->GetValue();
		field[i]->SetValue(wxT(""));
		for (int j=0;j<field[i]->GetCount();j++) {
			temp += field[i]->GetString (j);
			if (!field[i]->GetString(j).IsEmpty()) {
				temp += wxT("::");
			}
		}
		field[i]->SetValue(current);
		config->SetParameter(id[i],temp);
	}
	config->SetParameter(ConfigurationData::CD_MEDIAINSERTED,(long) m_InsertedMedia);
	config->SetParameter(ConfigurationData::CD_USEDMACHINE,m_usedMachine);
	config->SetParameter(ConfigurationData::CD_USEDEXTENSIONS,m_usedExtensions);
	bool result;
	result = ConfigurationData::instance()->SaveData();
#if !OPENMSX_DEMO_CD_VERSION
	if (!result) {
		wxMessageBox ("Error saving configuration data");
	}
#endif
}

void SessionPage::EnableCassettePort (wxString data)
{
	m_cassettePortState = data;
}

void SessionPage::AutoPlugCassette ()
{
	if (m_tape1->GetValue() != "") {
		if ((m_cassettePortState != wxT("disabled")) &&
			(m_cassettePortState != wxT("cassetteplayers"))) {
			m_controller->WriteCommand ("plug cassetteport cassetteplayer");
		}
	}
}
