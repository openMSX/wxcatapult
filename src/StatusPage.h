// $Id: StatusPage.h,v 1.2 2004/02/04 22:01:15 manuelbi Exp $
// StatusPage.h: interface for the StatusPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSPAGE_H__7715BFAD_F92F_4955_8A3B_E2DBF42E2774__INCLUDED_)
#define AFX_STATUSPAGE_H__7715BFAD_F92F_4955_8A3B_E2DBF42E2774__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StatusPage : public wxPanel  
{
	public:
		StatusPage(wxWindow * parent = (wxWindow *)NULL);
		virtual ~StatusPage();
		void UpdateLed (wxString ledname, wxString ledstate);
		wxTextCtrl * m_outputtext;

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

#endif // !defined(AFX_STATUSPAGE_H__7715BFAD_F92F_4955_8A3B_E2DBF42E2774__INCLUDED_)
