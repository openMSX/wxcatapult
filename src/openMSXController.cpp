// $Id: openMSXController.cpp,v 1.39 2004/04/17 15:49:54 h_oudejans Exp $
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
#include "SessionPage.h"
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
	InitLaunchScript ();
}

openMSXController::~openMSXController()
{
	delete [] m_launchScript;
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
	WriteMessage ("<openmsx-control>\n");
	switch (m_launchMode){
		case LAUNCH_NORMAL:
			executeLaunch();
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
	wxString led[6]={_("power"),_("caps"),_("kana"),_("pause"),_("turbo"),_("fdd")};
	for (int i=0;i<6;i++){	
		m_appWindow->UpdateLed(led[i],"off");
	}
	m_appWindow->StopTimers();
	m_appWindow->SetStatusText("Ready");
	delete m_parser;
	m_commands.clear();
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_appWindow->m_audioControlPage->DisableAudioPanel();
	m_openMsxRunning = false;
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_appWindow->SetControlsOnEnd();
	m_appWindow->m_launch_AbortButton->SetLabel(_("Start"));
	HandleNativeEndProcess ();
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
	if (*data == "\n"){
		delete data;
		return;
	}
	int i;
	for (i=0;i<m_appWindow->m_tabControl->GetPageCount();i++){
		if (m_appWindow->m_tabControl->GetPageText(i) == _("Status Info")){
			m_appWindow->m_tabControl->SetSelection(i);	
		}
	}
	m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle (wxTextAttr(wxColour(255,23,23),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxNORMAL)));

	m_appWindow->m_statusPage->m_outputtext->AppendText(*data);
	delete data;
}

void openMSXController::HandleParsedOutput(wxCommandEvent &event)
{
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	if (data->openMSXID != m_openMSXID){
		delete data;
		return; // another instance is allready started, ignore this message
	}
	switch (data->parseState)
	{
		case CatapultXMLParser::TAG_UPDATE:
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
			else if (data->updateType == CatapultXMLParser::UPDATE_PLUG){
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,5) != "plug ") || (lastcmd.Find(' ',true) == 4) ||
					(lastcmd.Mid(5,lastcmd.Find(' ',true)-5)!= data->name)){
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
						m_launchMode = LAUNCH_NORMAL;
						executeLaunch(NULL,43);
				}
			}
			else if (data->updateType == CatapultXMLParser::UPDATE_UNPLUG){
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,7) != "unplug ") || (lastcmd.Find(' ',true) == 6)){
					m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
					m_launchMode = LAUNCH_NORMAL;
					executeLaunch(NULL,43);
				}
			}
			else if (data->updateType == CatapultXMLParser::UPDATE_MEDIA){
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,data->name.Len()+1) != wxString(data->name + _(" "))) || 
					(lastcmd.Find(' ',true) == (int)data->name.Len())){
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
						m_appWindow->m_sessionPage->UpdateSessionData();
				}
			}

			break;
		case CatapultXMLParser::TAG_REPLY:
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
						if (m_launchMode == LAUNCH_NORMAL){
							HandleNormalLaunchReply(event);
						}
						else{
							wxString command = GetPendingCommand();
							if ((command == GetInfoCommand("fps")) ||
								(command.Mid(0,13) == _("update enable"))){
								// just ignore (old version)
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
						}
					}
					break;
				case CatapultXMLParser::REPLY_UNKNOWN:
					m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxBOLD)));
					m_appWindow->m_statusPage->m_outputtext->AppendText(_("Warning: Unknown reply received!\n"));
					break;
			}
			break; 
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
	if (!m_openMsxRunning) 
		return false;
	m_commands.push_back(msg);
	if (!WriteMessage (wxString("<command>" + msg + "</command>\n")))
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

void openMSXController::StartOpenMSX(wxString cmd, bool getversion)
{
	m_openMSXID++;
	if (getversion){
		m_appWindow->SetStatusText("Initializing...");
		wxString versioninfo = GetOpenMSXVersionInfo(cmd);
		SetupOpenMSXParameters(versioninfo);
		m_appWindow->SetStatusText("Ready");
	}
	else {
		m_launchMode = LAUNCH_NORMAL;
		m_appWindow->SetStatusText("Running emulator");
		Launch(cmd);
	}
}

void openMSXController::HandleNormalLaunchReply(wxCommandEvent &event)
{
	executeLaunch(&event);
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

int openMSXController::InitConnectors(wxString connectors, wxString dummy)
{
	if (connectors.IsEmpty())
		return -1;
	m_connectors.Clear();
	m_connectorclasses.Clear();
	m_connectorcontents.Clear();
	int pos;
	wxString temp = connectors;
	do
	{
		pos = temp.Find("\n");
		if (pos != -1)
		{
			m_connectors.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_connectors.Add(temp);
	return 0;
}


void openMSXController::GetConnectors (wxArrayString & connectors)
{
	connectors.Clear();
	if (m_connectors.IsEmpty()) return;
	for (unsigned int i=0;i<m_connectors.GetCount();i++){
		connectors.Add(m_connectors[i]);				
	}
}

wxString openMSXController::GetConnectorClass (wxString name)
{
	if (m_connectorclasses.IsEmpty()) return wxString (_(""));
	unsigned index = 0;
	bool found = false;
	while ((!found) && (index < m_connectors.GetCount())){
		if (m_connectors[index] == name){
			found = true;
		}
		else{
			index++;
		}
	}
	if (!found){
		wxMessageBox ("Get Connector class, connector: " + name + " not found");
		return wxString(_(""));
	}
	return wxString (m_connectorclasses[index]);
}

wxString openMSXController::GetConnectorContents (wxString name)
{
	if (m_connectorcontents.IsEmpty()) return wxString (_(""));
	unsigned index = 0;
	bool found = false;
	while ((!found) && (index < m_connectors.GetCount())){
		if (m_connectors[index] == name){
			found = true;
		}
		else{
			index++;
		}
	}
	if (!found){
		wxMessageBox ("Get Connector contents, connector: " + name + " not found");
		return wxString(_(""));
	}
	return wxString (m_connectorcontents[index]);
}



int openMSXController::InitPluggables(wxString pluggables, wxString dummy)
{
	if (pluggables.IsEmpty())
		return -1;
	m_pluggables.Clear();
	m_pluggabledescriptions.Clear();
	m_pluggableclasses.Clear();
	int pos;
	wxString temp = pluggables;
	do
	{
		pos = temp.Find("\n");
		if (pos != -1)
		{
			m_pluggables.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);					
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_pluggables.Add(temp);	
	return 0;
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

void openMSXController::GetPluggableClasses (wxArrayString & classes)
{
	classes.Clear();
	if (m_pluggableclasses.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggableclasses.GetCount();i++){
		classes.Add(m_pluggableclasses[i]);				
	}
}


bool openMSXController::SetupOpenMSXParameters(wxString version)
{
	long ver=-1;
	long majver;
	long minver;
	long subver;
	wxString temp = version;
	if (version.Mid(0,8) == _("openMSX ")){
		int pos = version.Find('.');
		if (pos != -1){
			temp.Mid(8,pos-8).ToLong(&majver);
			temp = temp.Mid(pos+1);
			pos = temp.Find('.');
			if (pos != -1){
				temp.Mid(0,pos).ToLong(&minver);
				temp.Mid(pos+1).ToLong(&subver);
				ver = (((majver * 100) + minver)*100)+subver;
			}
		}
	}
	if (ver == -1){
		wxMessageBox ("Unable to determine openMSX version!\nPlease upgrade to 0.3.4 or higher.","Error");
		return false;	
	}
	if (ver < 304){
		wxMessageBox ("The openMSX version you are using is too old!\nPlease upgrade to 0.3.4 or higher.","Error");
		return false;
	}
	if (version.Find(_("-dev")) != -1){
		m_infoCommand = _("openmsx_info_tcl");
		m_appWindow->m_audioControlPage->EnableMasterVolume();
		m_unsetCommand = _("unset");
		m_appWindow->m_miscControlPage->DisableAutoFrameSkip();
	}
	else{   // release (less possibilities)
		m_infoCommand = _("info");
		m_appWindow->m_audioControlPage->DisableMasterVolume();
		m_unsetCommand = _("restoredefault");
		m_appWindow->m_miscControlPage->EnableAutoFrameSkip(false);
	}
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
}

void openMSXController::InitLaunchScript ()
{
	m_launchScriptSize = 0;
	m_launchScript = new LaunchInstructionType [LAUNCHSCRIPT_MAXSIZE];
	AddLaunchInstruction ("update enable setting","","",NULL,false);
	AddLaunchInstruction ("update enable led","","",NULL,false);
	AddLaunchInstruction ("update enable plug","","",NULL,false);
	AddLaunchInstruction ("update enable unplug","","",NULL,false);
	AddLaunchInstruction ("set power on","e","power",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("#unset renderer","e","",NULL,true);
	AddLaunchInstruction ("#info renderer","","RendererSelector",&openMSXController::FillComboBox,true);
	AddLaunchInstruction ("#info scaler","","ScalerSelector",&openMSXController::FillComboBox,true);
	AddLaunchInstruction ("#info accuracy","","AccuracySelector",&openMSXController::FillComboBox,false);
	AddLaunchInstruction ("update enable media","","",NULL,false);
	AddLaunchInstruction ("#exist frontswitch","","#",&openMSXController::EnableFirmware,false);
	AddLaunchInstruction ("set renderer","","renderer",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set scaler","","scaler",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set accuracy","","accuracy",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set deinterlace","","deinterlace",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set limitsprites","","limitsprites",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set fullscreen","","fullscreen",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set blur","","blur",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set glow","","glow",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set gamma","","gamma",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set scanline","0","scanline",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("@execute","","",&openMSXController::SetSliderDefaults,false);
	AddLaunchInstruction ("set speed","","speed",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("#exist frameskip","1","#",&openMSXController::EnableAutoFrameSkip,false);
	AddLaunchInstruction ("set frameskip","","frameskip",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("#exist maxframeskip","1","#",NULL,false);
	AddLaunchInstruction ("set maxframeskip","","maxframeskip",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set throttle","","throttle",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set cmdtiming","","cmdtiming",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("#info pluggable","14","",&openMSXController::InitPluggables,true);
	AddLaunchInstruction ("#info_nostore pluggable *","","*",&openMSXController::AddPluggableDescription,true);
	AddLaunchInstruction ("#info_nostore connectionclass *","","*",&openMSXController::AddPluggableClass,true);
	AddLaunchInstruction ("#info connector","10","",&openMSXController::InitConnectors,true);
	AddLaunchInstruction ("#info_nostore connectionclass *","","*",&openMSXController::AddConnectorClass,true);
	AddLaunchInstruction ("plug *","","*",&openMSXController::AddConnectorContents,true);
	AddLaunchInstruction ("@checkfor msx-midi-in","1","",NULL,false);
	AddLaunchInstruction ("set midi-in-readfilename","","midi-in-readfilename",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("@checkfor msx-midi-out","1","",NULL,false);
	AddLaunchInstruction ("set midi-out-logfilename","","midi-out-logfilename",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("@checkfor pcminput","1","",NULL,false);
	AddLaunchInstruction ("set audio-inputfilename","","audio-inputfilename",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("@execute","","",&openMSXController::InitConnectorPanel,false);
	AddLaunchInstruction ("@execute","","",&openMSXController::InitAudioConnectorPanel,false);
	AddLaunchInstruction ("#info sounddevice","5","",&openMSXController::InitSoundDevices,true);
	AddLaunchInstruction ("#info_nostore sounddevice *","","*",&openMSXController::SetChannelType,true);
	AddLaunchInstruction ("#exist master_volume","1","",NULL,false);
	AddLaunchInstruction ("set master_volume","","master_volume",&openMSXController::UpdateSetting,false);
	AddLaunchInstruction ("set *_volume","","*_volume",&openMSXController::UpdateSetting,true);
	AddLaunchInstruction ("set *_mode","","*_mode",&openMSXController::UpdateSetting,true);
	
	
}

void openMSXController::AddLaunchInstruction (wxString cmd, wxString action, wxString parameter, 
				int (openMSXController::*pfunction)(wxString,wxString),
				bool showError)
{	
	if (m_launchScriptSize >= LAUNCHSCRIPT_MAXSIZE){
		wxMessageBox ("Not enough space to store the Launchscript","Internal Catapult Error");
		return;
	}
	m_launchScript[m_launchScriptSize].command = cmd;
	m_launchScript[m_launchScriptSize].scriptActions = action;
	m_launchScript[m_launchScriptSize].parameter = parameter;
	m_launchScript[m_launchScriptSize].p_okfunction = pfunction;
	m_launchScript[m_launchScriptSize].showError = showError;
	m_launchScriptSize ++;
}

void openMSXController::executeLaunch (wxCommandEvent * event, int startLine)
{	
	CatapultXMLParser::ParseResult * data = NULL;
	if (event != NULL){
		data = (CatapultXMLParser::ParseResult *)event->GetClientData();
	}
	wxString instruction;
	wxString cmd;
	wxString action;
	wxArrayString tokens;
	static bool wait;
	static int sendStep = 0;
	static int recvStep = 0;
	static int sendLoop = -1;
	static int recvLoop = -1;
	static wxString lastdata = "";
	wxString command;

	if (event != NULL){ // handle received command
		command = GetPendingCommand();
		instruction  = m_launchScript[recvStep].command;
		while (instruction.Mid(0,1) == "@"){
			recvStep ++;
			instruction  = m_launchScript[recvStep].command;
		}
		if ((recvLoop == -1) && (instruction.Find("*") != -1)){
			recvLoop = 0;
		}
		tokenize(instruction," ",tokens);
		cmd = translate(tokens,recvLoop,lastdata);
		if (command == cmd){
			if (tokens[0] == "#info"){
				lastdata = data->contents;
			}
			HandleLaunchReply (cmd,event,m_launchScript[recvStep],&sendStep,recvLoop,lastdata);
			if (data->replyState == CatapultXMLParser::REPLY_NOK){
				long displace;
				m_launchScript[recvStep].scriptActions.ToLong(&displace);
				recvStep += displace;	
			}
			if (cmd == "!done"){
				recvStep = -2; // it is gonna be increased in a moment
				recvLoop = -1;
			}
	
			if (recvLoop != -1){
				wxArrayString lastvalues;
				if (recvLoop < (tokenize(lastdata,"\n",lastvalues)-1)){
					recvLoop++;
				}
				else{
					recvLoop = -1;
					recvStep ++;
				}
			}
			else{
				recvStep++;
			}
		}
	}
	else{ // init chain of events
		sendStep = startLine;
		recvStep = startLine;
		sendLoop = -1;
		recvLoop = -1;
	}
		if (recvStep >= m_launchScriptSize){
			recvStep = 0;
			FinishLaunch();
			return;
		}
		if (wait){
			while (m_launchScript[recvStep].command.Mid(0,1) == "@"){
				recvStep ++;
			}
			if (recvStep >= sendStep){
			wait = false;
			}
		}

		while ((!wait) && (sendStep < m_launchScriptSize)){

				instruction  = m_launchScript[sendStep].command;
				if ((sendLoop == -1) && (instruction.Find("*") != -1)){
					sendLoop = 0;
				}
				tokenize(instruction," ",tokens);
				cmd = translate(tokens,sendLoop,lastdata);
				if (cmd.Mid(0,1) == "@"){
					wxString result = "0";
					if (tokens[0] == "@checkfor"){
						bool contains = lastdata.Contains(tokens[1]);
						if (contains){
							result = "1";
						}
					}
					else { // @execute
						result = "1";
					}
					if (result == "0"){
						while (m_launchScript[recvStep].command.Mid(0,1) == "@"){
							recvStep ++;
						}
						long displace;
						m_launchScript[sendStep].scriptActions.ToLong(&displace);
						recvStep += displace;
					}
					HandleLaunchReply (tokens[0] + result,NULL,m_launchScript[sendStep],&sendStep,-1,"");
					
				}
				else {
					WriteCommand (cmd);
				}
				action = m_launchScript[sendStep].scriptActions;
				
				if (sendLoop != -1){
					wxArrayString lastvalues;
					if (sendLoop < (tokenize(lastdata,"\n",lastvalues)-1)){
						sendLoop++;
					}
					else{
						sendLoop = -1;
						sendStep ++;
						wait = true;
					}
				}
				else {
					sendStep++;
				}

				if ( action != ""){
					if (recvStep < sendStep){
						wait = true;
					}
				}
		}
}

void openMSXController::FinishLaunch()
{
	wxSize tempsize = m_appWindow->GetSize();
	tempsize.SetHeight(tempsize.GetHeight()+1);
	tempsize.SetWidth(tempsize.GetWidth()+1);
	m_appWindow->SetSize(tempsize);
#ifdef __WINDOWS__
	tempsize.SetHeight(tempsize.GetHeight()-1);
	tempsize.SetWidth(tempsize.GetWidth()-1);
	m_appWindow->SetSize(tempsize);
#endif
	m_appWindow->SetControlsOnLaunch();
	m_launchMode = LAUNCH_NONE; // interactive mode
}


int openMSXController::tokenize (wxString text, wxString seperator, wxArrayString & result)
{
	int pos;
	result.Clear();
	wxString temp = text;
	do{
		pos = temp.Find(seperator);
		if (pos != -1){
			if (pos != 0){ // ignore multiple seperators
				result.Add(temp.Mid(0,pos));
				temp = temp.Mid(pos + seperator.Len());
			}
			else{
				temp = temp.Mid(seperator.Len());
			}
		}
	}while (pos != -1);
	if (!temp.IsEmpty()){
		result.Add(temp);
	}
	return result.GetCount();
}

wxString openMSXController::translate(wxArrayString tokens, int loop, wxString lastdata)
{
	unsigned int token;
	for (token=0;token<tokens.GetCount();token++){
		
		if (tokens[token].Find("*") != -1){
			if (loop != -1){
				wxArrayString lastvalues;
				tokenize (lastdata,"\n",lastvalues);
				if (loop < (int)lastvalues.GetCount()){
						tokens[token].Replace("*",lastvalues[loop],true);
						tokens[token].Replace (" ","\\ ",true);
				}			
			}
		}
	}
	token = 0;
	while (token < tokens.GetCount()){
		switch (tokens[token][(size_t)0]){
		case '#':
			if (tokens[token].Mid(0,5)=="#info"){
				wxString parameter = "";
				while (token < (tokens.GetCount()-1)){
					parameter += tokens[token+1];
					parameter += " ";
					tokens.Remove(token+1);
				}
				parameter.Trim(true);	
				tokens[token] = GetInfoCommand(parameter);
			}
			else if (tokens[token].Mid(0,6) == "#exist"){
				tokens[token] = GetExistCommand(tokens[token+1]);
				tokens.Remove(token+1);
			}
			else if (tokens[token].Mid(0,6) == "#unset"){
				tokens[token] = m_unsetCommand;
			}
			else {
				assert(false); // invalid command
			}
			break;
		default:
			break;
		}
		token++;
	}
	wxString result="";
	for (token=0;token<tokens.GetCount();token++){
		result += tokens[token];
		result += " ";
	}
	result.Trim(true);
	return wxString (result);
}

void openMSXController::HandleLaunchReply (wxString cmd,wxCommandEvent * event,
						LaunchInstructionType instruction ,int * sendStep, int loopcount,
						wxString datalist)
{
	CatapultXMLParser::ParseResult * data = NULL;
	if (event != NULL){
		data = (CatapultXMLParser::ParseResult *)event->GetClientData();
	}	
	bool ok = false;
	if (cmd.Mid(0,1)=="@"){
		if (cmd.Mid(cmd.Len()-1) == "1"){
			ok = true;
		}
	}
	else{
		if (data->replyState == CatapultXMLParser::REPLY_OK)
			ok = true;
	}
	wxString actions = instruction.scriptActions;
		
	if (ok){
		if (instruction.p_okfunction != NULL){
			wxString parameter = instruction.parameter;
			wxString contents = "";
			if (loopcount > -1){
				wxArrayString temp;
				tokenize (datalist,"\n",temp);
				parameter.Replace("*",temp[loopcount],true);
				parameter.Replace(" ","\\ ",true);
			}
			if (parameter == "#"){
				parameter = cmd;
			}
			if (event != NULL){
				contents = FilterCurrentValue(data->contents);
			}
			int result = (*this.*(instruction.p_okfunction))(contents,parameter);
			if (result >0){
				*sendStep += result;		
			}
		}
	}
	else {
		if (instruction.showError){
			// show error
		}
		if (actions != ""){
			if (actions == "e"){
				*sendStep = m_launchScriptSize;
			}
			else{
				long displace;
				actions.ToLong(&displace);
				*sendStep += displace;
			}
		}
	}
}

int openMSXController::UpdateSetting (wxString data,wxString setting)
{
	m_appWindow->m_videoControlPage->UpdateSetting(setting,data);
	return 0;
}

int openMSXController::FillComboBox (wxString data,wxString setting)
{
	m_appWindow->m_videoControlPage->FillComboBox(setting,data);
	return 0;
}

int openMSXController::EnableFirmware (wxString data, wxString cmd)
{
	if ((data != "0") || (cmd.Mid(0,4) == _("set "))){
		m_appWindow->m_miscControlPage->EnableFirmware();
	}
	return 0;
}

int openMSXController::InitSoundDevices (wxString data, wxString dummy)
{
	wxArrayString channels;
	tokenize(data,"\n",channels);
	if ((int)channels.GetCount() != (m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1)){
		m_appWindow->m_audioControlPage->DestroyAudioMixer();	
		m_appWindow->m_audioControlPage->InitAudioChannels(data);
		return 0;
	}
	return 5; // skip 5 instructions TODO: improve this
}

int openMSXController::SetChannelType (wxString data,wxString name)
{
	int maxchannels = m_appWindow->m_audioControlPage->GetNumberOfAudioChannels();
	int index = 0;
	bool found = false;
	while ((!found) && (index < maxchannels)){
		if (m_appWindow->m_audioControlPage->GetAudioChannelName(index) == name){
			found = true;
		}
		else{
			index++;
		}
	}
	if (!found){
		wxMessageBox ("Set Channeltype : " + name + " not found");
		return -1;
	}
	m_appWindow->m_audioControlPage->AddChannelType(index,data);
	if (index == (maxchannels-1)){
		m_appWindow->m_audioControlPage->SetupAudioMixer();
	}
	return 0;
}

int openMSXController::AddPluggableDescription(wxString data,wxString name)
{
	m_pluggabledescriptions.Add(data);
	return 0;
}

int openMSXController::AddPluggableClass(wxString data, wxString name)
{
	m_pluggableclasses.Add(data);
	return 0;
}

int openMSXController::AddConnectorClass(wxString data, wxString name)
{
	m_connectorclasses.Add(data);
	return 0;
}

int openMSXController::AddConnectorContents(wxString data, wxString name)
{
	m_connectorcontents.Add(data);
	return 0;
}

int openMSXController::SetSliderDefaults (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_videoControlPage->SetSliderDefaults();
	return 0;
}

int openMSXController::InitAudioConnectorPanel (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_audioControlPage->InitAudioIO();
	return 0;
}

int openMSXController::InitConnectorPanel (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_miscControlPage->InitConnectorPanel();
	return 0;
}

int openMSXController::EnableAutoFrameSkip (wxString data, wxString cmd)
{
	if ((data != "0") || (cmd.Mid(0,4) == _("set "))){
		m_appWindow->m_miscControlPage->EnableAutoFrameSkip();
	}
	return 0;
}
