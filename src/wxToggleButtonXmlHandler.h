// $Id$
// wxToggleButtonXmlHandler.h: interface for the wxToggleButtonXmlHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WXTOGGLEBUTTONXMLHANDLER_H__48F021F4_EAB7_45B8_ACD6_599C08123D79__INCLUDED_)
#define AFX_WXTOGGLEBUTTONXMLHANDLER_H__48F021F4_EAB7_45B8_ACD6_599C08123D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "wx/xrc/xmlres.h"

class WXXMLDLLEXPORT wxToggleButtonXmlHandler : public wxXmlResourceHandler  
{
	public:
		bool CanHandle(wxXmlNode *node);
		wxObject * DoCreateResource();
		wxToggleButtonXmlHandler();
		virtual ~wxToggleButtonXmlHandler();

};

#endif // !defined(AFX_WXTOGGLEBUTTONXMLHANDLER_H__48F021F4_EAB7_45B8_ACD6_599C08123D79__INCLUDED_)
