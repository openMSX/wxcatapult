#ifndef SESSIONPAGE_H
#define SESSIONPAGE_H

#include "CatapultPage.h"
#include "wx/dnd.h"

class RomTypeDlg;
class IPSSelectionDlg;
class openMSXController;
class wxCatapultFrame;
class wxComboBox;
class wxButton;
class wxBitmapButton;
class wxStaticText;
class wxListBox;

class SessionDropTarget : public wxFileDropTarget
{
public:
	SessionDropTarget(wxComboBox* target);
	bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

private:
	wxComboBox* m_target;
};


class SessionPage : public CatapultPage
{
public:
	SessionPage(wxWindow* parent = nullptr, openMSXController* controller = nullptr);
	virtual ~SessionPage();
//	void OnClickCombo(wxCommandEvent& event);
	void OnBrowseCassette(wxCommandEvent& event);
	void OnBrowseDiskA(wxCommandEvent& event);
	void OnBrowseDiskB(wxCommandEvent& event);
	void OnBrowseCartA(wxCommandEvent& event);
	void OnBrowseCartB(wxCommandEvent& event);
	void OnClearCassette(wxCommandEvent& event);
	void OnEjectCartA(wxCommandEvent& event);
	void OnEjectCartB(wxCommandEvent& event);
	void OnEjectCartByMenu(wxCommandEvent& event);
	void OnEjectDiskB(wxCommandEvent& event);
	void OnEjectDiskA(wxCommandEvent& event);
	void OnRewind(wxCommandEvent& event);
	void OnMotorControl(wxCommandEvent& event);
	void OnModePlay(wxCommandEvent& event);
	void OnModeRecord(wxCommandEvent& event);
	void OnAutoCassettefile(wxCommandEvent& event);
	void OnClickDiskACombo(wxCommandEvent& event);
	void OnClickDiskBCombo(wxCommandEvent& event);
	void OnClickCartACombo(wxCommandEvent& event);
	void OnClickCartBCombo(wxCommandEvent& event);
	void OnClickCassetteCombo(wxCommandEvent& event);
	void OnChangeDiskAContents(wxCommandEvent& event);
	void OnChangeDiskBContents(wxCommandEvent& event);
	void OnChangeCartAContents(wxCommandEvent& event);
	void OnChangeCartBContents(wxCommandEvent& event);
	void OnChangeCassetteContents(wxCommandEvent& event);
	void HandleCassetteChange();
	void OnClickDiskMenu(wxCommandEvent& event);
	void OnClickCartMenu(wxCommandEvent& event);
	void OnClickCasMenu(wxCommandEvent& event);
	void OnInsertEmptyDiskByMenu(wxCommandEvent& event);
	void OnBrowseDiskByMenu(wxCommandEvent& event);
	void OnBrowseDiskIps(wxCommandEvent& event);
	void OnBrowseDiskDirByMenu(wxCommandEvent& event);
	void OnBrowseCartByMenu(wxCommandEvent& event);
	void OnEjectDiskByMenu(wxCommandEvent& event);
	void OnSelectMapper(wxCommandEvent& event);
	void OnSelectIPS(wxCommandEvent& event);
	void SetupHardware(bool initial, bool reset);
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus);
	void SetControlsOnLaunch();
	void SetControlsOnEnd();
	void SetCassetteControl();
	void getMedia(wxArrayString& parameters);
	void getTypes(wxArrayString& parameters);
	void getPatches(wxArrayString* parameters);
	void getHardware(wxArrayString& hardware);
	void UpdateSessionData();
	void EnableCassettePort(wxString data);
	void SetCassetteMode(wxString data);
	void AutoPlugCassette();
	void AddRomType(wxString romtype);
	void SetRomTypeFullName(wxString romtype, wxString fullname);
	void SetupRomType(wxString romtype, wxString fullname);
	void SetOldStyleCassetteControls();
	wxArrayString& GetDetectedMachines();
	wxArrayString& GetDetectedExtensions();
	void RestoreHistory();
	void FixLayout();

private:
	struct mediaInfo {
		mediaInfo(wxMenu* menu)
		{
			contents = wxT("");
			ips.Clear();
			type = wxT("");
			control = NULL;
			history.Clear();
			typehistory.Clear();
			mmenu = menu;
			ipsdir = wxT("");
			avoid_evt = false;
			lastContents = wxT("");
			deviceName = wxT("");
		};
		wxString contents;
		wxString ipsdir;
		wxArrayString ips;
		wxString type; // only for carts at the moment
		wxComboBox* control;
		wxArrayString history;
		wxArrayString typehistory;
		wxMenu* mmenu;
		bool avoid_evt;
		wxString oldContents;
		wxString lastContents;
		wxString deviceName;
	};
	mediaInfo* GetLastMenuTarget();
	static int CompareCaseInsensitive(const wxString& first, const wxString& second);
	void GetRomTypes ();
	void UpdateMenuMapperLabel(mediaInfo* target);
	wxString ConvertRomType(wxString source, bool backwards);
	bool BrowseDisk(mediaInfo* target, wxString devicename, wxString defaultpath);
	void BrowseCart(mediaInfo* target, wxString defaultpath);
	void prepareMachines(wxString sharepath, wxArrayString& machineArray, bool optional = false);
	void fillMachines(wxArrayString& machineArray);
	void prepareExtensions(wxString sharepath, wxArrayString& extensionArray, bool optional = false);
	void fillExtensions(wxArrayString & extensionArray);
	void AddHistory(mediaInfo* media);
	void SaveHistory();
	void EjectCart(mediaInfo* target);
	void EjectDisk(mediaInfo* target);

	mediaInfo* m_diskA;
	mediaInfo* m_diskB;
	mediaInfo* m_cartA;
	mediaInfo* m_cartB;
	mediaInfo* m_cassette;

	wxToggleButton* m_playButton;
	wxToggleButton* m_recordButton;
	wxButton* m_rewindButton;

	wxButton* m_diskAButton;
	wxButton* m_diskBButton;
	wxButton* m_cartAButton;
	wxButton* m_cartBButton;
	wxButton* m_cassetteButton;

	wxBitmapButton* m_browseCartA;
	wxBitmapButton* m_browseCartB;
	wxBitmapButton* m_clearCartA;
	wxBitmapButton* m_clearCartB;

	wxBitmapButton* m_browseDiskA;
	wxBitmapButton* m_browseDiskB;
	wxBitmapButton* m_browseCassette;
	wxBitmapButton* m_clearDiskA;
	wxBitmapButton* m_clearDiskB;
	wxBitmapButton* m_clearCassette;
	wxString m_cassettePortState;

	wxStaticText* m_machineListLabel;
	wxStaticText* m_extensionListLabel;
	wxArrayString m_machineArray;
	wxArrayString m_extensionArray;

	int m_InsertedMedia;
	wxString m_usedMachine;
	wxString m_usedExtensions;
	openMSXController * m_controller;
	wxCatapultFrame * m_parent;

	wxMenu* m_diskMenu[2];
	wxMenu* m_cartMenu[2];
	wxMenu* m_casMenu;

	wxButton* m_lastUsedPopup;
	RomTypeDlg* m_romTypeDialog;
	IPSSelectionDlg* m_ipsDialog;
	bool m_cassetteControl;
	bool m_cassetteControlEnabled;
	bool m_cassetteAutoCreate;

	// old style compatability
	wxString m_casInsertCommand;
	wxString m_motorControlOnCommand;
	wxString m_motorControlOffCommand;

	DECLARE_CLASS(SessionPage)
	DECLARE_EVENT_TABLE()

public:
	wxComboBox* m_machineList;
	wxListBox* m_extensionList;
};

#endif
