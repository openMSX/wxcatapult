// $id: $
// CatapultPage.h: interface for the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CATAPULTPAGE_H
#define CATAPULTPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CatapultPage : public wxPanel 
{
public:
	CatapultPage();
	virtual ~CatapultPage();
	
	virtual void HandleFocusChange(wxWindow * oldFocus, wxWindow * newFocus)=0;
protected:
	wxString ConvertPath (wxString path, bool ConvertSlash=false);
};

#endif 
