#include "Version.h"
#include "Version.ii"

const wxString Version::FULL_VERSION =
	wxT("openMSX Catapult ") + Version::VERSION +
	(Version::RELEASE ? wxString() : wxT("-") + Version::REVISION);
