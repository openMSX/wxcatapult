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
	/**
	 * Set enabledness for hard disk controls.
	 * @param forcedDisable Force disabling controls, instead of relying on
	 * internal parameter evaluation. should only be used in very special
	 * cases (see cpp file). This is a 'hack'.
	 */
	void SetHardDiskControl(bool forcedDisable = false);
	void EnableCassettePort(wxString data);
	void SetCassetteMode(wxString data);
	const wxArrayString& GetDetectedMachines() const;
	const wxArrayString& GetDetectedExtensions() const;
	void RestoreHistory();
	void FixLayout();
	wxString getStartupCommandLineOptions() const;

private:
	enum MediaType { DISKETTE, CARTRIDGE, CASSETTE, HARDDISK };
	struct MediaInfo {
		MediaInfo(wxMenu& menu_, const wxString& deviceName_,
		          const wxString& control_,
			  ConfigurationData::MediaBits bits,
			  ConfigurationData::ID id,
			  ConfigurationData::ID typeId_,
			  ConfigurationData::ID ipsId_,
			  wxButton* button_, MediaType mediaType_,
			  int ipsLabel_, int typeLabel_)
			: menu(menu_)
			, deviceName(deviceName_)
			, control(*(wxComboBox*)FindWindowByName(control_))
			, mediaBits(bits)
			, confId(id)
			, typeId(typeId_)
			, ipsId(ipsId_)
			, button(button_)
			, mediaType(mediaType_)
			, ipsLabel(ipsLabel_)
			, typeLabel(typeLabel_) {}
		wxMenu& menu;
		const wxString deviceName;
		wxComboBox& control;
		const ConfigurationData::MediaBits mediaBits;
		const ConfigurationData::ID confId;
		const ConfigurationData::ID typeId;
		const ConfigurationData::ID ipsId;
		wxButton* button;
		wxString ipsdir;
		wxArrayString ips;
		wxString type; // only for carts at the moment
		wxString lastContents;
		const MediaType mediaType;
		const int ipsLabel;
		const int typeLabel;
	};
	struct HistoryData : public wxClientData {
		HistoryData(const wxString& type_, const wxArrayString& ips_)
			: type(type_), ips(ips_) {}
		wxString type;
		wxArrayString ips;
	};

	virtual void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus);
	void checkLooseFocus(wxWindow* oldFocus, MediaInfo& media);
	void insertMedia(MediaInfo& m);
	void insertMediaClear(MediaInfo& m);
	void OnBrowseDiskA(wxCommandEvent& event);
	void OnBrowseDiskB(wxCommandEvent& event);
	void OnBrowseCartA(wxCommandEvent& event);
	void OnBrowseCartB(wxCommandEvent& event);
	void OnBrowseCassette(wxCommandEvent& event);
	void OnBrowseHardDisk(wxCommandEvent& event);
	void OnEjectDiskB(wxCommandEvent& event);
	void OnEjectDiskA(wxCommandEvent& event);
	void OnEjectCartA(wxCommandEvent& event);
	void OnEjectCartB(wxCommandEvent& event);
	void OnClearCassette(wxCommandEvent& event);
	void OnEjectHardDisk(wxCommandEvent& event);
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
	void OnClickHardDiskCombo(wxCommandEvent& event);
	void OnClickMediaMenu(wxCommandEvent& event);
	void OnInsertEmptyDiskByMenu(wxCommandEvent& event);
	void OnBrowseDiskByMenu(wxCommandEvent& event);
	void OnBrowseIps(wxCommandEvent& event);
	void OnBrowseDiskDirByMenu(wxCommandEvent& event);
	void OnBrowseCartByMenu(wxCommandEvent& event);
	void OnEjectByMenu(wxCommandEvent& event);
	void OnSelectMapper(wxCommandEvent& event);
	void OnMachineOrExtensionListChanged(wxCommandEvent& event);

	void SetupRomType(wxString romtype, wxString fullname);
	MediaInfo* GetLastMenuTarget() const;
	void GetRomTypes();
	void SetIpsList(MediaInfo& m, const wxArrayString& ips);
	void SetMapperType(MediaInfo& media, const wxString& type);
	void BrowseDisk(MediaInfo& media);
	void BrowseCart(MediaInfo& media);
	void BrowseMedia(MediaInfo& m, const wxString& path, const wxString title);
	void ClickMediaCombo(wxCommandEvent& event, MediaInfo& media);
	void prepareMachines  (const wxString& sharepath, bool optional = false);
	void prepareExtensions(const wxString& sharepath, bool optional = false);
	void AddHistory(MediaInfo& media);
	void SaveHistory();
	void EjectMedia(MediaInfo& media);
	wxArrayString getHardware() const; // TODO rewrite this

	wxComboBox* m_machineList;
	wxListBox* m_extensionList;

	enum { DISKA, DISKB, CARTA, CARTB, CAS, HDD, NUM_MEDIA };
	std::unique_ptr<MediaInfo> media[NUM_MEDIA];

	wxToggleButton* m_playButton;
	wxToggleButton* m_recordButton;
	wxButton* m_rewindButton;

	wxButton* m_diskAButton;
	wxButton* m_diskBButton;
	wxButton* m_cartAButton;
	wxButton* m_cartBButton;
	wxButton* m_cassetteButton;
	wxButton* m_hardDiskButton;

	wxBitmapButton* m_browseCassette;
	wxBitmapButton* m_clearCassette;
	bool m_hasCassettePort;
	
	wxBitmapButton* m_browseHardDisk;
	wxBitmapButton* m_clearHardDisk;

	wxStaticText* m_machineListLabel;
	wxStaticText* m_extensionListLabel;
	wxArrayString m_machineArray;
	wxArrayString m_extensionArray;

	openMSXController& m_controller;

	wxMenu* m_diskMenu[2];
	wxMenu* m_cartMenu[2];
	wxMenu* m_casMenu;
	wxMenu* m_hardDiskMenu;

	wxButton* m_lastUsedPopup;
	std::unique_ptr<RomTypeDlg> m_romTypeDialog;
	std::unique_ptr<IPSSelectionDlg> m_ipsDialog;
	bool m_cassetteControl;
	bool m_cassetteAutoCreate;

	DECLARE_CLASS(SessionPage)
	DECLARE_EVENT_TABLE()
};

#endif
