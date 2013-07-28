#include "Version.h"
#include "Version.ii"

const wxString Version::FULL_VERSION
	= wxT("openMSX Catapult ") + wxString((const wxChar*)Version::VERSION)
	+ (Version::RELEASE ? wxT("") : wxT("-") + wxString((const wxChar*)Version::REVISION));
