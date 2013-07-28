# $Id$
#
# Configuration for Linux.

# Does platform support symlinks?
USE_SYMLINK:=true

# File name extension of executables.
EXEEXT:=

# The next line is needed to determine the name of the XRC library from wx-config.
WX2XRC_DYNAMIC:=s/-lwx_\\([^-]*\\)-\\([^ ]*\\)/& -lwx_\\1_xrc-\\2/

# Platform specific source files.
SOURCES+=openMSXLinuxController

CXXFLAGS+= -std=c++0x 