// $Id: InputPage.h,v 1.2 2004/10/02 20:50:45 h_oudejans Exp $
// InputPage.h: interface for the InputPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef AFX_INPUTPAGE_H
#define AFX_INPUTPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class openMSXController;

class InputPage : public CatapultPage  
{
	public:
		void SetControlsOnEnd();
		void SetControlsOnLaunch();
		InputPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller=NULL);
		virtual ~InputPage();
		wxTextCtrl * m_inputtext;
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus) {};
		void OnTypeText (wxCommandEvent &event);
		void OnClearText (wxCommandEvent &event);
		void OnTextChange (wxCommandEvent &event);
	private:
		openMSXController * m_controller;
		wxButton * m_typeTextButton;
		wxButton * m_clearTextButton;
		bool launched;

		DECLARE_CLASS(InputPage)
			// any class wishing to process wxWindows events must use this macro
		DECLARE_EVENT_TABLE()
};

#endif 
