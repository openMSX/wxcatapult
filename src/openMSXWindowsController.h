// $Id: openMSXWindowsController.h,v 1.3 2004/03/31 14:49:51 h_oudejans Exp $
// openMSXWindowsController.h: interface for the openMSXWindowsController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENMSXWINDOWSCONTROLLER_H__C26DD0EC_7D83_4421_9DF7_E35B43E7C73D__INCLUDED_)
#define AFX_OPENMSXWINDOWSCONTROLLER_H__C26DD0EC_7D83_4421_9DF7_E35B43E7C73D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "openMSXController.h"

class PipeConnectThread;
class openMSXWindowsController : public openMSXController  
{
	public:
		void HandleEndProcess (wxCommandEvent &event);
		bool WriteMessage (wxString msg);
		void HandlePipeCreated();
		virtual bool Launch (wxString cmdLine);
		virtual bool HandleMessage (wxCommandEvent & event);
		virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd);
		virtual void HandleNativeEndProcess (){};
		openMSXWindowsController(wxWindow * target);
		virtual ~openMSXWindowsController();

	private:
		void CloseHandles (bool useNamedPipes, HANDLE hThread, HANDLE hInputRead, 
				HANDLE hOutputWrite, HANDLE hErrorWrite);
		void ShowError (wxString msg);
		bool CreatePipes (bool useNamedPipes,HANDLE * input, HANDLE * output, HANDLE * error,
				HANDLE * outputWrite, HANDLE * errorWrite);
		wxString CreateControlParameter (bool useNamedPipes);
		bool DetermenNamedPipeUsage ();

		HANDLE m_outputHandle;
		HANDLE m_namedPipeHandle;
		bool m_pipeActive;
		unsigned long m_launchCounter;
		PipeConnectThread * m_connectThread;	
};

#endif // !defined(AFX_OPENMSXWINDOWSCONTROLLER_H__C26DD0EC_7D83_4421_9DF7_E35B43E7C73D__INCLUDED_)
