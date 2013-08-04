#include "Version.h"
#include "Version.ii"

const std::string Version::FULL_VERSION =
	"openMSX Catapult " + Version::VERSION +
	(Version::RELEASE ? "" : "-" + Version::REVISION);
