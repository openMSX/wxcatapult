// $Id: StatusPage.h,v 1.4 2004/03/23 16:31:19 h_oudejans Exp $
// StatusPage.h: interface for the StatusPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_STATUSPAGE_H
#define AFX_STATUSPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StatusPage : public CatapultPage  
{
	public:
		StatusPage(wxWindow * parent = (wxWindow *)NULL);
		virtual ~StatusPage();
		void UpdateLed (wxString ledname, wxString ledstate);
		wxTextCtrl * m_outputtext;
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus){};

	private:


		DECLARE_CLASS(StatusPage)
			// any class wishing to process wxWindows events must use this macro
			DECLARE_EVENT_TABLE()
};

#endif 