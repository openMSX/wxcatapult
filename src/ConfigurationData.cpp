#include "ConfigurationData.h"
#include <wx/config.h>

ConfigurationData::ConfigurationData()
{
	ConfigData = wxConfigBase::Create();
	ConfigData->SetExpandEnvVars(false); // we will not write env vars
	ConfigData->Read(wxT("/openMSXpaths/ExecPath"), &m_openMSXExecPath);
	ConfigData->Read(wxT("/configuration/InstalledMachines"), &m_installedMachines);
	ConfigData->Read(wxT("/configuration/InstalledExtensions"), &m_installedExtensions);
	ConfigData->Read(wxT("/history/DiskA"), &m_diskaHistory);
	ConfigData->Read(wxT("/history/DiskB"), &m_diskbHistory);
	ConfigData->Read(wxT("/history/CartA"), &m_cartaHistory);
	ConfigData->Read(wxT("/history/CartB"), &m_cartbHistory);
	ConfigData->Read(wxT("/history/Cassette"), &m_cassetteHistory);
	ConfigData->Read(wxT("/history/Harddisk"), &m_hardDiskHistory);
	ConfigData->Read(wxT("/history/IpsDiskA"), &m_diskaIps);
	ConfigData->Read(wxT("/history/IpsDiskB"), &m_diskbIps);
	ConfigData->Read(wxT("/history/IpsCartA"), &m_cartaIps);
	ConfigData->Read(wxT("/history/IpsCartB"), &m_cartbIps);
	ConfigData->Read(wxT("/history/TypeCartA"), &m_typeHistCartA);
	ConfigData->Read(wxT("/history/TypeCartB"), &m_typeHistCartB);
	ConfigData->Read(wxT("/history/MediaInserted"), &m_mediaInserted);
	ConfigData->Read(wxT("/history/UsedMachine"), &m_usedMachine);
	ConfigData->Read(wxT("/history/UsedExtensions"), &m_usedExtensions);
	ConfigData->Read(wxT("/preferences/ShowFullScreenWarning"), &m_showFullScreenWarning);
	ConfigData->Read(wxT("/preferences/ShowScreenshotInfo"), &m_showScreenshotInfo);
	ConfigData->Read(wxT("/preferences/AutoCreateCassetteRecordFile"), &m_cassetteAutoCreate);
	ConfigData->Read(wxT("/connectors/UsedJoyport1"), &m_usedJoyport1);
	ConfigData->Read(wxT("/connectors/UsedJoyport2"), &m_usedjoyport2);
	ConfigData->Read(wxT("/configuration/ViewFlags"), &m_viewFlags);
//	ConfigData->Read(wxT("/connectors/UsedPrinterport"), &m_usedPrinterport); // saving disabled
//	ConfigData->Read(wxT("/connectors/UsedPrinterfile"), &m_usedPrinterfile);
}

ConfigurationData& ConfigurationData::instance()
{
	static ConfigurationData oneInstance;
	return oneInstance;
}

bool ConfigurationData::HaveRequiredSettings() const
{
	if (m_openMSXExecPath.IsEmpty() || !::wxFileExists(m_openMSXExecPath)) {
		return false;
	}
	return true;
}

bool ConfigurationData::SetParameter(int p_iId, wxVariant p_data)
{
	switch (p_iId) {
	case CD_EXECPATH:
		m_openMSXExecPath = p_data.GetString();
		break;
	case CD_MACHINES:
		m_installedMachines = p_data.GetString();
		break;
	case CD_EXTENSIONS:
		m_installedExtensions = p_data.GetString();
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
	case CD_HISTCASSETTE:
		m_cassetteHistory = p_data.GetString();
		break;
	case CD_HISTHDD:
		m_hardDiskHistory = p_data.GetString();
		break;
	case CD_IPSDISKA:
		m_diskaIps = p_data.GetString();
		break;
	case CD_IPSDISKB:
		m_diskbIps = p_data.GetString();
		break;
	case CD_IPSCARTA:
		m_cartaIps = p_data.GetString();
		break;
	case CD_IPSCARTB:
		m_cartbIps = p_data.GetString();
		break;
	case CD_TYPEHISTCARTA:
		m_typeHistCartA = p_data.GetString();
		break;
	case CD_TYPEHISTCARTB:
		m_typeHistCartB = p_data.GetString();
		break;
	case CD_USEDMACHINE:
		m_usedMachine = p_data.GetString();
		break;
	case CD_USEDEXTENSIONS:
		m_usedExtensions = p_data.GetString();
		break;
	case CD_MEDIAINSERTED:
		m_mediaInserted = p_data.GetLong();
		break;
	case CD_FULLSCREENWARN:
		m_showFullScreenWarning = p_data.GetLong();
		break;
	case CD_AUTORECORD:
		m_cassetteAutoCreate = p_data.GetLong();
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
	case CD_VIEWFLAGS:
		m_viewFlags = p_data.GetLong();
		break;
	default:
		return false; // unknown ID
	}
	return true;
}

bool ConfigurationData::GetParameter(int p_iId, wxString& p_data) const
{
	switch (p_iId) {
	case CD_EXECPATH:
		p_data = m_openMSXExecPath;
		break;
	case CD_MACHINES:
		p_data = m_installedMachines;
		break;
	case CD_EXTENSIONS:
		p_data = m_installedExtensions;
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
	case CD_HISTCASSETTE:
		p_data = m_cassetteHistory;
		break;
	case CD_HISTHDD:
		p_data = m_hardDiskHistory;
		break;
	case CD_IPSDISKA:
		p_data = m_diskaIps;
		break;
	case CD_IPSDISKB:
		p_data = m_diskbIps;
		break;
	case CD_IPSCARTA:
		p_data = m_cartaIps;
		break;
	case CD_IPSCARTB:
		p_data = m_cartbIps;
		break;
	case CD_TYPEHISTCARTA:
		p_data = m_typeHistCartA;
		break;
	case CD_TYPEHISTCARTB:
		p_data = m_typeHistCartB;
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
		return false; // unknown ID
	}
	return true;
}

bool ConfigurationData::GetParameter(int p_iId, int* p_data) const
{
	switch (p_iId) {
	case CD_MEDIAINSERTED:
		*p_data = m_mediaInserted;
		break;
	case CD_FULLSCREENWARN:
		*p_data = m_showFullScreenWarning;
		break;
	case CD_AUTORECORD:
		*p_data = m_cassetteAutoCreate;
		break;
	case CD_SCREENSHOTINFO:
		*p_data = m_showScreenshotInfo;
		break;
	case CD_VIEWFLAGS:
		*p_data = m_viewFlags;
		break;
	default:
		return false;
	}
	return true;
}

bool ConfigurationData::SaveData() const
{
	bool retVal = true;
	retVal &= ConfigData->Write(wxT("/openMSXpaths/ExecPath"), m_openMSXExecPath);
	retVal &= ConfigData->Write(wxT("/configuration/InstalledMachines"), m_installedMachines);
	retVal &= ConfigData->Write(wxT("/configuration/InstalledExtensions"), m_installedExtensions);
	retVal &= ConfigData->Write(wxT("/history/DiskA"), m_diskaHistory);
	retVal &= ConfigData->Write(wxT("/history/DiskB"), m_diskbHistory);
	retVal &= ConfigData->Write(wxT("/history/CartA"), m_cartaHistory);
	retVal &= ConfigData->Write(wxT("/history/CartB"), m_cartbHistory);
	retVal &= ConfigData->Write(wxT("/history/Cassette"), m_cassetteHistory);
	retVal &= ConfigData->Write(wxT("/history/Harddisk"), m_hardDiskHistory);
	retVal &= ConfigData->Write(wxT("/history/IpsDiskA"), m_diskaIps);
	retVal &= ConfigData->Write(wxT("/history/IpsDiskB"), m_diskbIps);
	retVal &= ConfigData->Write(wxT("/history/IpsCartA"), m_cartaIps);
	retVal &= ConfigData->Write(wxT("/history/IpsCartB"), m_cartbIps);
	retVal &= ConfigData->Write(wxT("/history/TypeCartA"), m_typeHistCartA);
	retVal &= ConfigData->Write(wxT("/history/TypeCartB"), m_typeHistCartB);
	retVal &= ConfigData->Write(wxT("/history/MediaInserted"), m_mediaInserted);
	retVal &= ConfigData->Write(wxT("/history/UsedMachine"), m_usedMachine);
	retVal &= ConfigData->Write(wxT("/history/UsedExtensions"), m_usedExtensions);
	retVal &= ConfigData->Write(wxT("/preferences/ShowFullScreenWarning"), m_showFullScreenWarning);
	retVal &= ConfigData->Write(wxT("/preferences/ShowScreenshotInfo"), m_showScreenshotInfo);
	retVal &= ConfigData->Write(wxT("/preferences/AutoCreateCassetteRecordFile"), m_cassetteAutoCreate);
	retVal &= ConfigData->Write(wxT("/connectors/UsedJoyport1"), m_usedJoyport1);
	retVal &= ConfigData->Write(wxT("/connectors/UsedJoyport2"), m_usedjoyport2);
	retVal &= ConfigData->Write(wxT("/configuration/ViewFlags"), m_viewFlags);
//	retVal &= ConfigData->Write(wxT("/connectors/UsedPrinterport"), m_usedPrinterport); //saving disabled
//	retVal &= ConfigData->Write(wxT("/connectors/UsedPrinterfile"), m_usedPrinterfile);
	return retVal;
}
