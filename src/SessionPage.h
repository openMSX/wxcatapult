#ifndef SESSIONPAGE_H
#define SESSIONPAGE_H

#include "ConfigurationData.h"
#include "CatapultPage.h"
#include <memory>
#include <wx/dnd.h>

class RomTypeDlg;
class IPSSelectionDlg;
class openMSXController;
class wxCatapultFrame;
class wxComboBox;
class wxButton;
class wxBitmapButton;
class wxStaticText;
class wxToggleButton;
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
	SessionPage(wxWindow* parent, openMSXController& controller);

	void SetupHardware(bool initial, bool reset);
	void SetControlsOnLaunch();
	void SetControlsOnEnd();
	void SetCassetteControl();
	wxArrayString getMedia() const;
	wxArrayString getTypes() const;
	void getPatches(wxArrayString* parameters) const;
	wxArrayString getHardware() const;
	void UpdateSessionData();
	void EnableCassettePort(wxString data);
	void SetCassetteMode(wxString data);
	const wxArrayString& GetDetectedMachines() const;
	const wxArrayString& GetDetectedExtensions() const;
	void RestoreHistory();
	void FixLayout();

private:
	enum MediaType { DISKETTE, CARTRIDGE, CASSETTE };
	struct MediaInfo {
		MediaInfo(wxMenu& menu_, const wxString& deviceName_,
		          const wxString& control_,
			  ConfigurationData::MediaBits bits,
			  ConfigurationData::ID id,
			  wxButton* button_, MediaType mediaType_,
			  int ipsLabel_, int typeLabel_)
			: menu(menu_)
			, deviceName(deviceName_)
			, control(*(wxComboBox*)FindWindowByName(control_))
			, mediaBits(bits)
			, confId(id)
			, button(button_)
			, mediaType(mediaType_)
			, ipsLabel(ipsLabel_)
			, typeLabel(typeLabel_) {}
		wxMenu& menu;
		const wxString deviceName;
		wxComboBox& control;
		const ConfigurationData::MediaBits mediaBits;
		const ConfigurationData::ID confId;
		wxButton* button;
		wxString ipsdir;
		wxArrayString ips;
		wxString type; // only for carts at the moment
		wxArrayString typehistory;
		wxString lastContents;
		const MediaType mediaType;
		const int ipsLabel;
		const int typeLabel;
	};

	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus);
	void checkLooseFocus(wxWindow* oldFocus, MediaInfo& media);
	void insertMedia(MediaInfo& m);
	void OnBrowseCassette(wxCommandEvent& event);
	void OnBrowseDiskA(wxCommandEvent& event);
	void OnBrowseDiskB(wxCommandEvent& event);
	void OnBrowseCartA(wxCommandEvent& event);
	void OnBrowseCartB(wxCommandEvent& event);
	void OnClearCassette(wxCommandEvent& event);
	void OnEjectCartA(wxCommandEvent& event);
	void OnEjectCartB(wxCommandEvent& event);
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
	void OnClickMediaMenu(wxCommandEvent& event);
	void OnInsertEmptyDiskByMenu(wxCommandEvent& event);
	void OnBrowseDiskByMenu(wxCommandEvent& event);
	void OnBrowseDiskIps(wxCommandEvent& event);
	void OnBrowseDiskDirByMenu(wxCommandEvent& event);
	void OnBrowseCartByMenu(wxCommandEvent& event);
	void OnEjectByMenu(wxCommandEvent& event);
	void OnSelectMapper(wxCommandEvent& event);
	void OnSelectIPS(wxCommandEvent& event);

	void SetupRomType(wxString romtype, wxString fullname);
	MediaInfo* GetLastMenuTarget() const;
	void GetRomTypes();
	void SetMapperType(MediaInfo& media, const wxString& type);
	void BrowseDisk(MediaInfo& media);
	void BrowseCart(MediaInfo& media);
	void BrowseMedia(MediaInfo& m, const wxString& path, const wxString title);
	void ClickMediaCombo(wxCommandEvent& event, MediaInfo& media);
	void ChangeMediaContents(MediaInfo& media);
	void prepareMachines(const wxString& sharepath,
	                     wxArrayString& machineArray, bool optional = false);
	void fillMachines(const wxArrayString& machineArray);
	void prepareExtensions(const wxString& sharepath,
	                       wxArrayString& extensionArray, bool optional = false);
	void fillExtensions(const wxArrayString& extensionArray);
	void AddHistory(MediaInfo& media);
	void SaveHistory();
	void EjectMedia(MediaInfo& media);

	wxComboBox* m_machineList;
	wxListBox* m_extensionList;

	enum { DISKA, DISKB, CARTA, CARTB, CAS, NUM_MEDIA };
	std::unique_ptr<MediaInfo> media[NUM_MEDIA];

	wxToggleButton* m_playButton;
	wxToggleButton* m_recordButton;
	wxButton* m_rewindButton;

	wxButton* m_diskAButton;
	wxButton* m_diskBButton;
	wxButton* m_cartAButton;
	wxButton* m_cartBButton;
	wxButton* m_cassetteButton;

	wxBitmapButton* m_browseCassette;
	wxBitmapButton* m_clearCassette;
	bool m_hasCassettePort;

	wxStaticText* m_machineListLabel;
	wxStaticText* m_extensionListLabel;
	wxArrayString m_machineArray;
	wxArrayString m_extensionArray;

	int m_InsertedMedia;
	wxString m_usedMachine;
	wxString m_usedExtensions;
	openMSXController& m_controller;

	wxMenu* m_diskMenu[2];
	wxMenu* m_cartMenu[2];
	wxMenu* m_casMenu;

	wxButton* m_lastUsedPopup;
	std::unique_ptr<RomTypeDlg> m_romTypeDialog;
	std::unique_ptr<IPSSelectionDlg> m_ipsDialog;
	bool m_cassetteControl;
	bool m_cassetteAutoCreate;

	DECLARE_CLASS(SessionPage)
	DECLARE_EVENT_TABLE()
};

#endif
