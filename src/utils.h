#ifndef UTILS_H
#define UTILS_H

#include <wx/string.h>
#include <wx/arrstr.h>
#include <cstdlib>

#ifdef __WXMSW__

#define VLA(TYPE, NAME, LENGTH) \
	auto NAME = static_cast<TYPE*>(_alloca(sizeof(TYPE) * (LENGTH)))

#else

#define VLA(TYPE, NAME, LENGTH) \
	TYPE NAME[(LENGTH)]

#endif

namespace utils {

wxString ConvertPath       (wxString path);
wxString ConvertPathNoSlash(wxString path);
wxString ConvertPathBack   (wxString path);

wxArrayString parseTclList(const wxString& str);

}

#endif
