#include "utils.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>

using namespace std;

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


static vector<string> parseTclList(const string& str)
{
	vector<string> result;
	for (auto it = str.begin(); it != str.end(); /**/) {
		auto ch1 = *it++;
		if (ch1 == ' ') continue; // skip space between items

		string item;
		switch (ch1) {
		case '"':
			while (true) {
				if (it == str.end()) {
					throw runtime_error("unterminated open-quote");
				}
				char ch = *it++;
				if (ch == '"') {
					break;
				} else if (ch == '\\') {
					if (it == str.end()) {
						throw runtime_error("backslash at end of string");
					}
					ch = *it++;
				}
				item += ch;
			}
			if ((it != str.end()) && (*it != ' ')) {
				throw runtime_error("extra characters after close-quote");
			}
			break;
		case '{': {
			int braceLevel = 1;
			while (true) {
				if (it == str.end()) {
					throw runtime_error("unterminated open-brace");
				}
				char ch = *it++;
				if (ch == '}') {
					if (--braceLevel == 0) break;
				} else if (ch == '{') {
					++braceLevel;
				} else if (ch == '\\') {
					item += ch; // keep backslash
					if (it == str.end()) {
						throw runtime_error("backslash at end of string");
					}
					ch = *it++;
				}
				item += ch;
			}
			if ((it != str.end()) && (*it != ' ')) {
				throw runtime_error("extra characters after close-brace");
			}
			break;
		}
		default:
			--it; // also add first char (this might be \)
			while (it != str.end()) {
				char ch = *it++;
				if (ch == ' ') {
					break;
				} else if (ch == '\\') {
					if (it == str.end()) {
						throw runtime_error("backslash at end of string");
					}
					ch = *it++;
				}
				item += ch;
			}
			break;
		}
		result.push_back(item);
	}
	return result;
}

wxArrayString parseTclList(const wxString& str)
{
	// TODO the input was likely already converted from utf8->wxString
	//      and here we convert it back to utf8
	wxArrayString result;
	try {
		string str2 = static_cast<const char*>(str.mb_str(wxConvUTF8));
		for (auto& item : parseTclList(str2)) {
			result.Add(wxString(item.data(), wxConvUTF8, item.size()));
		}
	} catch (std::exception&) {
		assert(false);
	}
	return result;
}

wxString tclEscapeWord(wxString str)
{
	str.Replace(wxT("\\"), wxT("\\\\"));
	str.Replace(wxT("\n"), wxT("\\r"));
	str.Replace(wxT("$"),  wxT("\\$"));
	str.Replace(wxT("\""), wxT("\\\""));
	str.Replace(wxT("["),  wxT("\\["));
	str.Replace(wxT("]"),  wxT("\\]"));
	str.Replace(wxT("}"),  wxT("\\}"));
	str.Replace(wxT("{"),  wxT("\\{"));
	str.Replace(wxT(" "),  wxT("\\ "));
	str.Replace(wxT(";"),  wxT("\\;"));
	return str;
}

}
