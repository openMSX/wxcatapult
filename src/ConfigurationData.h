#ifndef CONFIGURATIONDATA_H
#define CONFIGURATIONDATA_H

#include "wxCatapultApp.h"
#include <wx/variant.h>

class wxConfigBase;

class ConfigurationData
{
public:
	enum ID {
		CD_NONE = 0,
		CD_EXECPATH, CD_SHAREPATH, CD_MACHINES, CD_EXTENSIONS,
		CD_HISTDISKA, CD_HISTDISKB, CD_HISTCARTA, CD_HISTCARTB,
		CD_HISTCASSETTE, CD_HISTHDD, CD_TYPEHISTCARTA,
		CD_TYPEHISTCARTB, CD_IPSDISKA, CD_IPSDISKB, CD_IPSCARTA,
		CD_IPSCARTB, CD_MEDIAINSERTED, CD_USEDMACHINE,
		CD_USEDEXTENSIONS, CD_FULLSCREENWARN, CD_SCREENSHOTINFO,
		CD_JOYPORT1, CD_JOYPORT2, CD_PRINTERPORT, CD_PRINTERFILE,
		CD_VIEWFLAGS, CD_AUTORECORD
	};
	enum MediaBits {
		MB_DISKA    =  1,
		MB_DISKB    =  2,
		MB_CARTA    =  4,
		MB_CARTB    =  8,
		MB_CASSETTE = 16,
		MB_HDD      = 32,
	};
	enum ViewFlags {
		VF_BROKEN = 1
	};

	static ConfigurationData& instance();

	bool SaveData() const;
	bool GetParameter(int p_iId, wxString& p_data) const;
	bool GetParameter(int p_iId, int* p_data) const;
	bool SetParameter(int p_iId, wxVariant p_data);
	bool HaveRequiredSettings() const;

private:
	ConfigurationData();

	int m_mediaInserted;
	int m_viewFlags;
	int m_showFullScreenWarning;
	int m_showScreenshotInfo;
	int m_cassetteAutoCreate;
	wxString m_openMSXSharePath;
	wxString m_openMSXExecPath;
	wxString m_installedMachines;
	wxString m_installedExtensions;
	wxString m_diskaHistory;
	wxString m_diskbHistory;
	wxString m_cartaHistory;
	wxString m_cartbHistory;
	wxString m_cassetteHistory;
	wxString m_hardDiskHistory;
	wxString m_diskaIps;
	wxString m_diskbIps;
	wxString m_cartaIps;
	wxString m_cartbIps;
	wxString m_typeHistCartA;
	wxString m_typeHistCartB;
	wxString m_usedMachine;
	wxString m_usedExtensions;
	wxString m_usedJoyport1;
	wxString m_usedjoyport2;
	wxString m_usedPrinterport;
	wxString m_usedPrinterfile;

	wxConfigBase* ConfigData;
};

#endif
