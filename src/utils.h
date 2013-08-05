#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>

#ifdef __WXMSW__

#define VLA(TYPE, NAME, LENGTH) \
	auto NAME = static_cast<TYPE*>(_alloca(sizeof(TYPE) * (LENGTH)))

#else

#define VLA(TYPE, NAME, LENGTH) \
	TYPE NAME[(LENGTH)]

#endif

#endif
