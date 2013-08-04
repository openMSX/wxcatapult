#ifndef VERSION_HH
#define VERSION_HH

#include <string>

class Version {
public:
	// Defined by build system:
	static const bool RELEASE;
	static const std::string VERSION;
	static const std::string REVISION;

	// Computed using constants above:
	static const std::string FULL_VERSION;
};

#endif
