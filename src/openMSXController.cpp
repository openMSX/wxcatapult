// $Id: openMSXController.cpp,v 1.20 2004/03/27 20:37:15 h_oudejans Exp $
// openMSXController.cpp: implementation of the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "CatapultXMLParser.h"
#include "openMSXController.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "MiscControlPage.h"
#include "AudioControlPage.h"
#include "wxCatapultApp.h"
#include <cassert>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

openMSXController::openMSXController(wxWindow * target)
{
	m_openMSXID = 0;
	m_appWindow = (wxCatapultFrame *)target;
	m_openMsxRunning = false;
	m_launchMode = LAUNCH_NONE;
	m_pluggables.Clear();
	m_connectors.Clear();
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
	switch (m_launchMode){
		case LAUNCH_HIDDEN:
			WriteCommand (_("openmsx_info"));
			m_appWindow->SetStatusText("Initializing...");
			break;
		case LAUNCH_NORMAL:
			WriteCommand (GetInfoCommand(_("renderer")));
			m_appWindow->SetStatusText("Running emulator");
			break;
		default:
			assert (false);
			break;
	}

	m_appWindow->StartTimers();
	return true;
}

void openMSXController::HandleEndProcess(wxCommandEvent &event)
{
	if (!m_openMsxRunning) 
		return;

	m_appWindow->StopTimers();
	m_appWindow->SetStatusText("Ready");
	delete m_parser;
	m_commands.clear();
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_openMsxRunning = false;
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_appWindow->SetControlsOnEnd();
	m_appWindow->m_launch_AbortButton->SetLabel(_("Launch"));
}

void openMSXController::HandleStdOut(wxCommandEvent &event)
{
	wxString * data = (wxString *)event.GetClientData();
	m_parser->ParseXmlInput(*data,m_openMSXID);
	delete data;
}

void openMSXController::HandleStdErr(wxCommandEvent &event)
{
	wxString * data = (wxString *)event.GetClientData();
	if (*data == "\n")
		return;
	int i;
	for (i=0;i<m_appWindow->m_tabControl->GetPageCount();i++){
		if (m_appWindow->m_tabControl->GetPageText(i) == _("Status Info")){
			m_appWindow->m_tabControl->SetSelection(i);	
		}
	}
	m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle (wxTextAttr(wxColour(255,23,23),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxNORMAL)));

	m_appWindow->m_statusPage->m_outputtext->AppendText(*data);
#ifdef __UNIX__
	m_appWindow->m_statusPage->m_outputtext->AppendText(_("\n"));
#endif
}

void openMSXController::HandleParsedOutput(wxCommandEvent &event)
{
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	if (data->openMSXID != m_openMSXID)
		return; // another instance is allready started, ignore this message
	switch (data->parseState)
	{
		case CatapultXMLParser::TAG_UPDATE:
			if (m_launchMode != LAUNCH_HIDDEN){
				if (data->updateType == CatapultXMLParser::UPDATE_LED){
					m_appWindow->UpdateLed (data->name, data->contents);
				}
				if (data->updateType == CatapultXMLParser::UPDATE_SETTING){
					wxString lastcmd = PeekPendingCommand();
					if ((lastcmd.Mid(0,4) != "set ") || (lastcmd.Find(' ',true) ==3) || 
						(lastcmd.Mid(4,lastcmd.Find(' ',true)-4)!= data->name)){
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
					}
				}
			}
			break;
		case CatapultXMLParser::TAG_REPLY:
			switch (m_launchMode){
				case LAUNCH_HIDDEN:
					HandleHiddenLaunchReply(event);
					break;
				default:
					switch (data->replyState) {			
						case CatapultXMLParser::REPLY_OK:
							if (m_launchMode == LAUNCH_NORMAL){
								HandleNormalLaunchReply(event);
							}
							else{
								wxString command = GetPendingCommand();
								if (command == GetInfoCommand("fps")){
									m_appWindow->SetFPSdisplay(data->contents);
								}
							}
							break;
						case CatapultXMLParser::REPLY_NOK:
							{
								wxString command = GetPendingCommand();
								if (command == GetInfoCommand("fps")){
									// just ignore (old version)
								}
								else if (command == _("set master_volume")) {
									m_appWindow->m_audioControlPage->DisableMasterVolume();
									WriteCommand (wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(1)+("_volume")));
								}
								else if (command == _("plug msx-midi-in midi-in-reader")){
									m_appWindow->m_audioControlPage->InvalidMidiInReader();
								}
								else if (command == _("plug msx-midi-out midi-out-logger")){
									m_appWindow->m_audioControlPage->InvalidMidiOutLogger();
								}
								else if (command == _("plug pcminput wavinput")){
									m_appWindow->m_audioControlPage->InvalidSampleFilename();
								}
								else {
									m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxBOLD)));
									m_appWindow->m_statusPage->m_outputtext->AppendText(_("Warning: NOK received on command: "));
									m_appWindow->m_statusPage->m_outputtext->AppendText(command);
									m_appWindow->m_statusPage->m_outputtext->AppendText(_("\n"));
									if (!data->contents.IsEmpty()){
										m_appWindow->m_statusPage->m_outputtext->AppendText(_("contents = "));
										m_appWindow->m_statusPage->m_outputtext->AppendText(data->contents);
										m_appWindow->m_statusPage->m_outputtext->AppendText(_("\n"));
									}
								}
								break;
							}
						case CatapultXMLParser::REPLY_UNKNOWN:
							m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxBOLD)));
							m_appWindow->m_statusPage->m_outputtext->AppendText(_("Warning: Unknown reply received!\n"));
							break;
					}
					break;				
			}

			break; 
		case CatapultXMLParser::TAG_LOG:
			if (m_launchMode != LAUNCH_HIDDEN){
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
			}
			break;
		default:
			break;
	}
	delete data;
}

bool openMSXController::WriteCommand(wxString msg)
{
	if (!m_openMsxRunning) 
		return false;
	m_commands.push_back(msg);
	if (!WriteMessage (wxString(_("<command>") + msg + _("</command>\n"))))
		return false;
	return true;
}

wxString openMSXController::GetPendingCommand()
{
	assert (!m_commands.empty());
	wxString pending = m_commands.front();
	m_commands.pop_front();
	return wxString(pending);
}

wxString openMSXController::PeekPendingCommand()
{
	wxString pending;
	if (m_commands.empty()){
		pending = "";
	}
	else {
		pending = m_commands.front();
	}
	return wxString(pending);
}

void openMSXController::StartOpenMSX(wxString cmd, bool hidden)
{
	m_openMSXID++;
	if (hidden){
		m_launchMode = LAUNCH_HIDDEN;
	}
	else {
		m_launchMode = LAUNCH_NORMAL;
	}
	Launch(cmd);			

}

void openMSXController::HandleHiddenLaunchReply(wxCommandEvent &event)
{
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	if (data->replyState == CatapultXMLParser::REPLY_UNKNOWN){
		wxMessageBox (_("Unable to determen openMSX capacities"));
		m_launchMode = LAUNCH_NONE;
		return;
	}
	wxString command = GetPendingCommand();
	if (command == _("openmsx_info")){
		switch (data->replyState){
			case CatapultXMLParser::REPLY_OK:
				m_infoCommand = _("openmsx_info");
				if (data->contents.Find("\n") == -1){
					m_infoCommand = _("openmsx_info_tcl");	
				}
				WriteCommand(_("set master_volume"));
				break;
			case CatapultXMLParser::REPLY_NOK:
				WriteCommand("info");
				return;
			default:
				break;
		}
	} 
	else if (command == _("info")) {
		switch (data->replyState) {
			case CatapultXMLParser::REPLY_OK:
				m_infoCommand = _("info");
				WriteCommand(_("set master_volume"));
				break;
			case CatapultXMLParser::REPLY_NOK:
				wxMessageBox (_("Please update openMSX to 0.3.3 or higher"));
				m_launchMode = LAUNCH_NONE;
				return;
			default:
				break;
		}
	}
	else if (command == _("set master_volume")){
		if (data->replyState == CatapultXMLParser::REPLY_NOK){
			m_appWindow->m_audioControlPage->DisableMasterVolume();
		}
		WriteCommand(_("unset"));
	}
	else if (command == _("unset")){
		switch (data->replyState) {
			case CatapultXMLParser::REPLY_OK:
				m_unsetCommand = _("unset");
				break;
			case CatapultXMLParser::REPLY_NOK:
				m_unsetCommand = _("restoredefault");
				break;
			default:
				break;
		}
		WriteCommand (_("quit"));
	}
	else if (command == _("quit")){
		m_launchMode = LAUNCH_NONE;
		m_appWindow->m_launch_AbortButton->Enable(true);
	}

}

void openMSXController::HandleNormalLaunchReply(wxCommandEvent &event)
{
	int i;
	unsigned int ii;
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	wxString command = GetPendingCommand();
	if (command == GetInfoCommand(_("renderer"))){
		WriteCommand ("update enable setting");
		WriteCommand ("update enable led");
		WriteCommand ("update enable plug");
		WriteCommand ("update enable unplug");
		WriteCommand (GetInfoCommand(_("scaler")));
		TrackAsserts ("Fill rendererers",data->contents); 
		m_appWindow->m_videoControlPage->FillRenderers(data->contents);		
	}
	else if (command == GetInfoCommand(_("scaler"))){
		TrackAsserts ("Fill scalers",data->contents);
		WriteCommand (_("set power on"));
		m_appWindow->m_videoControlPage->FillScalers(data->contents);		
	}
	else if (command == _("set power on")){
		WriteCommand (GetExistCommand("frontswitch"));
	}
	else if (command == GetExistCommand("frontswitch")){
		if ((command.Mid(0,3) == "set") || 
			((command.Mid(0,10) == "info exist") && (data->contents == "1"))){
				m_appWindow->m_miscControlPage->EnableFirmware();
		}
		WriteCommand (wxString(m_unsetCommand + " renderer"));
	}
	else if (command == wxString(m_unsetCommand + " renderer")){
		WriteCommand (_("set renderer"));
	}
	else if (command == _("set renderer")){
		TrackAsserts ("Set renderer",data->contents);
		m_appWindow->m_videoControlPage->SetRenderer(FilterCurrentValue(data->contents));
		WriteCommand (_("set scaler"));
	}
	else if (command == _("set scaler")){
		TrackAsserts ("Set scaler",data->contents);
		m_appWindow->m_videoControlPage->SetScaler(FilterCurrentValue(data->contents));
		WriteCommand (_("set accuracy"));
	}
	else if (command == _("set accuracy")){
		TrackAsserts ("Set Accuracy",data->contents);
		m_appWindow->m_videoControlPage->SetAccuracy(FilterCurrentValue(data->contents));
		WriteCommand (_("set deinterlace"));
	}
	else if (command == _("set deinterlace")){
		TrackAsserts ("Set deinterlace",data->contents);
		m_appWindow->m_videoControlPage->SetDeinterlace(FilterCurrentValue(data->contents));
		WriteCommand (_("set limitsprites"));
	}
	else if (command == _("set limitsprites")){
		TrackAsserts ("Set limitsprites",data->contents);
		m_appWindow->m_videoControlPage->SetLimitSprites(FilterCurrentValue(data->contents));
		WriteCommand (_("set blur"));
	}
	else if (command == _("set blur")){
		TrackAsserts ("Set blur",data->contents);
		m_appWindow->m_videoControlPage->SetBlur(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set glow"));
	}
	else if (command == _("set glow")){
		TrackAsserts ("Set glow",data->contents);
		m_appWindow->m_videoControlPage->SetGlow(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set gamma"));
	}
	else if (command == _("set gamma")){
		TrackAsserts ("Set gamma",data->contents);
		m_appWindow->m_videoControlPage->SetGamma(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set scanline"));
	}
	else if (command == _("set scanline")){
		TrackAsserts ("Set scanline",data->contents);
		m_appWindow->m_videoControlPage->SetScanline(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set speed"));
	}
	else if (command == _("set speed")){
		TrackAsserts ("Set speed",data->contents);
		m_appWindow->m_miscControlPage->SetSpeed(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set frameskip"));
	}
	else if (command == _("set frameskip")){
		TrackAsserts ("set frameskip",data->contents);
		m_appWindow->m_miscControlPage->SetFrameskip(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set throttle"));
	}
	else if (command == _("set throttle")){
		TrackAsserts ("set throttle",data->contents);
		m_appWindow->m_miscControlPage->SetThrottle(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (_("set cmdtiming"));
	}
	else if (command == _("set cmdtiming")){
		TrackAsserts ("set cmdtiming",data->contents);
		m_appWindow->m_miscControlPage->SetCmdTiming(FilterCurrentValue(FilterCurrentValue(data->contents)));
		WriteCommand (GetInfoCommand(_("connector")));
	}
	else if (command == GetInfoCommand(_("connector"))){
		InitConnectors (data->contents);		
		WriteCommand (GetInfoCommand(_("pluggable")));
	}
	else if (command == GetInfoCommand(_("pluggable"))){
		InitPluggables (data->contents);
		if (data->contents.Find(_("msx-midi-in"))){
			WriteCommand (_("set midi-in-readfilename"));
		}
		if (data->contents.Find(_("msx-midi-out"))){
			WriteCommand (_("set midi-out-logfilename"));
		}
		if (data->contents.Find(_("pcminput"))){
			WriteCommand (_("set audio-inputfilename"));
		}
		WriteCommand (GetInfoCommand(wxString (_("pluggable ") + m_pluggables[0])));
	}
	else if (command == _("set midi-in-readfilename")){
		m_appWindow->m_audioControlPage->SetMidiFilename(FilterCurrentValue(data->contents),true);
	}
	else if (command == _("set midi-out-logfilename")){
		m_appWindow->m_audioControlPage->SetMidiFilename(FilterCurrentValue(data->contents),false);
	}
	else if (command == _("set audio-inputfilename")){
		m_appWindow->m_audioControlPage->SetSampleFilename(FilterCurrentValue(data->contents));
	}
	else if (command == GetInfoCommand(_("sounddevice"))){
		TrackAsserts ("info sounddevice",data->contents);
		m_appWindow->m_audioControlPage->InitAudioChannels(data->contents);
		WriteCommand (wxString(GetInfoCommand(_("sounddevice ")+m_appWindow->m_audioControlPage->GetAudioChannelName(1))));
	}
	if (m_pluggables.GetCount() > 0){
		for (ii=0;ii<m_pluggables.GetCount()-1;ii++){	
			if (command == GetInfoCommand(wxString (_("pluggable ") + m_pluggables[ii]))){
				m_pluggabledescriptions.Add(data->contents);
				WriteCommand (GetInfoCommand(wxString (_("pluggable ") + m_pluggables[ii+1])));
			}
		}
		if (command == GetInfoCommand(wxString (_("pluggable ") + m_pluggables[m_pluggables.GetCount()-1]))){
			m_pluggabledescriptions.Add(data->contents);
			WriteCommand (GetInfoCommand(_("sounddevice")));
		}
	}
	
	if (m_appWindow->m_audioControlPage->GetNumberOfAudioChannels() >0){
		for (i=0;i<m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1;i++){
			if (command == wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(i)+_("_volume"))){
				TrackAsserts ("set volume",data->contents);
				m_appWindow->m_audioControlPage->SetChannelVolume(i,FilterCurrentValue(data->contents));
				WriteCommand (wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(i+1)+("_volume")));
			}
			if (command == wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(i)+_("_mode"))){
				TrackAsserts ("set soundmode",data->contents);
				m_appWindow->m_audioControlPage->SetChannelMode(i,FilterCurrentValue(data->contents));
				WriteCommand (wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(i+1)+("_mode")));
			}
			if (command == wxString(GetInfoCommand(_("sounddevice ")+m_appWindow->m_audioControlPage->GetAudioChannelName(i)))){
				TrackAsserts ("info channel",data->contents);
				m_appWindow->m_audioControlPage->AddChannelType(i,FilterCurrentValue(data->contents));
				WriteCommand (wxString(GetInfoCommand(_("sounddevice ")+m_appWindow->m_audioControlPage->GetAudioChannelName(i+1))));
			}
		}
		if (command == wxString(GetInfoCommand(_("sounddevice ")+m_appWindow->m_audioControlPage->GetAudioChannelName(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1)))){
			TrackAsserts ("info channel",data->contents);
			m_appWindow->m_audioControlPage->AddChannelType(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1,FilterCurrentValue(data->contents));
			m_appWindow->m_audioControlPage->SetupAudioMixer();		
			WriteCommand (wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(0)+_("_volume")));
		}
		if (command == wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1)+_("_volume"))){
			TrackAsserts ("set volume",data->contents);
			m_appWindow->m_audioControlPage->SetChannelVolume(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1,FilterCurrentValue(data->contents));
			WriteCommand (wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(1)+_("_mode")));
		}		
		if (command == wxString(_("set ") + m_appWindow->m_audioControlPage->GetAudioChannelName(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1)+_("_mode"))){
			TrackAsserts ("set soundmode",data->contents);
			m_appWindow->m_audioControlPage->SetChannelMode(m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1,FilterCurrentValue(data->contents));
			wxSize tempsize = m_appWindow->GetSize();
			tempsize.SetHeight(tempsize.GetHeight()+1);
			tempsize.SetWidth(tempsize.GetWidth()+1);
			m_appWindow->SetSize(tempsize);
			m_appWindow->SetControlsOnLaunch();
			m_launchMode = LAUNCH_NONE; // interactive mode
		}
	}
}

// if openmsx version < 0.3.5 filter the current value from the explanation

wxString openMSXController::FilterCurrentValue(wxString value)
{
	wxString temp = value;
	int pos = temp.Find(_("current value   : "));
	if (pos != -1){
		wxString temp2;
		temp = temp.Mid(pos+18);
		pos = temp.Find(0x0A);
		if (pos != -1){
			return (wxString(temp.Left(pos)));
		}
	}
	return value; // can't find it, just return the input
}

wxString openMSXController::GetInfoCommand (wxString parameter)
{
	wxString infoCommand;
	wxString tempCmd = m_infoCommand;
	wxString endCmd = "";
	if (m_infoCommand == _("openmsx_info_tcl")){
		tempCmd = _("join [openmsx_info");
		endCmd = _("] \\n");
	}
	infoCommand = tempCmd + _(" ") + parameter + endCmd;
	return wxString (infoCommand);
}

wxString openMSXController::GetExistCommand (wxString parameter)
{
	if (m_infoCommand.Find(_("openmsx")) != -1){
		return wxString (_("info exist ") + parameter);
	}
	else{
		return wxString (_("set ") + parameter);
	}
}	

void openMSXController::TrackAsserts (wxString command, wxString result)
{
#ifdef _DEBUG
	m_appWindow->m_statusPage->m_outputtext->AppendText("tracking assert:");
	m_appWindow->m_statusPage->m_outputtext->AppendText(command);
	m_appWindow->m_statusPage->m_outputtext->AppendText("==>");
	wxString res = result;
	res.Replace("\n"," ",true);
	m_appWindow->m_statusPage->m_outputtext->AppendText(res);
	m_appWindow->m_statusPage->m_outputtext->AppendText("\n");
#endif
}

void openMSXController::InitConnectors(wxString connectors)
{
	if (connectors.IsEmpty())
		return;
	m_connectors.Clear();
	int pos;
	wxString temp = connectors;
	do
	{
		pos = temp.Find(_("\n"));
		if (pos != -1)
		{
			m_connectors.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_connectors.Add(temp);	
}


void openMSXController::GetConnectors (wxArrayString & connectors)
{
	connectors.Clear();
	if (m_connectors.IsEmpty()) return;
	for (unsigned int i=0;i<m_connectors.GetCount();i++){
		connectors.Add(m_connectors[i]);				
	}
}

void openMSXController::InitPluggables(wxString pluggables)
{
	if (pluggables.IsEmpty())
		return;
	m_pluggables.Clear();
	int pos;
	wxString temp = pluggables;
	do
	{
		pos = temp.Find(_("\n"));
		if (pos != -1)
		{
			m_pluggables.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_pluggables.Add(temp);	
}



void openMSXController::GetPluggables (wxArrayString & pluggables)
{
	pluggables.Clear();
	if (m_pluggables.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggables.GetCount();i++){
		pluggables.Add(m_pluggables[i]);				
	}
}

void openMSXController::GetPluggableDescriptions (wxArrayString & descriptions)
{
	descriptions.Clear();
	if (m_pluggabledescriptions.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggabledescriptions.GetCount();i++){
		descriptions.Add(m_pluggabledescriptions[i]);				
	}
}



