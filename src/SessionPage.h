// $Id: SessionPage.h,v 1.22 2005/02/01 18:30:43 h_oudejans Exp $
// SessionPage.h: interface for the SessionPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SESSIONPAGE_H
#define SESSIONPAGE_H

#include "CatapultPage.h"
#include "wx/dnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RomTypeDlg;
class IPSSelectionDlg;
class openMSXController;
class wxCatapultFrame;

class SessionDropTarget : public wxFileDropTarget
{
public:	
	SessionDropTarget (wxComboBox * target);
	~SessionDropTarget ();
	bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
private:
	wxComboBox * m_target;
};


class SessionPage : public CatapultPage  
{
	public:
		SessionPage(wxWindow * parent = (wxWindow *)NULL, openMSXController * controller=NULL);
		virtual ~SessionPage();

	//	void OnClickCombo(wxCommandEvent &event);
		void OnBrowseCassette (wxCommandEvent &event);
		void OnBrowseDiskA (wxCommandEvent& event);
		void OnBrowseDiskB (wxCommandEvent& event);
		void OnBrowseCartA (wxCommandEvent& event);
		void OnBrowseCartB (wxCommandEvent& event);

		void OnClearCassette (wxCommandEvent & event);
		void OnClearCartB (wxCommandEvent & event);
		void OnClearCartA (wxCommandEvent & event);
		void OnClearDiskB (wxCommandEvent & event);
		void OnClearDiskA (wxCommandEvent & event);
		void OnRewind (wxCommandEvent & event);
		void OnForcePlay (wxCommandEvent & event);
		void OnForcePlayByMenu (wxCommandEvent & event);
		void OnClickDiskACombo (wxCommandEvent & event);
		void OnClickDiskBCombo (wxCommandEvent & event);
		void OnClickCartACombo (wxCommandEvent & event);
		void OnClickCartBCombo (wxCommandEvent & event);
		void OnClickCassetteCombo (wxCommandEvent & event);		
		void OnChangeDiskAContents (wxCommandEvent & event);
		void OnChangeDiskBContents (wxCommandEvent & event);
		void OnChangeCartAContents (wxCommandEvent & event);
		void OnChangeCartBContents (wxCommandEvent & event);
		void OnChangeCassetteContents (wxCommandEvent & event);
		void OnClickDiskMenu (wxCommandEvent & event);
		void OnClickCartMenu (wxCommandEvent & event);
		void OnClickCasMenu (wxCommandEvent & event);
		void OnInsertEmptyDiskByMenu (wxCommandEvent & event);
		void OnBrowseDiskByMenu (wxCommandEvent &event);
		void OnBrowseDiskIps(wxCommandEvent & event);
		void OnBrowseDiskDirByMenu (wxCommandEvent &event);
		void OnBrowseCartByMenu (wxCommandEvent &event);
		void OnEjectDiskByMenu (wxCommandEvent & event);
		void OnEjectCartByMenu (wxCommandEvent & event);
		void OnSelectMapper (wxCommandEvent & event);
		void OnSelectIPS (wxCommandEvent & event);
		void OnCloseMenu (wxMenuEvent & event);
		void SetupHardware (bool initial);
		void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus);
		void SetControlsOnLaunch();
		void SetControlsOnEnd();
		void getMedia(wxArrayString & parameters);
		void getTypes(wxArrayString & parameters);
		void getPatches(wxArrayString * parameters);
		void getHardware(wxArrayString & hardware);
		void UpdateSessionData();
		void EnableCassettePort (wxString data);
		void AutoPlugCassette ();
		void AddRomType (wxString romtype);
		void SetRomTypeFullName (wxString romtype, wxString fullname);
		void SetupRomType (wxString romtype, wxString fullname);

	private:
		struct mediaInfo
		{
			mediaInfo (wxMenu *		menu)
			{
				contents = wxT("");
				ips.Clear();
				type = wxT("");
				control = NULL;
				history.Clear();
				mmenu = menu; 
			};
			wxString contents;
			wxArrayString ips;
			wxString type; // only for carts at the moment
			wxComboBox * control;
			wxArrayString history;
			wxMenu * mmenu;
		};
		mediaInfo * GetLastMenuTarget();
		static int CompareCaseInsensitive(const wxString& first, const wxString& second);
		void GetRomTypes ();
		wxString ConvertRomType (wxString source, bool backwards);
		bool BrowseDisk (mediaInfo * target, wxString devicename, wxString defaultpath);
		void BrowseCart (mediaInfo * target, wxString defaultpath);
		void prepareMachines(wxString sharepath, wxArrayString & machineArray, bool optional=false);
		void fillMachines (wxArrayString & machineArray);
		void prepareExtensions (wxString sharepath, wxArrayString & extensionArray, bool optional=false);
		void fillExtensions (wxArrayString & extensionArray);
		void AddHistory (mediaInfo * media);
		void SaveHistory();
		void RestoreHistory();

		mediaInfo * m_diskA;
		mediaInfo * m_diskB;
		mediaInfo * m_cartA;
		mediaInfo * m_cartB;
		mediaInfo * m_cassette;

		wxButton * m_rewindButton;
		wxButton * m_diskAButton;
		wxButton * m_diskBButton;
		wxButton * m_cartAButton;
		wxButton * m_cartBButton;
		wxButton * m_cassetteButton;

		wxBitmapButton * m_browseCartA;
		wxBitmapButton * m_browseCartB;
		wxBitmapButton * m_clearCartA;
		wxBitmapButton * m_clearCartB;
		
		wxToggleButton * m_forcePlayButton;
		wxBitmapButton * m_browseDiskA;
		wxBitmapButton * m_browseDiskB;
		wxBitmapButton * m_browseCassette;
		wxBitmapButton * m_clearDiskA;
		wxBitmapButton * m_clearDiskB;
		wxBitmapButton * m_clearCassette;
		wxString m_lastDiskA;
		wxString m_lastDiskB;
		wxString m_lastCassette;
		wxString m_cassettePortState;

		wxStaticText * m_machineListLabel;
		wxStaticText * m_extensionListLabel;
		wxArrayString m_machineArray;
		wxArrayString m_extensionArray;

		int m_InsertedMedia;
		wxString m_usedMachine;
		wxString m_usedExtensions;
		openMSXController * m_controller;
		wxCatapultFrame * m_parent;

		wxMenu * m_diskMenu[2];
		wxMenu * m_cartMenu[2];
		wxMenu * m_casMenu;

		wxButton * m_lastUsedPopup;
		RomTypeDlg * m_romTypeDialog;
		IPSSelectionDlg * m_ipsDialog;

		DECLARE_CLASS(SessionPage)
			// any class wishing to process wxWindows events must use this macro
			DECLARE_EVENT_TABLE()

	public:
			wxComboBox * m_machineList;
			wxListBox * m_extensionList;


};

#endif 
