// $Id: CatapultXMLParser.h,v 1.7 2004/04/10 21:24:05 h_oudejans Exp $
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
		void ParseXmlInput (wxString input, unsigned int ID);
		CatapultXMLParser(wxWindow * target);
		virtual ~CatapultXMLParser();

		enum ParseState {
			STATE_START, 
			TAG_OPENMSX, 
			TAG_REPLY,
			TAG_LOG,
			TAG_UPDATE,
			
		};		

		enum ReplyStatus {
			REPLY_UNKNOWN,
			REPLY_OK,
			REPLY_NOK
		};

		enum LogLevel {
			LOG_UNKNOWN,
			LOG_INFO,
			LOG_WARNING
		};

		enum UpdateType {
			UPDATE_UNKNOWN,
			UPDATE_LED,
			UPDATE_SETTING,
			UPDATE_PLUG,
			UPDATE_UNPLUG,
			UPDATE_MEDIA

		};

		struct ParseResult
		{
			ParseState parseState;
			unsigned unknownLevel;
			ReplyStatus replyState;
			LogLevel logLevel;
			UpdateType updateType;
			wxString contents;
			wxString name;
			unsigned int openMSXID;
		};

	private:
			unsigned int m_openMSXID;
			static wxWindow * m_target;
			static xmlSAXHandler handler;
			static xmlParserCtxt * context;
			static ParseResult parseResult;

			static void cb_start_element (CatapultXMLParser * parser,  const xmlChar * name, const xmlChar ** attrs);
			static void cb_end_element (CatapultXMLParser * parser,  const xmlChar * name);
			static void cb_text (CatapultXMLParser * parser, const xmlChar * chars, int len);
			static void SendParsedData ();
			void parseReply(const char** attrs);
			void parseLog(const char** attrs);
			void parseUpdate(const char** attrs);



};

#endif // !defined(AFX_CATAPULTXMLPARSER_H__B9E4A2C4_8E3B_492A_988F_83EA19370DEF__INCLUDED_)
