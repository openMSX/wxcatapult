// $Id$
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

class openMSXLinuxController : public openMSXController  
{
	public:
		bool WriteMessage(wxString msg);
		virtual bool Launch (wxString cmdline);
		openMSXLinuxController(wxWindow * target);
		virtual ~openMSXLinuxController();
	private:
		wxTextOutputStream * m_outputstream;
};

#endif // !defined(AFX_OPENMSXLINUXCONTROLLER_H__25F18E15_838A_453A_A819_B48FDFE01E28__INCLUDED_)
