// $Id: CatapultXMLParser.cpp,v 1.2 2004/02/04 22:01:04 manuelbi Exp $
// CatapultXMLParser.cpp: implementation of the CatapultXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wxCatapultApp.h"
#include "CatapultXMLParser.h"

CatapultXMLParser::ParseResult CatapultXMLParser::parseResult;
xmlSAXHandler CatapultXMLParser::handler;
xmlParserCtxt * CatapultXMLParser::context;
wxWindow * CatapultXMLParser::m_target;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CatapultXMLParser::CatapultXMLParser(wxWindow * target)
{
	m_target = target;
	parseResult.parseState	= STATE_START;
	parseResult.contents.Clear();
	memset (&handler,0,sizeof(handler));
	handler.startElement = (startElementSAXFunc) cb_start_element;
	handler.endElement = (endElementSAXFunc) cb_end_element;
	handler.characters = (charactersSAXFunc) cb_text;
	context = xmlCreatePushParserCtxt(&handler, this, 0,0,0);
}

CatapultXMLParser::~CatapultXMLParser()
{
}

void CatapultXMLParser::cb_start_element(CatapultXMLParser * parser, const xmlChar *name, const xmlChar **attrs)
{
	switch (parser->parseResult.parseState)
	{
		case STATE_START:
			if (!strcmp((const char *)name, _("openmsx-output")))
				parser->parseResult.parseState = TAG_OPENMSX;
			break;
		case TAG_OPENMSX:
			if (!strcmp((const char *)name, _("reply"))) {
				parser->parseResult.parseState = TAG_REPLY;			
				parser->parseReply((const char**)attrs);
			}
			
			if (!strcmp((const char *)name, _("log"))) {
				parser->parseResult.parseState = TAG_LOG;
				parser->parseLog((const char**)attrs);
			}
			
			if (!strcmp((const char *)name, _("update"))) {
				parser->parseResult.parseState = TAG_UPDATE;
				parser->parseUpdate((const char**)attrs);
			}

// about to be depreciated tags

			if (!strcmp((const char *)name, _("info"))) {
				parser->parseResult.parseState = TAG_INFO;
				parser->parseResult.logLevel = LOG_INFO;
			}

			if (!strcmp((const char *)name, _("ok"))) {
				parser->parseResult.parseState = TAG_OK;
				parser->parseResult.replyState = REPLY_OK;
			}

			if (!strcmp((const char *)name, _("nok"))) {
				parser->parseResult.parseState = TAG_NOK;
				parser->parseResult.replyState = REPLY_NOK;
			}

			if (!strcmp((const char *)name, _("warning"))) {
				parser->parseResult.parseState = TAG_WARNING;
				parser->parseResult.logLevel = LOG_WARNING;
			}
			break;
		default:
			break;
	}
	parser->parseResult.contents.Clear();
}

void CatapultXMLParser::cb_end_element (CatapultXMLParser * parser,  const xmlChar * name)
{
	switch (parser->parseResult.parseState)
	{
		case TAG_OPENMSX:		
			parser->parseResult.parseState = STATE_START;
			break;
		case TAG_INFO:
		case TAG_OK:
		case TAG_NOK:
		case TAG_WARNING:
		case TAG_UPDATE:
		case TAG_LOG:
		case TAG_REPLY:
			SendParsedData ();
			parser->parseResult.parseState = TAG_OPENMSX;
			break;
	}
}

void CatapultXMLParser::cb_text (CatapultXMLParser * parser, const xmlChar * chars, int len)
{
	wxString temp;
	switch (parser->parseResult.parseState)
	{
		case TAG_INFO:
		case TAG_OK:
		case TAG_NOK:
		case TAG_WARNING:
		case TAG_UPDATE:
		case TAG_LOG:
		case TAG_REPLY:
			temp = (const char *) chars;
			parser->parseResult.contents.Append (temp.Left(len));
			break;
		default:
			break;
	}
}

void CatapultXMLParser::SendParsedData()
{
	wxCommandEvent parseEvent(EVT_CONTROLLER);
	ParseResult * result = new ParseResult;
	result->contents = parseResult.contents;

	// handle both new and depreciated messages
	switch (parseResult.parseState)	{
	case TAG_INFO:
	case TAG_WARNING:
	case TAG_LOG:
		result->parseState = TAG_LOG;
		result->logLevel = parseResult.logLevel;
		break;
	case TAG_OK:
	case TAG_NOK:
	case TAG_REPLY:
		result->parseState = TAG_REPLY;
		result->replyState = parseResult.replyState;
		break;
	case TAG_UPDATE:
		result->parseState = TAG_UPDATE;
		if (parseResult.updateType == UPDATE_OLD) {
			if (parseResult.contents.Find(_(" LED O")) != -1) {
				result->name = parseResult.contents.Left(parseResult.contents.Find(' ')).Lower();
				result->contents = parseResult.contents.Right(parseResult.contents.Len() - parseResult.contents.Find(' ',true)-1).Lower();
				result->updateType = UPDATE_LED;
			}
			else result->updateType = UPDATE_UNKNOWN;
		}
		else {
			result->name = parseResult.name.Lower();
			result->updateType = parseResult.updateType;
		}
		break;
	}
	
	parseEvent.SetClientData ((void *)result);
	parseEvent.SetId(MSGID_PARSED);
	wxPostEvent (m_target, parseEvent);
}

void CatapultXMLParser::ParseXmlInput(wxString input)
{
	xmlParseChunk (context, input.c_str(), input.Len(),0);
}

void CatapultXMLParser::parseReply(const char** attrs)
{
	parseResult.replyState = REPLY_UNKNOWN;
	if (attrs) {
		for ( ; *attrs; attrs += 2) {
			if (strcmp(attrs[0], "result") == 0) {
				if (strcmp(attrs[1], "ok") == 0) {
					parseResult.replyState = REPLY_OK;
				} else if (strcmp(attrs[1], "nok") == 0) {
					parseResult.replyState = REPLY_NOK;
				} 
			}
		}
	}
}

void CatapultXMLParser::parseLog(const char** attrs)
{
	parseResult.logLevel = LOG_UNKNOWN;
	if (attrs) {
		for ( ; *attrs; attrs += 2) {
			if (strcmp(attrs[0], "level") == 0) {
				if (strcmp(attrs[1], "info") == 0) {
					parseResult.logLevel = LOG_INFO;
				} else if (strcmp(attrs[1], "warning") == 0) {
					parseResult.logLevel = LOG_WARNING;
				}
			}
		}
	}
} 

void CatapultXMLParser::parseUpdate(const char** attrs)
{
	parseResult.updateType = UPDATE_UNKNOWN;
	if (attrs) {
		for ( ; *attrs; attrs += 2) {
			if (strcmp(attrs[0], "type") == 0) {
				if (strcmp(attrs[1], "led") == 0) {
					parseResult.updateType = UPDATE_LED;
				}
			} else if (strcmp(attrs[0], "name") == 0) {
				parseResult.name = attrs[1];
			}
		}
	}
	else {
		parseResult.updateType = UPDATE_OLD;
	}
} 