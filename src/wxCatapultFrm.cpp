// $Id: wxCatapultFrm.cpp,v 1.11 2004/03/21 13:50:14 manuelbi Exp $ 
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/image.h>

#include "wxCatapultApp.h"
#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "CatapultConfigDlg.h"
#include "PipeReadThread.h"
#include "CatapultXMLParser.h"
#include "SessionPage.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "AudioControlPage.h"
#include "MiscControlPage.h"
#ifdef __WINDOWS__
#include "openMSXWindowsController.h"
#else
#include "openMSXLinuxController.h"
#endif
#include "openMSXController.h"

#define unisprintf sprintf

#include "config.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
	// menu items
	Catapult_Quit = 1,
	Catapult_About,
	Catapult_Edit_Config
};

#define TIMER_ID -1

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
	IMPLEMENT_CLASS(wxCatapultFrame, wxFrame)
BEGIN_EVENT_TABLE(wxCatapultFrame, wxFrame)
	EVT_MENU(Catapult_Quit,  wxCatapultFrame::OnMenuQuit)
	EVT_MENU(Catapult_About, wxCatapultFrame::OnMenuAbout)
	EVT_MENU(Catapult_Edit_Config, wxCatapultFrame::OnMenuEditConfig)
	EVT_COMMAND (-1, EVT_CONTROLLER, wxCatapultFrame::OnControllerEvent)
	EVT_BUTTON(XRCID("Launch_AbortButton"),wxCatapultFrame::OnLaunch)
	EVT_BUTTON(XRCID("ApplyButton"),wxCatapultFrame::OnApplyChanges)
	EVT_BUTTON(XRCID("QuitButton"),wxCatapultFrame::OnMenuQuit)
	EVT_TIMER(TIMER_ID, wxCatapultFrame::OnTimer)
END_EVENT_TABLE()

	// include icon for any non-unix version
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "catapult.xpm"
#endif

	// ------------------------------------------------------------------
	// main frame
	// ------------------------------------------------------------------

	// frame constructor
	wxCatapultFrame::wxCatapultFrame(wxWindow * parent)
: m_timer(this, TIMER_ID)
{
#ifdef __WINDOWS__	
	m_controller = new openMSXWindowsController(this);
#else
	m_controller = new openMSXLinuxController(this);
#endif

	wxXmlResource::Get()->LoadFrame(this, parent, _("CatapultFrame"));
	SetIcon (wxICON(acatapult));

	// create menu bars				jdayeshdfsg

	wxMenu *fileMenu = new wxMenu("", wxMENU_TEAROFF);
	wxMenu *editMenu = new wxMenu("", wxMENU_TEAROFF);
	wxMenu *helpMenu = new wxMenu("", wxMENU_TEAROFF);

	fileMenu->Append(Catapult_Quit, _("&Quit\tCtrl-Q"), _("Quit this program"));
	editMenu->Append(Catapult_Edit_Config, _("Edit &Configuration\tCtrl-C"), _("Adjust Catapult Configuration"));
	helpMenu->Append(Catapult_About, _("&About Catapult...\tCtrl-A"), _("Show about dialog"));

	// now append the freshly created menu to the menu bar...
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, _("&File"));
	menuBar->Append(editMenu, _("&Edit"));
	menuBar->Append(helpMenu, _("&Help"));

	SetMenuBar(menuBar);

	CreateStatusBar(2);
	SetStatusText(_("Ready"));

	// Fill the membervariables with control pointer for easy access

	m_tabControl = (wxNotebook *)FindWindow(_("GlobalTabControl"));
	m_sessionPage = new SessionPage(m_tabControl);
	m_statusPage = new StatusPage(m_tabControl);
	m_miscControlPage = new MiscControlPage(m_tabControl,m_controller);
	m_videoControlPage = new VideoControlPage(m_tabControl,m_controller);
	m_audioControlPage = new AudioControlPage(m_tabControl,m_controller);

	m_tabControl->AddPage(m_sessionPage,_("Session"),true);
	m_tabControl->AddPage(m_statusPage,_("Status Info"),false);
	m_tabControl->AddPage(m_miscControlPage,_("Misc Controls"),false);
	m_tabControl->AddPage(m_videoControlPage,_("Video Controls"),false);
	m_tabControl->AddPage(m_audioControlPage,_("Audio Controls"),false);
	wxWindow * tempwindow = FindWindow(_("MainWindowPanel"));
	wxSize size = tempwindow->GetSizer()->Fit(tempwindow);
	this->GetSizer()->SetMinSize(size);
	this->GetSizer()->Fit(this);
	this->GetSizer()->SetSizeHints(this);

	m_launch_AbortButton = (wxButton *)FindWindow(_("Launch_AbortButton"));
	m_applyButton = (wxButton *)FindWindow(_("ApplyButton"));

	RestoreHistory();
	DisableControls();
	m_launch_AbortButton->Enable(false);
	wxString cmd;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);
	m_controller->StartOpenMSX(cmd,true);
}

// frame destructor
wxCatapultFrame::~wxCatapultFrame()
{
	delete m_controller;
	SaveHistory();
	bool result;
	result = ConfigurationData::instance()->SaveData();
#ifdef OPENMSX_DEMO_CD_VERSION
	if (!result){
		wxMessageBox ("Error saving configuration data");
	}
#endif
}

// event handlers
void wxCatapultFrame::OnMenuQuit(wxCommandEvent& event)
{
	// TRUE is to force the frame to close
	Close(TRUE);
}

void wxCatapultFrame::OnMenuAbout(wxCommandEvent& event)
{
	// called when help - about is picked from the menu or toolbar
	wxString msg;
	msg.Printf(_("openMSX Catapult Neo " VERSION "\n\n"
				"Created by the openMSX team\n\nBuilt on "__DATE__"\n"));

	wxMessageBox(msg, _("About wxCatapult"), wxOK | wxICON_INFORMATION, this);
}

void wxCatapultFrame::OnMenuEditConfig(wxCommandEvent& event)
{
	CatapultConfigDlg dlg;
	dlg.Center();
	dlg.ShowModal();
	m_sessionPage->SetupHardware();
}

void wxCatapultFrame::OnLaunch(wxCommandEvent& event)
{
	if (m_launch_AbortButton->GetLabel().IsSameAs(_("Abort"))){
		m_controller->WriteCommand(_("quit"));
		return;
	}

	m_lastDiskA = _("");
	m_lastDiskB = _("");
	m_lastTape1 = _("");
	m_lastTape2 = _("");

	m_launch_AbortButton->SetLabel(_("Abort"));
	m_launch_AbortButton->Enable(false);

	m_miscControlPage->m_powerButton->SetValue(true);
	wxString cmd;
	ConfigurationData::instance()->GetParameter(ConfigurationData::CD_EXECPATH, cmd);

	if (m_sessionPage->m_machineList->GetValue() != _(" <default> "))
	{
		cmd += " -machine " + m_sessionPage->m_machineList->GetValue();
	}
	m_usedMachine = m_sessionPage->m_machineList->GetValue();

	wxArrayInt sel;
	m_usedExtensions.Clear();
	if (m_sessionPage->m_extensionList->GetSelections(sel) >0)
	{
		for (unsigned int i=0;i<sel.GetCount();i++){
			cmd += _(" -ext ") + m_sessionPage->m_extensionList->GetString(sel[i]);
			m_usedExtensions += m_sessionPage->m_extensionList->GetString(sel[i]);
			m_usedExtensions += _("::");
		}
	}
	m_InsertedMedia = 0;
	if (!m_sessionPage->m_diskA->GetValue().IsEmpty()){
		cmd += " -diska \"" + m_sessionPage->m_diskA->GetValue() + _("\"");
		AddHistory (m_sessionPage->m_diskA);
		m_InsertedMedia |= ConfigurationData::MB_DISKA;
		m_lastDiskA = m_sessionPage->m_diskA->GetValue();
	}
	if (!m_sessionPage->m_diskB->GetValue().IsEmpty()){
		cmd += " -diskb \"" + m_sessionPage->m_diskB->GetValue() + _("\"");
		AddHistory (m_sessionPage->m_diskB);
		m_InsertedMedia |= ConfigurationData::MB_DISKB;
		m_lastDiskB = m_sessionPage->m_diskB->GetValue();
	}
	if (!m_sessionPage->m_cartA->GetValue().IsEmpty()){
		cmd += " -carta \"" + m_sessionPage->m_cartA->GetValue() + _("\"");
		AddHistory (m_sessionPage->m_cartA);
		m_InsertedMedia |= ConfigurationData::MB_CARTA;
	}
	if (!m_sessionPage->m_cartB->GetValue().IsEmpty()){
		cmd += " -cartb \"" + m_sessionPage->m_cartB->GetValue() + _("\"");
		AddHistory (m_sessionPage->m_cartB);
		m_InsertedMedia |= ConfigurationData::MB_CARTB;
	}
	if (!m_sessionPage->m_tape1->GetValue().IsEmpty()){
		cmd += " -cassetteplayer \"" + m_sessionPage->m_tape1->GetValue() + _("\"");
		AddHistory (m_sessionPage->m_tape1);
		m_InsertedMedia |= ConfigurationData::MB_TAPE1;
		m_lastTape1 = m_sessionPage->m_tape1->GetValue();
	}
	if (!m_sessionPage->m_tape2->GetValue().IsEmpty()){
		cmd += " -cas \"" + m_sessionPage->m_tape2->GetValue() +  _("\"");
		AddHistory (m_sessionPage->m_tape2);
		m_InsertedMedia |= ConfigurationData::MB_TAPE2;
		m_lastTape2 = m_sessionPage->m_tape2->GetValue();
	}

	m_controller->StartOpenMSX(cmd);
	EnableControls();
	m_statusPage->m_outputtext->Clear();
}

void wxCatapultFrame::AddHistory(wxComboBox *media)
{
	// wxWindows 2.4 does not support insertion in a wxComboBox
	// so this is gonna be replace as soon as 2.5 stable is

	wxArrayString items;
	unsigned int pos = media->FindString(media->GetValue());
	unsigned int num = media->GetCount();		
	unsigned int i;
	if (num == 0) 
	{
		media->Append(media->GetValue());
	}
	else
	{
		items.Add(media->GetValue());
		for (i=0;i<num;i++)
		{
			if (i != pos)
				items.Add(media->GetString(i));
		}
		media->Clear();
		for (i=0;i<items.GetCount();i++)
			media->Append(items[i]);
	}
	media->SetSelection(0);
}

void wxCatapultFrame::RestoreHistory()
{
	wxComboBox * field[6] = {m_sessionPage->m_diskA, m_sessionPage->m_diskB,
		m_sessionPage->m_cartA, m_sessionPage->m_cartB, 
		m_sessionPage->m_tape1, m_sessionPage->m_tape2};
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
					pos = value.Find(_("::"));
					if (pos != -1)
					{
						field[i]->Append(value.Left(pos));
						value = value.Mid(pos + 2);	
					}
				}while (pos !=-1);
				if (m_InsertedMedia & (1 << i))
					field[i]->SetSelection(0);
			}

			config->GetParameter(ConfigurationData::CD_USEDMACHINE, m_usedMachine);
			if (!m_usedMachine.IsEmpty())
			{
				int pos = m_sessionPage->m_machineList->FindString(m_usedMachine);
				if (pos != -1)
					m_sessionPage->m_machineList->SetSelection (pos);
			}

			config->GetParameter(ConfigurationData::CD_USEDEXTENSIONS,value);
			do
			{
				pos = value.Find(_("::"));
				if (pos != -1)
				{
					m_sessionPage->m_extensionList->SetStringSelection (value.Left(pos));
					value = value.Mid(pos + 2);	
				}
			} while (pos !=-1);
}

void wxCatapultFrame::SaveHistory()
{
	wxComboBox * field[6] = {m_sessionPage->m_diskA, m_sessionPage->m_diskB, 
		m_sessionPage->m_cartA, m_sessionPage->m_cartB, 
		m_sessionPage->m_tape1, m_sessionPage->m_tape2};
		ConfigurationData::ID id[6] = {ConfigurationData::CD_HISTDISKA,
			ConfigurationData::CD_HISTDISKB,
			ConfigurationData::CD_HISTCARTA,
			ConfigurationData::CD_HISTCARTB,
			ConfigurationData::CD_HISTTAPE1,
			ConfigurationData::CD_HISTTAPE2};
			ConfigurationData * config = ConfigurationData::instance();
			wxString temp;
			for (int i=0;i<6;i++)
			{
				temp.Clear();
				field[i]->SetValue(_(""));
				for (int j=0;j<field[i]->GetCount();j++)
				{
					temp += field[i]->GetString (j);
					if (!field[i]->GetString(j).IsEmpty())
						temp += _("::");
				}
				config->SetParameter(id[i],temp);
			}
			config->SetParameter(ConfigurationData::CD_MEDIAINSERTED,(long) m_InsertedMedia);
			config->SetParameter(ConfigurationData::CD_USEDMACHINE,m_usedMachine);
			config->SetParameter(ConfigurationData::CD_USEDEXTENSIONS,m_usedExtensions);
}

void wxCatapultFrame::EnableControls()
{
	m_miscControlPage->EnableControls();
	m_videoControlPage->EnableControls();
	m_sessionPage->m_machineList->Enable(false);
	m_sessionPage->m_extensionList->Enable(false);
	m_sessionPage->m_cartA->Enable(false);
	m_sessionPage->m_cartB->Enable(false);
	m_sessionPage->m_clearCartA->Enable(false);
	m_sessionPage->m_clearCartB->Enable(false);
	m_sessionPage->m_browseCartA->Enable(false);
	m_sessionPage->m_browseCartB->Enable(false);
}

void wxCatapultFrame::DisableControls()
{
	m_miscControlPage->DisableControls();
	m_videoControlPage->DisableControls();
	m_sessionPage->m_machineList->Enable(true);
	m_sessionPage->m_extensionList->Enable(true);
	m_sessionPage->m_cartA->Enable(true);
	m_sessionPage->m_cartB->Enable(true);
	m_sessionPage->m_clearCartA->Enable(true);
	m_sessionPage->m_clearCartB->Enable(true);
	m_sessionPage->m_browseCartA->Enable(true);
	m_sessionPage->m_browseCartB->Enable(true);
}

void wxCatapultFrame::OnApplyChanges(wxCommandEvent &event)
{
	if (m_lastDiskA != m_sessionPage->m_diskA->GetValue()){
		m_controller->WriteCommand(_("diska eject"));
		if (!m_sessionPage->m_diskA->GetValue().IsEmpty())
			m_controller->WriteCommand(_("diska ") + ConvertPath(m_sessionPage->m_diskA->GetValue(),true));
	}
	if (m_lastDiskB != m_sessionPage->m_diskB->GetValue()){
		m_controller->WriteCommand(_("diska eject"));
		if (!m_sessionPage->m_diskB->GetValue().IsEmpty())
			m_controller->WriteCommand(_("diskb ") + ConvertPath(m_sessionPage->m_diskB->GetValue(),true));
	}
	if (m_lastTape1 != m_sessionPage->m_tape1->GetValue()){
		m_controller->WriteCommand(_("cassetteplayer eject"));
		if (!m_sessionPage->m_tape1->GetValue().IsEmpty())
			m_controller->WriteCommand(_("cassetteplayer ") + ConvertPath(m_sessionPage->m_tape1->GetValue(),true));
	}
	if (m_lastTape2 != m_sessionPage->m_tape2->GetValue()){
		m_controller->WriteCommand(_("cas eject"));
		if (!m_sessionPage->m_tape2->GetValue().IsEmpty())
			m_controller->WriteCommand(_("cas ") + ConvertPath(m_sessionPage->m_tape2->GetValue(),true));
	}
	m_lastDiskA = m_sessionPage->m_diskA->GetValue();
	m_lastDiskB = m_sessionPage->m_diskB->GetValue();
	m_lastTape1 = m_sessionPage->m_tape1->GetValue();
	m_lastTape2 = m_sessionPage->m_tape2->GetValue();
	m_applyButton->Enable(false);
}

wxString wxCatapultFrame::ConvertPath(wxString path, bool ConvertSlash)
{
	path.Prepend(_("\""));
	path.Append(_("\""));
	if (ConvertSlash)
		path.Replace(_("\\"),_("/"),true);
	return path;
}

void wxCatapultFrame::OnControllerEvent(wxCommandEvent &event)
{
	m_controller->HandleMessage(event);	
}

void wxCatapultFrame::StartTimer()
{
	m_timer.Start(1000);
}

void wxCatapultFrame::StopTimer()
{
	m_timer.Stop();
	SetStatusText(_(""),1);
}

void wxCatapultFrame::SetFPSdisplay(wxString val)
{
	double valfl = strtod(val.c_str(),NULL);
	val.sprintf("%2.1f",valfl);	
	SetStatusText(_(val + " fps"),1);
}

void wxCatapultFrame::OnTimer(wxTimerEvent& event)
{
	m_controller->WriteCommand(m_controller->GetInfoCommand(_("fps")));
}
