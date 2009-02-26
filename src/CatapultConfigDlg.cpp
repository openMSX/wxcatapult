// $Id$
// CatapultConfigDlg.cpp: implementation of the CatapultConfigDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "CatapultConfigDlg.h"
#include "ConfigurationData.h"
#include "wxCatapultFrm.h"
#include "openMSXController.h"

#ifndef __WXMSW__
#include "config.h"
#endif

	IMPLEMENT_CLASS(CatapultConfigDlg, wxDialog)
BEGIN_EVENT_TABLE(CatapultConfigDlg, wxDialog)
	EVT_BUTTON(XRCID("ConfigOk"),CatapultConfigDlg::OnOk)
	EVT_BUTTON(XRCID("ConfigCancel"),CatapultConfigDlg::OnCancel)
	EVT_BUTTON(XRCID("ConfigBrowseExec"),CatapultConfigDlg::OnBrowseExec)
	EVT_BUTTON(XRCID("ConfigBrowseShare"),CatapultConfigDlg::OnBrowseShare)
END_EVENT_TABLE()

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

CatapultConfigDlg::CatapultConfigDlg(wxWindow * parent, openMSXController* controller) : m_parent(parent), m_controller(controller)
{
	wxString guess = wxT("");
	wxXmlResource::Get()->LoadDialog(this, m_parent, wxT("ConfigurationDialog"));
	m_ExecPath = (wxTextCtrl *)FindWindowByName(wxT("ConfigExecData"));
	m_SharePath = (wxTextCtrl *)FindWindowByName(wxT("ConfigShareData"));
	m_ConfigCheck = (wxCheckBox *)FindWindowByName(wxT("configCheck"));
	wxTextCtrl * OkButton = (wxTextCtrl *)FindWindowByName(wxT("ConfigOk"));
	wxString temp;
	ConfigurationData * config = ConfigurationData::instance();
	OkButton->SetFocus(); // avoid strange behaviour of the execpath textctrl
	if (config->GetParameter(ConfigurationData::CD_EXECPATH,temp)) {
		m_ExecPath->SetValue(temp);
	}
	if (temp == wxT("")) {
#ifdef __WXMSW__
		guess = ((wxCatapultApp &)wxGetApp()).GetResourceDir();
		guess.Replace (wxT("/"),wxT("\\"),true);
		for (int i=0;i<2;i++) {
			while (guess.Last() == '\\') {
				guess = guess.Left(guess.Len()-1);
			}
			int pos = guess.Find('\\',true);
			if (pos !=-1) {
				guess = guess.Left(pos+1);
			}
		}
		guess += wxT("openmsx.exe");
#else
		guess = CATAPULT_OPENMSX_BINARY;
#endif
		if (wxFileExists(guess)) {
			m_ExecPath->SetValue(guess);
		}
	}
	if (config->GetParameter(ConfigurationData::CD_SHAREPATH,temp))
		m_SharePath->SetValue(temp);
	if (temp==wxT("") && (m_ExecPath->GetValue() != wxT(""))) {
#ifdef __WXMSW__
		guess = m_ExecPath->GetValue();
		guess.Replace (wxT("/"),wxT("\\"),true);
		int pos = guess.Find('\\',true);
		if (pos != -1) {
			guess = guess.Left(pos+1) + wxT("share");
		}
		else{
			guess = wxT("");
		}
#else
		guess = CATAPULT_OPENMSX_SHARE;
#endif
	}
	if (wxDirExists(guess)) {
		m_SharePath->SetValue(guess);
	}
}

CatapultConfigDlg::~CatapultConfigDlg()
{
}

void CatapultConfigDlg::OnOk(wxCommandEvent& event)
{
	wxString tempExec;
	wxString tempShare;

	tempExec = m_ExecPath->GetValue();
	tempShare = m_SharePath->GetValue();
	if (!::wxFileExists(tempExec))
	{
		wxMessageBox (wxT("That's not a valid executable..."));
	}
	else
	{
		if (!::wxDirExists(tempShare))
		{
			wxMessageBox(wxT("That's not a valid share path..."));
		}
		else if (m_controller->StartOpenMSX(tempExec, true))
		{
			ConfigurationData * config = ConfigurationData::instance();
			config->SetParameter(ConfigurationData::CD_EXECPATH,tempExec);
			config->SetParameter(ConfigurationData::CD_SHAREPATH,tempShare);
			config->SaveData();
			if (m_ConfigCheck->IsChecked()) {
				// TODO: make this cleaner... (It works though...)
				((wxCatapultFrame*) m_parent)->CheckConfigs();
        		}
			EndModal (wxID_OK);
		}
	}
}

void CatapultConfigDlg::OnCancel(wxCommandEvent& event)
{
	EndModal(wxID_CANCEL);
}

void CatapultConfigDlg::OnBrowseExec(wxCommandEvent &event)
{
	wxString defaultpath = ::wxPathOnly(m_ExecPath->GetValue());
	wxString executable;

#ifdef __UNIX__
	executable = wxT("");
#else
	executable = wxT(".exe");
#endif

	wxFileDialog filedlg(this,wxT("Select openMSX executable"),
			defaultpath, wxT("openmsx") + executable,
			wxT("*") + executable,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_ExecPath->SetValue (filedlg.GetPath());
	}
}

void CatapultConfigDlg::OnBrowseShare(wxCommandEvent &event)
{
	wxString defaultpath = wxT("");
	if (!m_SharePath->GetValue().IsEmpty()) {
		defaultpath = ::wxPathOnly(wxString(m_SharePath->GetValue())+wxT("/"));
	}
	else if (!m_ExecPath->GetValue().IsEmpty()) {
		defaultpath = ::wxPathOnly(wxString(m_ExecPath->GetValue())+wxT("/"));
	}

	wxDirDialog dirdlg(this, wxT("Select openMSX share directory"),defaultpath);
	if (dirdlg.ShowModal() == wxID_OK)
	{
		m_SharePath->SetValue (dirdlg.GetPath());
	}
}
