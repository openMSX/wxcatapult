# $Id: platform-linux.mk,v 1.1 2004/07/10 15:25:01 mthuurne Exp $
#
# Configuration for Linux on x86 machines.

# Does platform support symlinks?
USE_SYMLINK:=true

# File name extension of executables.
EXEEXT:=

# The next line is needed to determen the name of the XRC library from wx-config
WX2XRC_DYNAMIC:=s/-lwx_\\([^-]*\\)-\\([^ ]*\\)/& -lwx_\\1_xrc-\\2/

#platform specific sourcefiles
SOURCES+= openMSXLinuxController

CXXFLAGS+=-D_REENTRANT -D_THREAD_SAFE
LINK_FLAGS+=-pthread 