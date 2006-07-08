// $Id: ConfigurationData.h,v 1.12 2005/12/10 14:14:41 h_oudejans Exp $
// onfigurationData.h: interface for the ConfigurationData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONFIGURATIONDATA_H__A32B4A30_2F92_11D8_B9CD_00104B4B187E__INCLUDED_)
#define AFX_ONFIGURATIONDATA_H__A32B4A30_2F92_11D8_B9CD_00104B4B187E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wxCatapultApp.h"
#include "wx/variant.h"
#include "wx/config.h"

#if OPENMSX_DEMO_CD_VERSION
#include "wx/fileconf.h"
#endif

class ConfigurationData
{
	public:
		bool SaveData ();
		static ConfigurationData * instance();
		bool GetParameter (int p_iId, wxString & p_data);
		bool GetParameter (int p_iId, int * p_data);
		enum ID {CD_EXECPATH, CD_SHAREPATH,CD_MACHINES,CD_EXTENSIONS,CD_HISTDISKA, CD_HISTDISKB, CD_HISTCARTA, CD_HISTCARTB,
			CD_HISTCASSETTE, CD_TYPEHISTCARTA, CD_TYPEHISTCARTB, CD_MEDIAINSERTED, CD_USEDMACHINE,
			CD_USEDEXTENSIONS, CD_FULLSCREENWARN,CD_SCREENSHOTINFO,CD_JOYPORT1,CD_JOYPORT2,
			CD_PRINTERPORT,CD_PRINTERFILE,CD_VIEWFLAGS,CD_AUTORECORD
			};
			enum MediaBits {MB_DISKA=1, MB_DISKB=2, MB_CARTA=4, MB_CARTB=8, MB_CASSETTE=16};
			enum ViewFlags {VF_BROKEN=1};
			bool SetParameter (int p_iId, wxVariant p_data);
			bool HaveRequiredSettings ();

			virtual ~ConfigurationData();

	private:
			int m_mediaInserted;
			int m_viewFlags;
			int m_showFullScreenWarning;
			int m_showScreenshotInfo;
			int m_cassetteAutoCreate;
			ConfigurationData();
			wxString m_openMSXSharePath;
			wxString m_openMSXExecPath;
			wxString m_installedMachines;
			wxString m_installedExtensions;
			wxString m_diskaHistory;
			wxString m_diskbHistory;
			wxString m_cartaHistory;
			wxString m_cartbHistory;
			wxString m_cassetteHistory;
			wxString m_typeHistCartA;
			wxString m_typeHistCartB;
			wxString m_usedMachine;
			wxString m_usedExtensions;
			wxString m_usedJoyport1;
			wxString m_usedjoyport2;
			wxString m_usedPrinterport;
			wxString m_usedPrinterfile;

#if OPENMSX_DEMO_CD_VERSION
			wxFileConfig * ConfigData;
#else
			wxConfigBase * ConfigData;
#endif
};

#endif // !defined(AFX_ONFIGURATIONDATA_H__A32B4A30_2F92_11D8_B9CD_00104B4B187E__INCLUDED_)
