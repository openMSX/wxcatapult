// $Id: StatusPage.h,v 1.3 2004/02/05 20:02:35 h_oudejans Exp $
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
		wxStaticBitmap * m_powerLed;	// this should be replaced with real leds in time
		wxStaticBitmap * m_capsLed;
		wxStaticBitmap * m_kanaLed;
		wxStaticBitmap * m_pauseLed;
		wxStaticBitmap * m_turboLed;
		wxStaticBitmap * m_fddLed;

		DECLARE_CLASS(StatusPage)
			// any class wishing to process wxWindows events must use this macro
			DECLARE_EVENT_TABLE()
};

#endif 