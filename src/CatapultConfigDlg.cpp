// $Id: CatapultConfigDlg.cpp,v 1.4 2004/04/25 15:27:50 h_oudejans Exp $
// CatapultConfigDlg.cpp: implementation of the CatapultConfigDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CatapultConfigDlg.h"
#include "ConfigurationData.h"

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

CatapultConfigDlg::CatapultConfigDlg(wxWindow * parent)
{
	wxString guess = "";
	wxXmlResource::Get()->LoadDialog(this, parent, _("ConfigurationDialog"));
	m_ExecPath = (wxTextCtrl *)FindWindow(_("ConfigExecData"));
	m_SharePath = (wxTextCtrl *)FindWindow(_("ConfigShareData"));
	wxTextCtrl * OkButton = (wxTextCtrl *)FindWindow(_("ConfigOk"));
	wxString temp;
	ConfigurationData * config = ConfigurationData::instance();
	OkButton->SetFocus(); // avoid strange behaviour of the execpath textctrl
	if (config->GetParameter(ConfigurationData::CD_EXECPATH,temp)){
		m_ExecPath->SetValue(temp);
	}
	if (temp == _("")){
#ifdef __WINDOWS__		
		guess = ((wxCatapultApp &)wxGetApp()).GetResourceDir();
		guess.Replace ("/","\\",true);
		for (int i=0;i<2;i++){
			while (guess.Last() == '\\'){
				guess = guess.Left(guess.Len()-1);
			}
			int pos = guess.Find('\\',true);
			if (pos !=-1){
				guess = guess.Left(pos+1);
			}
		}
		guess += "openmsx.exe";
#else
		guess = _("opt/openMSX/bin/openmsx");
#endif
		if (wxFileExists(guess)){
			m_ExecPath->SetValue(guess);
		}
	}
	if (config->GetParameter(ConfigurationData::CD_SHAREPATH,temp))
		m_SharePath->SetValue(temp);
	if (temp==_("") && (m_ExecPath->GetValue() != _(""))){
#ifdef __WINDOWS__
		guess = m_ExecPath->GetValue();
		guess.Replace ("/","\\",true);
		int pos = guess.Find('\\',true);
		if (pos != -1){
			guess = guess.Left(pos+1) + _("share");
		}
		else{
			guess = "";
		}
#else
		guess = _("opt/openMSX/share");
#endif
	}
	if (wxDirExists(guess)){
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
		wxMessageBox ("No valid exec path");
	}
	else
	{
		if (!::wxDirExists(tempShare))
		{
			wxMessageBox("No valid share path");
		}
		else
		{
			ConfigurationData * config = ConfigurationData::instance();
			config->SetParameter(ConfigurationData::CD_EXECPATH,tempExec);
			config->SetParameter(ConfigurationData::CD_SHAREPATH,tempShare);
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
	executable = _("");
#else
	executable = _(".exe");
#endif

	wxFileDialog filedlg(this,_("Select openMSX executable"),
			defaultpath, _("openmsx") + executable, 
			_("*") + executable,wxOPEN);
	if (filedlg.ShowModal() == wxID_OK)
	{
		m_ExecPath->SetValue (filedlg.GetPath());	
	}
}

void CatapultConfigDlg::OnBrowseShare(wxCommandEvent &event)
{
	wxString defaultpath = "";
	if (!m_SharePath->GetValue().IsEmpty()){
		defaultpath = ::wxPathOnly(wxString(m_SharePath->GetValue())+_("/"));
	}
	else if (!m_ExecPath->GetValue().IsEmpty()){
		defaultpath = ::wxPathOnly(wxString(m_ExecPath->GetValue())+_("/"));
	}

	wxDirDialog dirdlg(this, _("Select openMSX share directory"),defaultpath);
	if (dirdlg.ShowModal() == wxID_OK)
	{
		m_SharePath->SetValue (dirdlg.GetPath());
	}
}
