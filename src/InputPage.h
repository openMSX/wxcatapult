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

private:
	void HandleFocusChange(wxWindow* oldFocus, wxWindow* newFocus) {}
	void OnTypeText(wxCommandEvent& event);
	void OnClearText(wxCommandEvent& event);
	void OnTextChange(wxCommandEvent& event);

	openMSXController* m_controller;
	wxTextCtrl* m_inputtext;
	wxButton* m_typeTextButton;
	wxButton* m_clearTextButton;
	bool launched;

	DECLARE_CLASS(InputPage)
	DECLARE_EVENT_TABLE()
};

#endif
