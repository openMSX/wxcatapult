// $Id: openMSXLinuxController.h,v 1.12 2005/10/16 19:22:04 h_oudejans Exp $
// openMSXLinuxController.h: interface for the openMSXLinuxController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENMSXLINUXCONTROLLER_H__25F18E15_838A_453A_A819_B48FDFE01E28__INCLUDED_)
#define AFX_OPENMSXLINUXCONTROLLER_H__25F18E15_838A_453A_A819_B48FDFE01E28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/txtstrm.h>
#include "openMSXController.h"
#include <string> 

using std::string;

class PipeReadThread;

class openMSXLinuxController : public openMSXController  
{
	public:
		bool WriteMessage(xmlChar * msg,size_t length);
		virtual bool Launch (wxString cmdline);
		virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd);
		virtual void HandleNativeEndProcess ();
		openMSXLinuxController(wxWindow * target);
		virtual ~openMSXLinuxController();
	private:
		bool execute(const string& command, int& fdIn, int& fdOut, int& fdErr);
		int m_openMSXstdin;
		int m_openMSXstdout;
		int m_openMSXstderr;
		PipeReadThread * m_stdErrThread;
		PipeReadThread * m_stdOutThread;
		int m_socket;
};

#endif // !defined(AFX_OPENMSXLINUXCONTROLLER_H__25F18E15_838A_453A_A819_B48FDFE01E28__INCLUDED_)
