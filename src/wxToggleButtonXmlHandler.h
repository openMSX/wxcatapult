#ifndef WXTOGGLEBUTTONXMLHANDLER_H
#define WXTOGGLEBUTTONXMLHANDLER_H

#include "wx/xrc/xmlres.h"

class WXXMLDLLEXPORT wxToggleButtonXmlHandler : public wxXmlResourceHandler
{
public:
	bool CanHandle(wxXmlNode *node);
	wxObject * DoCreateResource();
	wxToggleButtonXmlHandler();
	virtual ~wxToggleButtonXmlHandler();
};

#endif
