// $Id: Version.cpp,v 1.1 2004/05/08 20:50:44 mthuurne Exp $

#include "Version.h"
#include "Version.ii"

const wxString Version::FULL_VERSION
	= wxT("openMSX Catapult ") + wxString((const wxChar *)Version::VERSION)
	+ ( Version::RELEASE ? wxT("") : wxT("-dev") +  wxString((const wxChar *)Version::CHANGELOG_REVISION) );

