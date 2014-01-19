#include "wxCatapultApp.h"
#include "CatapultXMLParser.h"
#include <cassert>
#include <wx/event.h>
#include <wx/window.h>
#include <wx/wxprec.h>

CatapultXMLParser::ParseResult CatapultXMLParser::parseResult;
xmlSAXHandler CatapultXMLParser::handler;
xmlParserCtxt* CatapultXMLParser::context;
wxWindow* CatapultXMLParser::m_target;

CatapultXMLParser::CatapultXMLParser(wxWindow* target)
{
	m_target = target;
	parseResult.parseState	= STATE_START;
	parseResult.unknownLevel = 0;
	parseResult.contents.Clear();
	memset(&handler, 0, sizeof(handler));
	handler.startElement = (startElementSAXFunc)cb_start_element;
	handler.endElement = (endElementSAXFunc)cb_end_element;
	handler.characters = (charactersSAXFunc)cb_text;
	context = xmlCreatePushParserCtxt(&handler, this, 0, 0, 0);
}

void CatapultXMLParser::cb_start_element(
	CatapultXMLParser* parser, const xmlChar* name, const xmlChar** attrs)
{
	if (parser->parseResult.unknownLevel) {
		++(parser->parseResult.unknownLevel);
		return;
	}
	switch (parser->parseResult.parseState) {
	case STATE_START:
		if (!strcmp((const char*)name, "openmsx-output")) {
			parser->parseResult.parseState = TAG_OPENMSX;
		} else {
			++(parser->parseResult.unknownLevel);
		}
		break;
	case TAG_OPENMSX:
		if (!strcmp((const char*)name, "reply")) {
			parser->parseResult.parseState = TAG_REPLY;
			parser->parseReply((const char**)attrs);
		} else if (!strcmp((const char*)name, "log")) {
			parser->parseResult.parseState = TAG_LOG;
			parser->parseLog((const char**)attrs);
		} else if (!strcmp((const char*)name, "update")) {
			parser->parseResult.parseState = TAG_UPDATE;
			parser->parseUpdate((const char**)attrs);
		} else  {
			++(parser->parseResult.unknownLevel);
		}
		break;
	default:
		++(parser->parseResult.unknownLevel);
		break;
	}
	parser->parseResult.contents.Clear();
}

void CatapultXMLParser::cb_end_element(CatapultXMLParser* parser, const xmlChar* name)
{
	if (parser->parseResult.unknownLevel) {
		--(parser->parseResult.unknownLevel);
		return;
	}
	switch (parser->parseResult.parseState) {
	case STATE_START:
		assert(false);
		break;
	case TAG_OPENMSX:
		parser->parseResult.parseState = STATE_START;
		break;
	case TAG_UPDATE:
	case TAG_LOG:
	case TAG_REPLY:
		SendParsedData();
		parser->parseResult.parseState = TAG_OPENMSX;
		break;
	}
}

void CatapultXMLParser::cb_text(CatapultXMLParser* parser, const xmlChar* chars, int len)
{
	assert(len >= 0);
	switch (parser->parseResult.parseState) {
	case TAG_UPDATE:
	case TAG_LOG:
	case TAG_REPLY:
		parser->parseResult.contents.Append(wxString(
			(const char*)chars, wxConvUTF8, (size_t)len));
		break;
	default:
		break;
	}
}

void CatapultXMLParser::SendParsedData()
{
	wxCommandEvent parseEvent(EVT_CONTROLLER);
	auto* result = new ParseResult;
	result->contents = parseResult.contents;
	result->openMSXID = parseResult.openMSXID;
	// handle both new and deprecated messages
	switch (parseResult.parseState) {
	case STATE_START:
	case TAG_OPENMSX:
		assert(false);
		break;
	case TAG_LOG:
		result->parseState = TAG_LOG;
		result->logLevel = parseResult.logLevel;
		break;
	case TAG_REPLY:
		result->parseState = TAG_REPLY;
		result->replyState = parseResult.replyState;
		break;
	case TAG_UPDATE:
		result->parseState = TAG_UPDATE;
		result->name = parseResult.name;
		result->updateType = parseResult.updateType;
		break;
	}

	parseEvent.SetClientData(result);
	parseEvent.SetId(MSGID_PARSED);
	wxPostEvent(m_target, parseEvent);
}

void CatapultXMLParser::ParseXmlInput(wxString input, unsigned id)
{
	parseResult.openMSXID = id;
	xmlParseChunk(context, (const char*)(input.mb_str(wxConvUTF8)), input.Len(), 0);
}

void CatapultXMLParser::parseReply(const char** attrs)
{
	parseResult.replyState = REPLY_UNKNOWN;
	if (!attrs) return;
	for (/**/; *attrs; attrs += 2) {
		if (strcmp(attrs[0], "result") == 0) {
			if (strcmp(attrs[1], "ok") == 0) {
				parseResult.replyState = REPLY_OK;
			} else if (strcmp(attrs[1], "nok") == 0) {
				parseResult.replyState = REPLY_NOK;
			}
		}
	}
}

void CatapultXMLParser::parseLog(const char** attrs)
{
	parseResult.logLevel = LOG_UNKNOWN;
	if (!attrs) return;
	for (/**/; *attrs; attrs += 2) {
		if (strcmp(attrs[0], "level") == 0) {
			if (strcmp(attrs[1], "info") == 0) {
				parseResult.logLevel = LOG_INFO;
			} else if (strcmp(attrs[1], "warning") == 0) {
				parseResult.logLevel = LOG_WARNING;
			}
		}
	}
}

void CatapultXMLParser::parseUpdate(const char** attrs)
{
	parseResult.updateType = UPDATE_UNKNOWN;
	if (!attrs) return;
	for (/**/; *attrs; attrs += 2) {
		if (strcmp(attrs[0], "type") == 0) {
			if (strcmp(attrs[1], "led") == 0) {
				parseResult.updateType = UPDATE_LED;
			} else if (strcmp(attrs[1], "setting") == 0) {
				parseResult.updateType = UPDATE_SETTING;
			} else if (strcmp(attrs[1], "plug") == 0) {
				parseResult.updateType = UPDATE_PLUG;
			} else if (strcmp(attrs[1], "unplug") == 0) {
				parseResult.updateType = UPDATE_UNPLUG;
			} else if (strcmp(attrs[1], "media") == 0) {
				parseResult.updateType = UPDATE_MEDIA;
			} else if (strcmp(attrs[1], "status") == 0){
				parseResult.updateType = UPDATE_STATE;
			}
		} else if (strcmp(attrs[0], "name") == 0) {
			parseResult.name = wxString(attrs[1], wxConvUTF8);
		}
	}
}
