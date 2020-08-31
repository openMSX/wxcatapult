#ifndef CATAPULTXMLPARSER_H
#define CATAPULTXMLPARSER_H

#include <libxml/parser.h>
#include <wx/string.h>

class wxWindow;

class CatapultXMLParser
{
public:
	CatapultXMLParser(wxWindow* target);
	void ParseXmlInput(wxString input, unsigned id);

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
		UPDATE_MEDIA,
		UPDATE_STATE,
		UPDATE_HARDWARE,
		UPDATE_EXTENSION,
		UPDATE_CONNECTOR
	};
	struct ParseResult {
		ParseState parseState;
		unsigned unknownLevel;
		ReplyStatus replyState;
		LogLevel logLevel;
		UpdateType updateType;
		wxString contents;
		wxString name;
		unsigned openMSXID;
	};

private:
	static wxWindow* m_target;
	static xmlSAXHandler handler;
	static xmlParserCtxt* context;
	static ParseResult parseResult;

	static void cb_start_element(CatapultXMLParser* parser, const xmlChar* name, const xmlChar** attrs);
	static void cb_end_element(CatapultXMLParser* parser, const xmlChar* name);
	static void cb_text(CatapultXMLParser* parser, const xmlChar* chars, int len);
	static void SendParsedData();
	void parseReply(const char** attrs);
	void parseLog(const char** attrs);
	void parseUpdate(const char** attrs);
};

#endif
