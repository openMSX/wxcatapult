# $Id$
#
# Configuration for FreeBSD.

# Does platform support symlinks?
USE_SYMLINK:=true

# File name extension of executables.
EXEEXT:=

# The next line is needed to determine the name of the XRC library from wx-config.
WX2XRC_DYNAMIC:=s/-lwx_\\([^-]*\\)-\\([^ ]*\\)/& -lwx_\\1_xrc-\\2/

CXXFLAGS+=-D_REENTRANT -D_THREADSAFE \
	`if [ -d /usr/local/include ]; then echo '-I/usr/local/include'; fi`

LINK_FLAGS+=-pthread \
	`if [ -d /usr/local/lib ]; then echo '-L/usr/local/lib'; fi`
