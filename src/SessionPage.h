// SessionPage.h: interface for the SessionPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SESSIONPAGE_H__93FE8492_93A5_411A_B50C_22F14696B5FC__INCLUDED_)
#define AFX_SESSIONPAGE_H__93FE8492_93A5_411A_B50C_22F14696B5FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class wxCatapultFrame;
class SessionPage : public wxPanel  
{
	public:
		SessionPage(wxWindow * parent = (wxWindow *)NULL);
		virtual ~SessionPage();

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
		void OnChangeMedia (wxCommandEvent & event);

	private:

		static int CompareCaseInsensitive(const wxString& first, const wxString& second);
		void BrowseDisk (wxComboBox * target, wxString defaultpath);
		void BrowseCart (wxComboBox * target, wxString defaultpath);
		void prepareMachines(wxString sharepath, wxArrayString & machineArray);
		void fillMachines (wxArrayString & machineArray);
		void prepareExtensions (wxString sharepath, wxArrayString & extensionArray);
		void fillExtensions (wxArrayString & extensionArray);
		void CheckMedia();

		wxBitmapButton * m_browseDiskA;
		wxBitmapButton * m_browseDiskB;
		wxBitmapButton * m_browseTape1;
		wxBitmapButton * m_browseTape2;
		wxBitmapButton * m_clearDiskA;
		wxBitmapButton * m_clearDiskB;
		wxBitmapButton * m_clearTape1;
		wxBitmapButton * m_clearTape2;

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

#endif // !defined(AFX_SESSIONPAGE_H__93FE8492_93A5_411A_B50C_22F14696B5FC__INCLUDED_)
