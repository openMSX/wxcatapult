// $Id: Version.cpp,v 1.2 2005/01/06 16:27:22 h_oudejans Exp $

#include "Version.h"
#include "Version.ii"

const wxString Version::FULL_VERSION
	= wxT("openMSX Catapult ") + wxString((const wxChar *)Version::VERSION)
	+ ( Version::RELEASE ? wxT("") : wxT("-dev") +  wxString((const wxChar *)Version::CHANGELOG_REVISION) );
