// $Id: openMSXController.cpp,v 1.3 2004/02/05 20:02:35 h_oudejans Exp $
// openMSXController.cpp: implementation of the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxCatapultFrm.h"
#include "CatapultXMLParser.h"
#include "openMSXController.h"
#include "StatusPage.h"
#include "wxCatapultApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

openMSXController::openMSXController(wxWindow * target)
{
	m_appWindow = (wxCatapultFrame *)target;
	m_openMsxRunning = false;
}

openMSXController::~openMSXController()
{
}

bool openMSXController::HandleMessage(wxCommandEvent &event)
{
	int id = event.GetId();
	switch (id)
	{
		case MSGID_STDOUT:
			HandleStdOut(event);
			break;
		case MSGID_STDERR:
			HandleStdErr(event);
			break;
		case MSGID_PARSED:
			HandleParsedOutput(event);
			break;
		case MSGID_ENDPROCESS:
			HandleEndProcess(event);
			break;
		default:
			return false; // invalid ID
	}
	return true;
}

bool openMSXController::PreLaunch()
{
	m_parser = new CatapultXMLParser(m_appWindow);
	return true;
}

bool openMSXController::PostLaunch()
{
	WriteMessage (_("<openmsx-control>\n"));
	WriteCommand (_("set power on"));
	WriteCommand (_("restoredefault renderer"));
	return true;
}

void openMSXController::HandleEndProcess(wxCommandEvent &event)
{
	if (!m_openMsxRunning) 
		return;
	delete m_parser;
	m_openMsxRunning = false;
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_appWindow->DisableControls();
	m_appWindow->m_launch_AbortButton->SetLabel(_("Launch"));

}

void openMSXController::HandleStdOut(wxCommandEvent &event)
{
	wxString * data = (wxString *)event.GetClientData();
	m_parser->ParseXmlInput(*data);
	delete data;
}

void openMSXController::HandleStdErr(wxCommandEvent &event)
{
	wxString * data = (wxString *)event.GetClientData();
	if (*data == "\n")
		return;
	m_appWindow->m_tabControl->SetSelection(1);	
	m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle (wxTextAttr(wxColour(255,23,23),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxNORMAL)));

	m_appWindow->m_statusPage->m_outputtext->AppendText(*data);
#ifdef __UNIX__
	m_appWindow->m_statusPage->m_outputtext->AppendText(_("\n"));
#endif
}

void openMSXController::HandleParsedOutput(wxCommandEvent &event)
{
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	switch (data->parseState)
	{
		case CatapultXMLParser::TAG_UPDATE:
			if (data->updateType == CatapultXMLParser::UPDATE_LED)
				m_appWindow->m_statusPage->UpdateLed (data->name, data->contents);
			break;
		case CatapultXMLParser::TAG_REPLY:
			break; // ignore for now
		case CatapultXMLParser::TAG_LOG:
			switch (data->logLevel) {
			case CatapultXMLParser::LOG_WARNING:
				m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle (wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxNORMAL)));
				break;
			case CatapultXMLParser::LOG_INFO:
			case CatapultXMLParser::LOG_UNKNOWN:
				m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle (wxTextAttr(wxColour(0,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxNORMAL)));
				break;
			}	
			m_appWindow->m_statusPage->m_outputtext->AppendText(data->contents);		
			m_appWindow->m_statusPage->m_outputtext->AppendText(_("\n"));
			break;
		default:
			break;
	}
	delete data;
}

bool openMSXController::WriteCommand(wxString msg)
{
	return WriteMessage (wxString(_("<command>") + msg + _("</command>\n")));
}
