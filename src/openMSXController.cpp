// $Id$
// openMSXController.cpp: implementation of the openMSXController class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/version.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wxCatapultFrm.h"
#include "ConfigurationData.h"
#include "openMSXController.h"
#include "StatusPage.h"
#include "VideoControlPage.h"
#include "MiscControlPage.h"
#include "AudioControlPage.h"
#include "SessionPage.h"
#include "wxCatapultApp.h"
#include "ScreenShotDlg.h"
#include <cassert>


#if wxCHECK_VERSION(2,5,0)
#define WX24SIGNCAST
#else
#define WX24SIGNCAST (unsigned int)
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

openMSXController::openMSXController(wxWindow * target)
{
	m_openMSXID = 0;
	m_appWindow = (wxCatapultFrame *)target;
	m_openMsxRunning = false;
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
//	connectSocket (); // disabled since openMSX has also diabled sockets for security reasons
	char initial[] = "<openmsx-control>\n";
	WriteMessage ((unsigned char *)initial,strlen(initial));
	executeLaunch();
	m_appWindow->StartTimers();
	return true;
}

bool openMSXController::connectSocket()
{
	bool bRetval = false;
	if (m_socket == NULL){ // only if we don't have a socket connection
		m_socket = new wxSocketClient;
		m_socket->SetEventHandler(*m_appWindow,OPENMSX_SOCKET);
		m_socket->SetNotify (wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
		m_socket->Notify(true);

		wxIPV4address addr; // wx implemented only ipv4 so far
		addr.Hostname (wxT("localhost")); // only localhost for now
		addr.Service (9938); // openMSX port
        if (!m_socket->Connect(addr,true)){ // don't wait, openMSX should be available allready
			wxMessageBox (wxT("Error: openMSX not available for socket connection !"));
		}
		else{
			bRetval = true; // succes
		}
	}
	return bRetval;
}

void openMSXController::HandleSocketEvent(wxSocketEvent & event)
{
	switch (event.GetSocketEvent()){
	case wxSOCKET_INPUT:
		{
			char * buffer = new char[1024];
        	event.GetSocket()->Read(buffer,1024);
			buffer[event.GetSocket()->LastCount()-1]=0;
			wxString data = wxCSConv(wxT("ISO8859-1")).cMB2WX(buffer);
			m_parser->ParseXmlInput(data,m_openMSXID);
			delete buffer;
		}
	case wxSOCKET_LOST:
		break; // not sure what to do yet
	case wxSOCKET_CONNECTION:
		break;
	default:
		break;
	}
}

void openMSXController::HandleEndProcess(wxCommandEvent &event)
{
	if (!m_openMsxRunning)
		return;
	wxString led[6]={wxT("power"), wxT("caps"), wxT("kana"), wxT("pause"),
		wxT("turbo"), wxT("FDD")};
	unsigned int i=0;
	FOREACH(i,led){
		m_appWindow->UpdateLed(led[i],wxT("off"));
	}
	m_appWindow->StopTimers();
	m_appWindow->SetStatusText(wxT("Ready"));
	m_appWindow->EnableSaveSettings(false);
	delete m_parser;
	m_appWindow->m_audioControlPage->DestroyAudioMixer();
	m_appWindow->m_audioControlPage->DisableAudioPanel();
	m_openMsxRunning = false;
	m_appWindow->m_launch_AbortButton->Enable(true);
	m_appWindow->SetControlsOnEnd();
	m_appWindow->m_launch_AbortButton->SetLabel(wxT("Start"));
	HandleNativeEndProcess ();
	m_commands.clear();
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
	if (*data == wxT("\n")) {
		delete data;
		return;
	}
	unsigned int i;
	for (i=0;i< WX24SIGNCAST m_appWindow->m_tabControl->GetPageCount();i++) {
		if (m_appWindow->m_tabControl->GetPageText(i) == wxT("Status Info")) {
			m_appWindow->m_tabControl->SetSelection(i);
		}
	}
	m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(255,23,23), wxNullColour, wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)));

	m_appWindow->m_statusPage->m_outputtext->AppendText(*data);
	delete data;
}

void openMSXController::HandleParsedOutput(wxCommandEvent &event)
{
	CatapultXMLParser::ParseResult * data = (CatapultXMLParser::ParseResult *)event.GetClientData();
	if (data->openMSXID != m_openMSXID) {
		delete data;
		return; // another instance is allready started, ignore this message
	}
	switch (data->parseState)
	{
		case CatapultXMLParser::TAG_UPDATE:
			if (data->updateType == CatapultXMLParser::UPDATE_LED) {
				m_appWindow->UpdateLed (data->name, data->contents);
			}
			if (data->updateType == CatapultXMLParser::UPDATE_STATE) {
				if (data->name == wxT("cassetteplayer")){
					m_appWindow->m_sessionPage->SetCassetteMode(data->contents);
				}
				else{
					m_appWindow->UpdateState (data->name, data->contents);
				}
			}
			if (data->updateType == CatapultXMLParser::UPDATE_SETTING) {
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,4) != wxT("set ")) || (lastcmd.Find(' ',true) ==3) ||
					(lastcmd.Mid(4,lastcmd.Find(' ',true)-4)!= data->name)) {
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
				}
			}
			else if (data->updateType == CatapultXMLParser::UPDATE_PLUG) {
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,5) != wxT("plug ")) || (lastcmd.Find(' ',true) == 4) ||
					(lastcmd.Mid(5,lastcmd.Find(' ',true)-5)!= data->name)) {
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
						executeLaunch(NULL,43);
				}
			}
			else if (data->updateType == CatapultXMLParser::UPDATE_UNPLUG) {
				wxString lastcmd = PeekPendingCommand();
				if ((lastcmd.Mid(0,7) != wxT("unplug ")) || /*(lastcmd.Find(' ',true) == 6)) {*/
					(lastcmd.Mid(7)!= data->name)) {
					m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
					executeLaunch(NULL,43);
				}
			}
			else if (data->updateType == CatapultXMLParser::UPDATE_MEDIA) {
				wxString lastcmd = PeekPendingCommand();
				bool eject = false;
				int space = lastcmd.Find(' ',false);
				if ((space != -1) && (space != (int)lastcmd.Len()-1)){
					eject = true;
				}
				if ((lastcmd.Mid(0,data->name.Len()+1) != wxString(data->name + wxT(" "))) ||
					(!eject && (lastcmd.Mid(space+1) != (wxString (wxT("\"")) + data->contents + wxString(wxT("\""))))) ||
					(lastcmd.Left(18) == wxT("cassetteplayer new"))){
						m_appWindow->m_videoControlPage->UpdateSetting (data->name, data->contents);
						m_appWindow->m_sessionPage->UpdateSessionData();
				}
			}
			break;
		case CatapultXMLParser::TAG_REPLY:
			switch (data->replyState) {
				case CatapultXMLParser::REPLY_OK:
					if (PeekPendingCommandTarget() == TARGET_STARTUP) {
						HandleNormalLaunchReply(event);
					}
					else{
						wxString command = GetPendingCommand();
						if (command == wxT("openmsx_info fps")) {
							m_appWindow->SetFPSdisplay(data->contents);
						}
						else if (command == wxT("save_settings")){
							wxMessageBox (wxT("openMSX settings saved successfully!"));
						}
						else if (command.Left(10) == wxT("screenshot")){
							m_appWindow->m_videoControlPage->UpdateScreenshotCounter();
						}
					}
					break;
				case CatapultXMLParser::REPLY_NOK:
					{
						if (PeekPendingCommandTarget() == TARGET_STARTUP) {
							HandleNormalLaunchReply(event);
						}
						else{
							wxString command = GetPendingCommand();
							if (command == wxT("plug msx-midi-in midi-in-reader")) {
								m_appWindow->m_audioControlPage->InvalidMidiInReader();
							}
							else if (command == wxT("plug msx-midi-out midi-out-logger")) {
								m_appWindow->m_audioControlPage->InvalidMidiOutLogger();
							}
							else if (command == wxT("plug pcminput wavinput")) {
								m_appWindow->m_audioControlPage->InvalidSampleFilename();
							}
							else if (command == wxT("plug printerport logger")) {
								m_appWindow->m_miscControlPage->InvalidPrinterLogFilename();
							}
							else if (command == wxT("save_settings")){
								wxMessageBox (wxString (wxT("Error saving openMSX settings\n"))+data->contents);
							}
							else {
								m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxBOLD)));
								m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("Warning: NOK received on command: "));
								m_appWindow->m_statusPage->m_outputtext->AppendText(command);
								m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
								if (!data->contents.IsEmpty()) {
									m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("contents = "));
									m_appWindow->m_statusPage->m_outputtext->AppendText(data->contents);
									m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
								}
							}
						}
					}
					break;
				case CatapultXMLParser::REPLY_UNKNOWN:
					m_appWindow->m_statusPage->m_outputtext->SetDefaultStyle(wxTextAttr(wxColour(174,0,0),wxNullColour,wxFont(10,wxMODERN,wxNORMAL,wxBOLD)));
					m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("Warning: Unknown reply received!\n"));
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
			m_appWindow->m_statusPage->m_outputtext->AppendText(wxT("\n"));
			if (data->contents.Left(15) == wxT("Screen saved to")){
				int inhibit;
				ConfigurationData::instance()->GetParameter(ConfigurationData::CD_SCREENSHOTINFO,&inhibit);
				if (inhibit == 0){
					ScreenshotDlg dlg(m_appWindow);
					dlg.ShowModal();
				}
			}
			break;
		default:
			break;
	}
	delete data;
}

bool openMSXController::WriteCommand(wxString msg, TargetType target)
{
	if (!m_openMsxRunning)
		return false;
	CommandEntry temp;
	temp.command = msg;
	temp.target = target;
	m_commands.push_back(temp);
	char * convertBuffer = new char[msg.Len()+1];
	strcpy(convertBuffer,(const char*) (wxConvUTF8.cWX2MB(msg)));
	xmlChar* buffer = xmlEncodeEntitiesReentrant(NULL,(const xmlChar *)convertBuffer);
	char * commandBuffer = new char[strlen((const char *) buffer)+25];
	strcpy (commandBuffer,"<command>");
	strcat (commandBuffer,(const char *)buffer);
	strcat (commandBuffer,"</command>\n");
	bool result = WriteMessage((xmlChar *)commandBuffer,strlen(commandBuffer));
	if (buffer != NULL) {
		xmlFree(buffer);
	}
	return result;
}

wxString openMSXController::GetPendingCommand()
{
//	assert (!m_commands.empty());
	CommandEntry entry;
	wxString pending;
	if (m_commands.empty()){	// TODO: why is assert (!m_commands.empty()) triggered ?
		pending = wxT("");			// it can only happen if a reply is received without a previously sent command
	}
	else{
		entry = m_commands.front();
		pending = entry.command;
		m_commands.pop_front();
	}
	return wxString(pending);
}

wxString openMSXController::PeekPendingCommand()
{
	CommandEntry entry;
	wxString pending;
	if (m_commands.empty()) {
		pending = wxT("");
	}
	else {
		entry = m_commands.front();
		pending = entry.command;
	}
	return wxString(pending);
}

enum openMSXController::TargetType openMSXController::PeekPendingCommandTarget()
{
	CommandEntry entry;
	TargetType pending;
	if (m_commands.empty()) {
		pending = TARGET_INTERACTIVE;
	}
	else {
		entry = m_commands.front();
		pending = entry.target;
	}
	return pending;
}

bool openMSXController::StartOpenMSX(wxString cmd, bool getversion)
{
	m_openMSXID++;
	bool retval = true;
	if (getversion) {
		m_appWindow->SetStatusText(wxT("Initializing..."));
		wxString versioninfo = GetOpenMSXVersionInfo(cmd);
		retval = SetupOpenMSXParameters(versioninfo);
		m_appWindow->SetStatusText(wxT("Ready"));
	}
	else {
		m_appWindow->SetStatusText(wxT("Running"));
		m_appWindow->EnableSaveSettings(true);
		Launch(cmd);
	}
	return retval;
}

void openMSXController::HandleNormalLaunchReply(wxCommandEvent &event)
{
	executeLaunch(&event);
}


int openMSXController::InitConnectors(wxString dummy, wxString connectors)
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
		pos = temp.Find(wxT("\n"));
		if (pos != -1)
		{
			m_connectors.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);
		}
	}while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_connectors.Add(temp);
	return 0; // don't skip any lines in the startup script
}


void openMSXController::GetConnectors (wxArrayString & connectors)
{
	connectors.Clear();
	if (m_connectors.IsEmpty()) return;
	for (unsigned int i=0;i<m_connectors.GetCount();i++) {
		connectors.Add(m_connectors[i]);
	}
}

wxString openMSXController::GetConnectorClass (wxString name)
{
	if (m_connectorclasses.IsEmpty()) return wxString (wxT(""));
	unsigned index = 0;
	bool found = false;
	while ((!found) && (index < m_connectors.GetCount())) {
		if (m_connectors[index] == name) {
			found = true;
		}
		else{
			index++;
		}
	}

	assert(found);

	return wxString (m_connectorclasses[index]);
}

wxString openMSXController::GetConnectorContents (wxString name)
{
	if (m_connectorcontents.IsEmpty()) return wxString (wxT(""));
	unsigned index = 0;
	bool found = false;
	while ((!found) && (index < m_connectors.GetCount())) {
		if (m_connectors[index] == name) {
			found = true;
		}
		else{
			index++;
		}
	}

	assert(found);

	return wxString (m_connectorcontents[index]);
}

int openMSXController::InitPluggables(wxString dummy, wxString pluggables)
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
		pos = temp.Find(wxT("\n"));
		if (pos != -1)
		{
			m_pluggables.Add(temp.Left(pos));
			temp = temp.Mid(pos + 1);
		}
	} while (pos !=-1);
	if (!temp.IsEmpty()) // not everything parsed ?
		m_pluggables.Add(temp);
	return 0; // don't skip any lines in the startup script
}

void openMSXController::GetPluggables (wxArrayString & pluggables)
{
	pluggables.Clear();
	if (m_pluggables.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggables.GetCount();i++) {
		pluggables.Add(m_pluggables[i]);
	}
}

void openMSXController::GetPluggableDescriptions (wxArrayString & descriptions)
{
	descriptions.Clear();
	if (m_pluggabledescriptions.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggabledescriptions.GetCount();i++) {
		descriptions.Add(m_pluggabledescriptions[i]);
	}
}

void openMSXController::GetPluggableClasses (wxArrayString & classes)
{
	classes.Clear();
	if (m_pluggableclasses.IsEmpty()) return;
	for (unsigned int i=0;i<m_pluggableclasses.GetCount();i++) {
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
	if (version.Mid(0,8) == wxT("openMSX ")) {
		int pos = version.Find('.');
		if (pos != -1) {
			temp.Mid(8,pos-8).ToLong(&majver);
			temp = temp.Mid(pos+1);
			pos = temp.Find('.');
			if (pos != -1) {
				temp.Mid(0,pos).ToLong(&minver);
				temp.Mid(pos+1).ToLong(&subver);
				ver = (((majver * 100) + minver)*100)+subver;
			}
		}
	}
	// printf ("Detected openMSX version: %d\n", ver);
	if (ver == -1) {
		wxMessageBox (wxT("Unable to determine openMSX version!\nPlease upgrade to 0.6.2 or higher.\n(Or contact the authors.)"),wxT("Error"));
		return false;
	}
	if (ver < 602) {
		wxMessageBox (wxT("The openMSX version you are using is too old!\nPlease upgrade to 0.6.2 or higher."),wxT("Error"));
		return false;
	}
	m_appWindow->m_miscControlPage->FillInitialJoystickPortValues();
	m_appWindow->m_launch_AbortButton->Enable(true);
	return true;
}

void openMSXController::InitLaunchScript ()
{
	m_launchScriptSize = 0;
	m_launchScript = new LaunchInstructionType [LAUNCHSCRIPT_MAXSIZE];
	AddLaunchInstruction (wxT("update enable setting"),wxT(""),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("update enable led"),wxT(""),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("set power on"),wxT("e"),wxT("power"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("unset renderer"),wxT("e"),wxT(""),NULL,true);
	AddLaunchInstruction (wxT("@execute"),wxT(""),wxT(""),&openMSXController::EnableMainWindow,false);
	AddLaunchInstruction (wxT("^info renderer"),wxT(""),wxT("RendererSelector"),&openMSXController::FillComboBox,true);
	AddLaunchInstruction (wxT("^info scale_algorithm"),wxT(""),wxT("ScalerAlgoSelector"),&openMSXController::FillComboBox,true);
	AddLaunchInstruction (wxT("lindex [openmsx_info setting scale_factor] 2"),wxT(""),wxT("ScalerFactorSelector"),&openMSXController::FillRangeComboBox,true);
	AddLaunchInstruction (wxT("^info accuracy"),wxT(""),wxT("AccuracySelector"),&openMSXController::FillComboBox,false);
	AddLaunchInstruction (wxT("update enable media"),wxT(""),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("info exist frontswitch"),wxT(""),wxT("#"),&openMSXController::EnableFirmware,false);
	AddLaunchInstruction (wxT("info exist firmwareswitch"),wxT(""),wxT("#"),&openMSXController::EnableFirmware,false);
	AddLaunchInstruction (wxT("set renderer"),wxT(""),wxT("renderer"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set scale_algorithm"),wxT(""),wxT("scale_algorithm"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set scale_factor"),wxT(""),wxT("scale_factor"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set accuracy"),wxT(""),wxT("accuracy"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set deinterlace"),wxT(""),wxT("deinterlace"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set limitsprites"),wxT(""),wxT("limitsprites"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set fullscreen"),wxT(""),wxT("fullscreen"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set blur"),wxT(""),wxT("blur"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set glow"),wxT(""),wxT("glow"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set gamma"),wxT(""),wxT("gamma"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set scanline"),wxT("0"),wxT("scanline"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("info exist renshaturbo"),wxT("1"),wxT("#"),&openMSXController::EnableRenShaTurbo,false);
	AddLaunchInstruction (wxT("set renshaturbo"),wxT("0"),wxT("renshaturbo"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set save_settings_on_exit"),wxT(""),wxT("save_settings_on_exit"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set printerlogfilename"),wxT(""),wxT("printerlogfilename"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("@execute"),wxT(""),wxT(""),&openMSXController::SetSliderDefaults,false);
	AddLaunchInstruction (wxT("set speed"),wxT(""),wxT("speed"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set maxframeskip"),wxT(""),wxT("maxframeskip"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set minframeskip"),wxT(""),wxT("minframeskip"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set throttle"),wxT(""),wxT("throttle"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set cmdtiming"),wxT(""),wxT("cmdtiming"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("!info pluggable"),wxT("13"),wxT(""),&openMSXController::InitPluggables,true);
	AddLaunchInstruction (wxT("!info_nostore pluggable *"),wxT(""),wxT("*"),&openMSXController::AddPluggableDescription,true);
	AddLaunchInstruction (wxT("!info_nostore connectionclass *"),wxT(""),wxT("*"),&openMSXController::AddPluggableClass,false);
	AddLaunchInstruction (wxT("!info connector"),wxT("10"),wxT(""),&openMSXController::InitConnectors,true);
	AddLaunchInstruction (wxT("!info_nostore connectionclass *"),wxT(""),wxT("*"),&openMSXController::AddConnectorClass,false);
	AddLaunchInstruction (wxT("plug *"),wxT(""),wxT("*"),&openMSXController::AddConnectorContents,true);
	AddLaunchInstruction (wxT("@checkfor msx-midi-in"),wxT("1"),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("set midi-in-readfilename"),wxT(""),wxT("midi-in-readfilename"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("@checkfor msx-midi-out"),wxT("1"),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("set midi-out-logfilename"),wxT(""),wxT("midi-out-logfilename"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("@checkfor pcminput"),wxT("1"),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("set audio-inputfilename"),wxT(""),wxT("audio-inputfilename"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("@execute"),wxT(""),wxT(""),&openMSXController::InitConnectorPanel,false);
	AddLaunchInstruction (wxT("@execute"),wxT(""),wxT(""),&openMSXController::InitAudioConnectorPanel,false);
//	AddLaunchInstruction (wxT("#info romtype"),wxT(""),wxT(""),&openMSXController::InitRomTypes,true);
//	AddLaunchInstruction (wxT("#info_nostore romtype *"),wxT(""),wxT("*"),&openMSXController::SetRomDescription,true);
	AddLaunchInstruction (wxT("!info sounddevice"),wxT("5"),wxT(""),&openMSXController::InitSoundDevices,true);
	AddLaunchInstruction (wxT("!info_nostore sounddevice *"),wxT(""),wxT("*"),&openMSXController::SetChannelType,true);
	AddLaunchInstruction (wxT("set master_volume"),wxT(""),wxT("master_volume"),&openMSXController::UpdateSetting,false);
	AddLaunchInstruction (wxT("set *_volume"),wxT(""),wxT("*_volume"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set *_balance"),wxT(""),wxT("*_balance"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("set mute"),wxT(""),wxT("mute"),&openMSXController::UpdateSetting,true);
	AddLaunchInstruction (wxT("plug cassetteport"),wxT(""),wxT("cassetteport"),&openMSXController::EnableCassettePort,false);
	AddLaunchInstruction (wxT("join [cassetteplayer] \\n"),wxT(""),wxT(""),&openMSXController::SetCassetteMode,false);
	AddLaunchInstruction (wxT("update enable plug"),wxT(""),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("update enable unplug"),wxT(""),wxT(""),NULL,false);
	AddLaunchInstruction (wxT("update enable status"),wxT(""),wxT(""),NULL,false);
}

void openMSXController::AddLaunchInstruction (wxString cmd, wxString action,
		wxString parameter,
		int (openMSXController::*pfunction)(wxString,wxString),
		bool showError)
{
	if (m_launchScriptSize >= LAUNCHSCRIPT_MAXSIZE) {
		wxMessageBox (wxT("Not enough space to store the Launchscript!\nPlease contact the authors."),wxT("Internal Catapult Error"));
		return;
	}
	// Add the instruction, parameters and other flags to the launch script
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
	if (event != NULL) {
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
	static wxString lastdata = wxT("");

	wxString command;
	if (event != NULL) { // handle received command
		command = GetPendingCommand();
		instruction  = m_launchScript[recvStep].command;
		while (instruction.Mid(0,1) == wxT("@")) {
			recvStep ++;
			instruction  = m_launchScript[recvStep].command;
		}
		if ((recvLoop == -1) && (instruction.Find(wxT("*")) != -1)) {
			recvLoop = 0;
		}
		tokenize(instruction,wxT(" "),tokens);
		cmd = translate(tokens,recvLoop,lastdata);
		if (command == cmd) {
			if (tokens[0] == wxT("#info")) {
				lastdata = data->contents;
			}
			if (tokens[0] == wxT("^info")) {
				lastdata = data->contents;
			}
			if (tokens[0] == wxT("!info")) {
				lastdata = data->contents;
			}
			HandleLaunchReply (cmd,event,m_launchScript[recvStep],&sendStep,recvLoop,lastdata);
			if ((data->replyState == CatapultXMLParser::REPLY_NOK) ||
				((cmd.Mid(0,10) == wxT("info exist")) && (data->contents == wxT("0")))) {
				long displace;
				m_launchScript[recvStep].scriptActions.ToLong(&displace);
				recvStep += displace;
			}
			if (cmd == wxT("!done")) {
				recvStep = -2; // it is gonna be increased in a moment
				recvLoop = -1;
			}

			if (recvLoop != -1) {
				wxArrayString lastvalues;
				if (recvLoop < (tokenize(lastdata,wxT("\n"),lastvalues)-1)) {
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
		if (recvStep >= m_launchScriptSize) {
			recvStep = 0;
			FinishLaunch();
			return;
		}
		if (wait) {
			while (m_launchScript[recvStep].command.Mid(0,1) == wxT("@")) {
				recvStep ++;
			}
			if (recvStep >= sendStep) {
			wait = false;
			}
		}

		while ((!wait) && (sendStep < m_launchScriptSize)) {

				instruction  = m_launchScript[sendStep].command;
				if ((sendLoop == -1) && (instruction.Find(wxT("*")) != -1)) {
					sendLoop = 0;
				}
				tokenize(instruction,wxT(" "),tokens);
				cmd = translate(tokens,sendLoop,lastdata);
				if (cmd.Mid(0,1) == wxT("@")) {
					wxString result = wxT("0");
					if (tokens[0] == wxT("@checkfor")) {
						bool contains = lastdata.Contains(tokens[1]);
						if (contains) {
							result = wxT("1");
						}
					}
					else { // @execute
						result = wxT("1");
					}
					if (result == wxT("0")) {
						while (m_launchScript[recvStep].command.Mid(0,1) == wxT("@")) {
							recvStep ++;
						}
						long displace;
						m_launchScript[sendStep].scriptActions.ToLong(&displace);
						recvStep += displace;
					}
					HandleLaunchReply (tokens[0] + result,NULL,m_launchScript[sendStep],&sendStep,-1,wxT(""));

				}
				else {
					WriteCommand (cmd,TARGET_STARTUP);
				}
				action = m_launchScript[sendStep].scriptActions;

				if (sendLoop != -1) {
					wxArrayString lastvalues;
					if (sendLoop < (tokenize(lastdata,wxT("\n"),lastvalues)-1)) {
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

				if ( action != wxT("")) {
					if (recvStep < sendStep) {
						wait = true;
					}
				}
		}
		return;
}

void openMSXController::FinishLaunch()
{
	m_appWindow->m_sessionPage->AutoPlugCassette();
	wxSize tempsize = m_appWindow->GetSize();
	tempsize.SetHeight(tempsize.GetHeight()+1);
	tempsize.SetWidth(tempsize.GetWidth()+1);
	m_appWindow->SetSize(tempsize);
#ifdef __WXMSW__
	tempsize.SetHeight(tempsize.GetHeight()-1);
	tempsize.SetWidth(tempsize.GetWidth()-1);
	m_appWindow->SetSize(tempsize);
#endif
	m_appWindow->SetControlsOnLaunch();
	m_appWindow->m_sessionPage->SetCassetteControl();
}


int openMSXController::tokenize (wxString text, wxString seperator, wxArrayString & result)
{
	int pos;
	result.Clear();
	wxString temp = text;
	do{
		pos = temp.Find(seperator);
		if (pos != -1) {
			if (pos != 0) { // ignore multiple seperators
				result.Add(temp.Mid(0,pos));
				temp = temp.Mid(pos + seperator.Len());
			}
			else{
				temp = temp.Mid(seperator.Len());
			}
		}
	}while (pos != -1);
	if (!temp.IsEmpty()) {
		result.Add(temp);
	}
	return result.GetCount();
}

wxString openMSXController::translate(wxArrayString tokens, int loop, wxString lastdata)
{
	unsigned int token;
	for (token=0;token<tokens.GetCount();token++) {

		if (tokens[token].Find(wxT("*")) != -1) {
			if (loop != -1) {
				wxArrayString lastvalues;
				tokenize (lastdata,wxT("\n"),lastvalues);
				if (loop < (int)lastvalues.GetCount()) {
						tokens[token].Replace(wxT("*"),lastvalues[loop],true);
						tokens[token].Replace (wxT(" "),wxT("\\ "),true);
				}
			}
		}
	}
	token = 0;
	while (token < tokens.GetCount()) {
		switch (tokens[token][(size_t)0]) {
		case '#':
			if (tokens[token].Mid(0,5)== wxT("#info")) {
				wxString parameter = wxT("");
				while (token < (tokens.GetCount()-1)) {
					parameter += tokens[token+1];
					parameter += wxT(" ");
					tokens.RemoveAt(token+1);
				}
				parameter.Trim(true);
				tokens[token] = wxString(wxT("join [openmsx_info ") + parameter + wxT("] \\n"));
			}
			else {
				assert(false); // invalid command
			}
			break;
		case '!':
			if (tokens[token].Mid(0,5)== wxT("!info")) {
				wxString parameter = wxT("");
				while (token < (tokens.GetCount()-1)) {
					parameter += tokens[token+1];
					parameter += wxT(" ");
					tokens.RemoveAt(token+1);
				}
				parameter.Trim(true);
				tokens[token] = wxString(wxT("join [machine_info ") + parameter + wxT("] \\n"));
			}
			else {
				assert(false); // invalid command
			}
			break;
		case '^':
			if (tokens[token].Mid(0,5)== wxT("^info")) {
				wxString parameter = wxT("");
				while (token < (tokens.GetCount()-1)) {
					parameter += tokens[token+1];
					parameter += wxT(" ");
					tokens.RemoveAt(token+1);
				}
				parameter.Trim(true);
				tokens[token] = wxString(wxT("join [lindex [openmsx_info setting ") + parameter + wxT("] 2] \\n"));
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
	wxString result= wxT("");
	for (token=0;token<tokens.GetCount();token++) {
		result += tokens[token];
		result += wxT(" ");
	}
	result.Trim(true);
	return wxString (result);
}

void openMSXController::HandleLaunchReply (wxString cmd,wxCommandEvent * event,
	LaunchInstructionType instruction ,int * sendStep, int loopcount,
	wxString datalist)
{
	CatapultXMLParser::ParseResult * data = NULL;
	if (event != NULL) {
		data = (CatapultXMLParser::ParseResult *)event->GetClientData();
	}
	bool ok = false;
	if (cmd.Mid(0,1)== wxT("@")) {
		if (cmd.Mid(cmd.Len()-1) == wxT("1")) {
			ok = true;
		}
	}
	else{
		if (cmd.Mid(0,10) == wxT("info exist")) {
			if (data->contents == wxT("1")) {
				ok = true;
			}
		}
		else{
			if (data->replyState == CatapultXMLParser::REPLY_OK) {
				ok = true;
			}
		}
	}
	wxString actions = instruction.scriptActions;

	if (ok) {
		if (instruction.p_okfunction != NULL) {
			wxString parameter = instruction.parameter;
			wxString contents = wxT("");
			if (loopcount > -1) {
				wxArrayString temp;
				tokenize (datalist,wxT("\n"),temp);
				parameter.Replace(wxT("*"),temp[loopcount],true);
				parameter.Replace(wxT(" "),wxT("\\ "),true);
			}
			if (parameter == wxT("#")) {
				parameter = cmd;
			}
			if (event != NULL) {
				contents = data->contents;
			}
			int result = (*this.*(instruction.p_okfunction))(parameter, contents);
			if (result >0) {
				*sendStep += result;
			}
		}
	}
	else {
		if (instruction.showError) {
			m_appWindow->m_statusPage->m_outputtext->AppendText(wxString(wxT("Error received on command: ") + cmd + wxT("\n")));
		}
		if (actions != wxT("")) {
			if (actions == wxT("e")) {
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

int openMSXController::UpdateSetting (wxString setting,wxString data)
{
	m_appWindow->m_videoControlPage->UpdateSetting(setting,data); // Just use any instance of CatapultPage
	return 0; // don't skip any lines in the startup script
}

int openMSXController::FillComboBox (wxString setting,wxString data)
{
	m_appWindow->m_videoControlPage->FillComboBox(setting,data); // Just use any instance of CatapultPage
	return 0; // don't skip any lines in the startup script
}

int openMSXController::FillRangeComboBox (wxString setting, wxString data)
{
	long min;
	long max;
	wxString range = wxT("");
	int pos = data.Find(' ');
	if (pos != -1){
		data.Left(pos).ToLong(&min);
		data.Mid(pos+1).ToLong(&max);
		for (int index=min;index<=max;index++){
			range << index;
			range += wxT("\n");
		}
	}

	m_appWindow->m_videoControlPage->FillComboBox(setting,range); // Just use any instance of CatapultPage
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableFirmware (wxString cmd, wxString data)
{
	if ((data != wxT("0")) || (cmd.Mid(0,4) == wxT("set "))) {
		if (cmd.Find(wxT("frontswitch")) != -1){
			m_appWindow->m_miscControlPage->EnableFirmware(wxT("frontswitch"));
		}
		else{
			m_appWindow->m_miscControlPage->EnableFirmware(wxT("firmwareswitch"));
		}
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableRenShaTurbo (wxString cmd, wxString data)
{
	if ((data != wxT("0")) || (cmd.Mid(0,4) == wxT("set "))) {
		m_appWindow->m_miscControlPage->EnableRenShaTurbo();
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableMainWindow(wxString dummy1, wxString dummy2)
{
	m_appWindow->EnableMainWindow();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitRomTypes (wxString dummy, wxString data)
{
	wxArrayString types;
	tokenize (data, wxT("\n"),types);
	for (unsigned int index=0;index < types.GetCount();index++)
	{
		m_appWindow->m_sessionPage->AddRomType (types[index]);
	}
	return 0;
}

int openMSXController::SetRomDescription(wxString name, wxString data)
{
	m_appWindow->m_sessionPage->SetRomTypeFullName(name,data);
	return 0;
}

int openMSXController::InitSoundDevices (wxString dummy, wxString data)
{
	wxArrayString channels;
	tokenize(data,wxT("\n"),channels);
	if (channels.GetCount() != (m_appWindow->m_audioControlPage->GetNumberOfAudioChannels()-1)) {
		m_appWindow->m_audioControlPage->DestroyAudioMixer();
		m_appWindow->m_audioControlPage->InitAudioChannels(data);
		return 0;
	}
	return 5; // skip 5 instructions TODO: improve this
}

int openMSXController::SetChannelType (wxString name,wxString data)
{
	int maxchannels = m_appWindow->m_audioControlPage->GetNumberOfAudioChannels();
	int index = 0;
	bool found = false;
	while ((!found) && (index < maxchannels)) {
		if (m_appWindow->m_audioControlPage->GetAudioChannelName(index) == name) {
			found = true;
		}
		else{
			index++;
		}
	}

	assert(found);

	m_appWindow->m_audioControlPage->AddChannelType(index,data);
	if (index == (maxchannels-1)) {
		m_appWindow->m_audioControlPage->SetupAudioMixer();
	}
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddPluggableDescription(wxString name,wxString data)
{
	m_pluggabledescriptions.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddPluggableClass(wxString name, wxString data)
{
	m_pluggableclasses.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddConnectorClass(wxString name, wxString data)
{
	m_connectorclasses.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::AddConnectorContents(wxString name, wxString data)
{
	m_connectorcontents.Add(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::SetSliderDefaults (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_videoControlPage->SetSliderDefaults();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitAudioConnectorPanel (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_audioControlPage->InitAudioIO();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::InitConnectorPanel (wxString dummy1, wxString dummy2)
{
	m_appWindow->m_miscControlPage->InitConnectorPanel();
	return 0; // don't skip any lines in the startup script
}

int openMSXController::EnableCassettePort (wxString dummy, wxString data)
{
	m_appWindow->m_sessionPage->EnableCassettePort(data);
	return 0; // don't skip any lines in the startup script
}

int openMSXController::SetCassetteMode (wxString dummy, wxString data)
{
	wxArrayString arrayData;
	int args = tokenize(data,wxT("\n"),arrayData);
	m_appWindow->m_sessionPage->SetCassetteMode (arrayData[args-1]);
	return 0;
}

void openMSXController::UpdateMixer()
{
	executeLaunch(NULL,43);
}
