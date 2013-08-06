#include "utils.h"

namespace utils {

wxString ConvertPath(wxString path)
{
	path.Prepend(wxT("\""));
	path.Append(wxT("\""));
	path.Replace(wxT("\\"), wxT("/"));
	return path;
}

wxString ConvertPathNoSlash(wxString path)
{
	path.Prepend(wxT("\""));
	path.Append(wxT("\""));
	return path;
}

wxString ConvertPathBack(wxString path)
{
	path.Replace(wxT("\""), wxT(""));
	path.Replace(wxT("/"), wxT("\\"));
	return path;
}

}
