# $Id: platform-darwin.mk,v 1.1 2004/11/14 18:34:26 h_oudejans Exp $
#
# Configuration for Darwin.

# Does platform support symlinks?
USE_SYMLINK:=true

# File name extension of executables.
EXEEXT:=

# Bind when executable is loaded, rather then when symbols are accessed.
# I don't know why, but the linker suggests this.
LINK_FLAGS+=-bind_at_load

# The next line is needed to determen the name of the XRC library from wx-config
WX2XRC_DYNAMIC:=s/-lwx_\\([^-]*\\)-\\([^ ]*\\)/& -lwx_\\1_xrc-\\2/

#platform specific sourcefiles
SOURCES+= openMSXLinuxController
