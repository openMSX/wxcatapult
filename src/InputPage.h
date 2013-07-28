#ifndef INPUTPAGE_H
#define INPUTPAGE_H

#include "CatapultPage.h"

class wxTextCtrl;
class wxButton;

class openMSXController;

class InputPage : public CatapultPage
{
public:
	InputPage(wxWindow* parent, openMSXController* controller);

	void SetControlsOnEnd();
	void SetControlsOnLaunch();
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) {};
	void OnTypeText(wxCommandEvent& event);
	void OnClearText(wxCommandEvent& event);
	void OnTextChange(wxCommandEvent& event);

	wxTextCtrl* m_inputtext;
private:
	openMSXController* m_controller;
	wxButton* m_typeTextButton;
	wxButton* m_clearTextButton;
	bool launched;

	DECLARE_CLASS(InputPage)
	DECLARE_EVENT_TABLE()
};

#endif
