// $Id: Version.cc,v 1.1 2004/04/12 09:48:18 mthuurne Exp $

#include "Version.h"
#include "Version.ii"

const wxString Version::FULL_VERSION
	= "openMSX Catapult " + Version::VERSION
	+ ( Version::RELEASE ? "" : "-dev" + Version::CHANGELOG_REVISION );

