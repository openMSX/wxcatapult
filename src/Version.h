// $Id: Version.hh,v 1.2 2004/04/16 21:58:17 mthuurne Exp $

#ifndef __VERSION_HH__
#define __VERSION_HH__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class Version {
public:
	// Defined by build system:
	static const bool RELEASE;
	static const wxString VERSION;
	static const wxString CHANGELOG_REVISION;

	// Computed using constants above:
	static const wxString FULL_VERSION;
};

#endif //__VERSION_HH__
