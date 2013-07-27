#ifndef VERSION_HH
#define VERSION_HH

#include <wx/string.h>

class Version {
public:
	// Defined by build system:
	static const bool RELEASE;
	static const wxString VERSION;
	static const wxString CHANGELOG_REVISION;

	// Computed using constants above:
	static const wxString FULL_VERSION;
};

#endif
