// $Id: ConfigurationData.cpp,v 1.2 2004/02/04 22:01:04 manuelbi Exp $
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
			_("catapult.dat"), _(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH );
#else
	ConfigData = wxConfigBase::Create();
#endif
	ConfigData->Read(_("/openMSXpaths/ExecPath"),&m_openMSXExecPath);
	ConfigData->Read(_("/openMSXpaths/SharePath"),&m_openMSXSharePath);
	ConfigData->Read(_("/history/DiskA"),&m_diskaHistory);
	ConfigData->Read(_("/history/DiskB"),&m_diskbHistory);
	ConfigData->Read(_("/history/CartA"),&m_cartaHistory);
	ConfigData->Read(_("/history/CartB"),&m_cartbHistory);
	ConfigData->Read(_("/history/Tape1"),&m_tape1History);
	ConfigData->Read(_("/history/Tape2"),&m_tape2History);
	ConfigData->Read(_("/history/MediaInserted"),&m_mediaInserted);
	ConfigData->Read(_("/history/UsedMachine"),&m_usedMachine);
	ConfigData->Read(_("/history/UsedExtensions"), &m_usedExtensions);
	ConfigData->Read(_("/preferences/ShowFullScreenWarning"),&m_showFullScreenWarning);
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
		case CD_HISTTAPE2:
			m_tape2History = p_data.GetString();
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
		case CD_HISTTAPE2:
			p_data = m_tape2History;
			break;
		case CD_USEDMACHINE:
			p_data = m_usedMachine;
			break;
		case CD_USEDEXTENSIONS:
			p_data = m_usedExtensions;
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
	retVal =  ConfigData->Write(_("/openMSXpaths/ExecPath"),m_openMSXExecPath);
	retVal &= ConfigData->Write(_("/openMSXpaths/SharePath"),m_openMSXSharePath);

	retVal &= ConfigData->Write(_("/history/DiskA"),m_diskaHistory);
	retVal &= ConfigData->Write(_("/history/DiskB"),m_diskbHistory);
	retVal &= ConfigData->Write(_("/history/CartA"),m_cartaHistory);
	retVal &= ConfigData->Write(_("/history/CartB"),m_cartbHistory);
	retVal &= ConfigData->Write(_("/history/Tape1"),m_tape1History);
	retVal &= ConfigData->Write(_("/history/Tape2"),m_tape2History);
	retVal &= ConfigData->Write(_("/history/MediaInserted"),m_mediaInserted);
	retVal &= ConfigData->Write(_("/history/UsedMachine"),m_usedMachine);
	retVal &= ConfigData->Write(_("/history/UsedExtensions"),m_usedExtensions);
	retVal &= ConfigData->Write(_("/preferences/ShowFullScreenWarning"),m_showFullScreenWarning);
	return retVal;
}

