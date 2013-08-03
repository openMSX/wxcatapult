#ifndef OPENMSXLINUXCONTROLLER_H
#define OPENMSXLINUXCONTROLLER_H

#include "openMSXController.h"
#include <string>

class PipeReadThread;

class openMSXLinuxController : public openMSXController
{
public:
	openMSXLinuxController(wxWindow* target);
	virtual ~openMSXLinuxController();

private:
	virtual void WriteMessage(xmlChar* msg, size_t length);
	virtual bool Launch(wxString cmdline);
	virtual wxString GetOpenMSXVersionInfo(wxString openmsxCmd);
	virtual void HandleNativeEndProcess();
	bool execute(const std::string& command, int& fdIn, int& fdOut, int& fdErr);

	int m_openMSXstdin;
	int m_openMSXstdout;
	int m_openMSXstderr;
	PipeReadThread* m_stdErrThread;
	PipeReadThread* m_stdOutThread;
	int m_socket;
};

#endif
