// CatapultXMLParser.h: interface for the CatapultXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CATAPULTXMLPARSER_H__B9E4A2C4_8E3B_492A_988F_83EA19370DEF__INCLUDED_)
#define AFX_CATAPULTXMLPARSER_H__B9E4A2C4_8E3B_492A_988F_83EA19370DEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <libxml/parser.h>

class CatapultXMLParser  
{
	public:
		void ParseXmlInput (wxString input);
		CatapultXMLParser(wxWindow * target);
		virtual ~CatapultXMLParser();

		enum State {STATE_START, 
			TAG_OPENMSX, 
			TAG_OK, 
			TAG_NOK, 
			TAG_INFO, 
			TAG_WARNING, 
			TAG_UPDATE,
			TAG_ERROR};

			struct ParseState
			{
				State state;
				wxString contents;
			};

	private:

#ifdef __WINDOWS__		
			void xmlFakeParse (wxString msg);
			void xmlFakeProcessLine (wxString line);
#endif
			static wxWindow * m_target;
			static ParseState user_data;
			static xmlSAXHandler handler;
			static xmlParserCtxt * context;


			static void cb_start_element (ParseState * user_data,  const xmlChar * name, const xmlChar ** attrs);
			static void cb_end_element (ParseState * user_data,  const xmlChar * name);
			static void cb_text (ParseState * user_data, const xmlChar * chars, int len);
			static void SendParsedData (ParseState data);
};

#endif // !defined(AFX_CATAPULTXMLPARSER_H__B9E4A2C4_8E3B_492A_988F_83EA19370DEF__INCLUDED_)
