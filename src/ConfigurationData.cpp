// $Id: ConfigurationData.cpp,v 1.7 2004/10/08 15:24:39 h_oudejans Exp $
// onfigurationData.cpp: implementation of the ConfigurationData class.
//
//////////////////////////////////////////////////////////////////////
#include "ConfigurationData.h"

#if OPENMSX_DEMO_CD_VERSION
#include "wx/fileconf.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConfigurationData::ConfigurationData()
{
#if OPENMSX_DEMO_CD_VERSION
	ConfigData = new wxFileConfig (wxGetApp().GetAppName(), wxGetApp().GetVendorName(),
			wxT("catapult.dat"), wxT(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH );
#else
	ConfigData = wxConfigBase::Create();
#endif
	ConfigData->Read(wxT("/openMSXpaths/ExecPath"),&m_openMSXExecPath);
	ConfigData->Read(wxT("/openMSXpaths/SharePath"),&m_openMSXSharePath);
	ConfigData->Read(wxT("/history/DiskA"),&m_diskaHistory);
	ConfigData->Read(wxT("/history/DiskB"),&m_diskbHistory);
	ConfigData->Read(wxT("/history/CartA"),&m_cartaHistory);
	ConfigData->Read(wxT("/history/CartB"),&m_cartbHistory);
	ConfigData->Read(wxT("/history/Tape1"),&m_tape1History);
	ConfigData->Read(wxT("/history/MediaInserted"),&m_mediaInserted);
	ConfigData->Read(wxT("/history/UsedMachine"),&m_usedMachine);
	ConfigData->Read(wxT("/history/UsedExtensions"), &m_usedExtensions);
	ConfigData->Read(wxT("/preferences/ShowFullScreenWarning"),&m_showFullScreenWarning);
	ConfigData->Read(wxT("/preferences/ShowScreenshotInfo"),&m_showScreenshotInfo);
	ConfigData->Read(wxT("/connectors/UsedJoyport1"),&m_usedJoyport1);
	ConfigData->Read(wxT("/connectors/UsedJoyport2"),&m_usedjoyport2);
//	ConfigData->Read(wxT("/connectors/UsedPrinterport"),&m_usedPrinterport); // saving disabled
//	ConfigData->Read(wxT("/connectors/UsedPrinterfile"),&m_usedPrinterfile);
}

ConfigurationData::~ConfigurationData()
{
#if OPENMSX_DEMO_CD_VERSION	
	delete ConfigData;
#endif
}

bool ConfigurationData::HaveRequiredSettings()
{
	if (m_openMSXExecPath.IsEmpty() || !::wxFileExists(m_openMSXExecPath))
		return false;
	if (m_openMSXSharePath.IsEmpty() || !::wxDirExists(m_openMSXSharePath))
		return true;
	return true;
}

bool ConfigurationData::SetParameter(int p_iId, wxVariant p_data)
{
	bool retVal = true;
	switch (p_iId)
	{
		case CD_EXECPATH:
			m_openMSXExecPath = p_data.GetString();
			break;
		case CD_SHAREPATH:
			m_openMSXSharePath = p_data.GetString();
			break;
		case CD_HISTDISKA:
			m_diskaHistory = p_data.GetString();
			break;
		case CD_HISTDISKB:
			m_diskbHistory = p_data.GetString();
			break;
		case CD_HISTCARTA:
			m_cartaHistory = p_data.GetString();
			break;
		case CD_HISTCARTB:
			m_cartbHistory = p_data.GetString();
			break;
		case CD_HISTTAPE1:
			m_tape1History = p_data.GetString();
			break;
		case CD_USEDMACHINE:
			m_usedMachine = p_data.GetString();
			break;
		case CD_USEDEXTENSIONS:
			m_usedExtensions = p_data.GetString();
			break;
		case CD_MEDIAINSERTED:
			m_mediaInserted =p_data.GetLong();
			break;
		case CD_FULLSCREENWARN:
			m_showFullScreenWarning = p_data.GetLong();
			break;
		case CD_SCREENSHOTINFO:
			m_showScreenshotInfo = p_data.GetLong();
			break;
		case CD_JOYPORT1:
			m_usedJoyport1 = p_data.GetString();
			break;
		case CD_JOYPORT2:
			m_usedjoyport2 = p_data.GetString();
			break;
		case CD_PRINTERPORT:
			m_usedPrinterport = p_data.GetString();
			break;
		case CD_PRINTERFILE:
			m_usedPrinterfile = p_data.GetString();
			break;
		default:
			retVal = false; // unknown ID
			break;
	}
	return retVal;
}

bool ConfigurationData::GetParameter(int p_iId, wxString &p_data)
{
	bool retVal = true;
	switch (p_iId)
	{
		case CD_EXECPATH:
			p_data = m_openMSXExecPath;
			break;
		case CD_SHAREPATH:
			p_data = m_openMSXSharePath;
			break;
		case CD_HISTDISKA:
			p_data = m_diskaHistory;
			break;
		case CD_HISTDISKB:
			p_data = m_diskbHistory;
			break;
		case CD_HISTCARTA:
			p_data = m_cartaHistory;
			break;
		case CD_HISTCARTB:
			p_data = m_cartbHistory;
			break;
		case CD_HISTTAPE1:
			p_data = m_tape1History;
			break;
		case CD_USEDMACHINE:
			p_data = m_usedMachine;
			break;
		case CD_USEDEXTENSIONS:
			p_data = m_usedExtensions;
			break;
		case CD_JOYPORT1:
			p_data = m_usedJoyport1;
			break;
		case CD_JOYPORT2:
			p_data = m_usedjoyport2;
			break;
		case CD_PRINTERPORT:
			p_data = m_usedPrinterport;
			break;
		case CD_PRINTERFILE:
			p_data = m_usedPrinterfile;
			break;
		default:
			retVal = false; // unknown ID
			break;
	}
	return retVal;
}

bool ConfigurationData::GetParameter (int p_iId, int * p_data)
{
	bool retVal = true;
	switch (p_iId)
	{
		case CD_MEDIAINSERTED:
			*p_data = m_mediaInserted;
			break;
		case CD_FULLSCREENWARN:
			*p_data = m_showFullScreenWarning;
			break;
		case CD_SCREENSHOTINFO:
			*p_data = m_showScreenshotInfo;
			break;
		default:
			retVal = false;
			break;
	}
	return retVal;
}


ConfigurationData * ConfigurationData::instance()
{
	static ConfigurationData oneInstance;
	return &oneInstance;
}

bool ConfigurationData::SaveData()
{
	bool retVal;
	// retVal is created as an n-parameter AND function.
	retVal =  ConfigData->Write(wxT("/openMSXpaths/ExecPath"),m_openMSXExecPath);
	retVal &= ConfigData->Write(wxT("/openMSXpaths/SharePath"),m_openMSXSharePath);

	retVal &= ConfigData->Write(wxT("/history/DiskA"),m_diskaHistory);
	retVal &= ConfigData->Write(wxT("/history/DiskB"),m_diskbHistory);
	retVal &= ConfigData->Write(wxT("/history/CartA"),m_cartaHistory);
	retVal &= ConfigData->Write(wxT("/history/CartB"),m_cartbHistory);
	retVal &= ConfigData->Write(wxT("/history/Tape1"),m_tape1History);
	retVal &= ConfigData->Write(wxT("/history/MediaInserted"),m_mediaInserted);
	retVal &= ConfigData->Write(wxT("/history/UsedMachine"),m_usedMachine);
	retVal &= ConfigData->Write(wxT("/history/UsedExtensions"),m_usedExtensions);
	retVal &= ConfigData->Write(wxT("/preferences/ShowFullScreenWarning"),m_showFullScreenWarning);
	retVal &= ConfigData->Write(wxT("/preferences/ShowScreenshotInfo"),m_showScreenshotInfo);
	retVal &= ConfigData->Write(wxT("/connectors/UsedJoyport1"),m_usedJoyport1);
	retVal &= ConfigData->Write(wxT("/connectors/UsedJoyport2"),m_usedjoyport2);
//	retVal &= ConfigData->Write(wxT("/connectors/UsedPrinterport"),m_usedPrinterport); //saving disabled
//	retVal &= ConfigData->Write(wxT("/connectors/UsedPrinterfile"),m_usedPrinterfile);
	return retVal;
}

