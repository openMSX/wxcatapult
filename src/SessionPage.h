// $Id: SessionPage.h,v 1.6 2004/03/28 15:11:51 h_oudejans Exp $
// SessionPage.h: interface for the SessionPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SESSIONPAGE_H
#define SESSIONPAGE_H

#include "CatapultPage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class openMSXController;
class wxCatapultFrame;
class SessionPage : public CatapultPage  
{
	public:
		SessionPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller=NULL);
		virtual ~SessionPage();

	//	void OnClickCombo(wxCommandEvent &event);
		void OnBrowseCasPatch(wxCommandEvent &event);
		void OnBrowseNormalTape (wxCommandEvent &event);
		void OnBrowseDiskA (wxCommandEvent& event);
		void OnBrowseDiskB (wxCommandEvent& event);

		void OnBrowseCartA (wxCommandEvent& event);
		void OnBrowseCartB (wxCommandEvent& event);

		void OnClearCasPatch (wxCommandEvent & event);
		void OnClearNormalTape (wxCommandEvent & event);
		void OnClearCartB (wxCommandEvent & event);
		void OnClearCartA (wxCommandEvent & event);
		void OnClearDiskB (wxCommandEvent & event);
		void OnClearDiskA (wxCommandEvent & event);
		void SetupHardware ();
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus);
		void SetControlsOnLaunch();
		void SetControlsOnEnd();

	private:
		static int CompareCaseInsensitive(const wxString& first, const wxString& second);
		bool BrowseDisk (wxComboBox * target, wxString devicename, wxString defaultpath);
		void BrowseCart (wxComboBox * target, wxString defaultpath);
		void prepareMachines(wxString sharepath, wxArrayString & machineArray, bool optional=false);
		void fillMachines (wxArrayString & machineArray);
		void prepareExtensions (wxString sharepath, wxArrayString & extensionArray, bool optional=false);
		void fillExtensions (wxArrayString & extensionArray);
		wxBitmapButton * m_browseDiskA;
		wxBitmapButton * m_browseDiskB;
		wxBitmapButton * m_browseTape1;
		wxBitmapButton * m_browseTape2;
		wxBitmapButton * m_clearDiskA;
		wxBitmapButton * m_clearDiskB;
		wxBitmapButton * m_clearTape1;
		wxBitmapButton * m_clearTape2;
		wxString m_lastDiskA;
		wxString m_lastDiskB;
		wxString m_lastTape1;
		wxString m_lastTape2;		
		
		openMSXController * m_controller;
		wxCatapultFrame * m_parent;

		DECLARE_CLASS(SessionPage)
			// any class wishing to process wxWindows events must use this macro
			DECLARE_EVENT_TABLE()

	public:
			wxComboBox * m_machineList;
			wxListBox * m_extensionList;

			wxComboBox * m_diskA;
			wxComboBox * m_diskB;
			wxComboBox * m_cartA;
			wxComboBox * m_cartB;
			wxComboBox * m_tape1;
			wxComboBox * m_tape2;

			wxBitmapButton * m_browseCartA;
			wxBitmapButton * m_browseCartB;
			wxBitmapButton * m_clearCartA;
			wxBitmapButton * m_clearCartB;
};

#endif 
