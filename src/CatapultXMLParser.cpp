// $Id$
// CatapultXMLParser.cpp: implementation of the CatapultXMLParser class.
//
//////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wxCatapultApp.h"
#include "CatapultXMLParser.h"

CatapultXMLParser::ParseState CatapultXMLParser::user_data;
xmlSAXHandler CatapultXMLParser::handler;
xmlParserCtxt * CatapultXMLParser::context;
wxWindow * CatapultXMLParser::m_target;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CatapultXMLParser::CatapultXMLParser(wxWindow * target)
{
	m_target = target;
	user_data.state	= STATE_START;
	user_data.contents.Clear();
	memset (&handler,0,sizeof(handler));
	handler.startElement = (startElementSAXFunc) cb_start_element;
	handler.endElement = (endElementSAXFunc) cb_end_element;
	handler.characters = (charactersSAXFunc) cb_text;
#ifdef __UNIX__
	context = xmlCreatePushParserCtxt(&handler, &user_data, 0,0,0);
#endif
}

CatapultXMLParser::~CatapultXMLParser()
{
}

void CatapultXMLParser::cb_start_element(ParseState *user_data, const xmlChar *name, const xmlChar **attrs)
{
	switch (user_data->state)
	{
		case STATE_START:
			if (!strcmp((const char *)name, _("openmsx-output")))
				user_data->state = TAG_OPENMSX;
			break;
		case TAG_OPENMSX:
			if (!strcmp((const char *)name, _("info")))
				user_data->state = TAG_INFO;

			if (!strcmp((const char *)name, _("ok")))
				user_data->state = TAG_OK;

			if (!strcmp((const char *)name, _("nok")))
				user_data->state = TAG_NOK;

			if (!strcmp((const char *)name, _("warning")))
				user_data->state = TAG_WARNING;

			if (!strcmp((const char *)name, _("update")))
				user_data->state = TAG_UPDATE;

			if (!strcmp((const char *)name, _("error")))
				user_data->state = TAG_ERROR;

			break;
		default:
			break;
	}
	user_data->contents.Clear();
}

void CatapultXMLParser::cb_end_element (ParseState * user_data,  const xmlChar * name)
{
	switch (user_data->state)
	{
		case TAG_OPENMSX:		
			user_data->state = STATE_START;
			break;
		case TAG_INFO:
		case TAG_OK:
		case TAG_NOK:
		case TAG_WARNING:
		case TAG_UPDATE:
		case TAG_ERROR:
			SendParsedData (*user_data);
			user_data->state = TAG_OPENMSX;
			break;
	}
}

void CatapultXMLParser::cb_text (ParseState * user_data, const xmlChar * chars, int len)
{
	wxString temp;
	switch (user_data->state)
	{
		case TAG_INFO:
		case TAG_OK:
		case TAG_NOK:
		case TAG_WARNING:
		case TAG_UPDATE:
		case TAG_ERROR:
			temp = (const char *) chars;
			user_data->contents.Append (temp.Left(len));
			break;
		default:
			break;
	}
}

void CatapultXMLParser::SendParsedData(ParseState data)
{
	wxCommandEvent parseEvent(EVT_CONTROLLER);
	ParseState * result = new ParseState;
	result->contents = data.contents;
	result->state = data.state;
	parseEvent.SetClientData ((void *)result);
	parseEvent.SetId(MSGID_PARSED);
	wxPostEvent (m_target, parseEvent);
}

void CatapultXMLParser::ParseXmlInput(wxString input)
{
#ifdef __UNIX__
	xmlParseChunk (context, input.c_str(), input.Len(),0);
#else
	xmlFakeParse (input);
#endif
}

#ifdef __WINDOWS__

void CatapultXMLParser::xmlFakeParse(wxString msg)
{
	int pos,pos2;
	wxString value = msg;
	wxString tag;
	wxString current;
	do
	{
		pos = value.Find(_("<"));
		pos2= value.Find(_(">"));
		if ((pos < pos2) && ((pos2-pos) >0)){
			tag = value.Mid(pos+1,pos2-pos-1);
			pos = value.Find (wxString(_("</") + tag + _(">")));
			if (pos != -1)
			{
				current = value.Left(pos + tag.Len() +3);
				value = value.Mid(pos + tag.Len() +4);	
				xmlFakeProcessLine (current);
			}
		}
	} while (pos !=-1);
	if (!value.IsEmpty()) // not everything parsed ?
		xmlFakeProcessLine (value);
}

void CatapultXMLParser::xmlFakeProcessLine (wxString line)
{
	wxCommandEvent parseEvent(EVT_CONTROLLER);
	ParseState * result = new ParseState;
	if (line.Find(_("<ok>")) != -1) result->state = TAG_OK;
	if (line.Find(_("<nok>")) != -1) result->state = TAG_NOK;
	if (line.Find(_("<info>")) != -1) result->state =TAG_INFO;
	if (line.Find(_("<warning>")) != -1) result->state = TAG_WARNING;
	if (line.Find(_("<update>")) != -1) result->state = TAG_UPDATE;
	if (line.Find(_("<error>")) != -1) result->state = TAG_ERROR;

	int start = line.Find('>');
	int stop = line.Find('<',true);
	if ((start != -1) && (stop != -1) && (start<stop))
	{
		result->contents = line.Mid(start+1, stop-start-1);
		parseEvent.SetClientData ((void *)result);
		parseEvent.SetId(MSGID_PARSED);
		wxPostEvent (m_target, parseEvent);
	}
}

#endif
