// $id: $
// CatapultPage.cpp: implementation of the CatapultPage class.
//
//////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#include "wx/xrc/xmlres.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "CatapultPage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CatapultPage::CatapultPage()
{

}

CatapultPage::~CatapultPage()
{

}

wxString CatapultPage::ConvertPath(wxString path, bool ConvertSlash)
{
	path.Prepend(_("\""));
	path.Append(_("\""));
	if (ConvertSlash)
		path.Replace(_("\\"),_("/"),true);
	return path;
}

