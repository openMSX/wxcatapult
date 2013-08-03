# $Id$
#
# Configuration for MinGW.

# Does platform support symlinks?
USE_SYMLINK:=false

# File name extension of executables.
EXEEXT:=.exe

# The next lines are needed to determine the name of the XRC library from wx-config.
WX2XRC_DYNAMIC:=s/-lwx\\([^0-9]*\\)\\([0-9]\\)\\([0-9]\\)./ -lwx_\\1_xrc-\\2\\.\\3 & /
WX2XRC_STATIC:=s/\\(\\/[^ ]*\\/\\)libwx\\([^0-9]*\\)\\([0-9]\\)\\([0-9]\\).\\.a/ \\1libwx_\\2_xrc-\\3\\.\\4\\.a & /

# Compiler flags.
CXXFLAGS+= \
	-std=gnu++0x \
	-mthreads -mconsole -mms-bitfields \
	-I/mingw/include -I/mingw/include/w32api \
	`if test -d /usr/local/include; then echo '-I/usr/local/include'; fi` \
	-D__GTHREAD_HIDE_WIN32API \
	-DFS_CASEINSENSE

# Linker flags.
LINK_FLAGS:=-L/mingw/lib -L/mingw/lib/w32api \
	`if test -d /usr/local/lib; then echo '-L/usr/local/lib'; fi` \
	$(LINK_FLAGS)

# Platform specific source files.
SOURCES+=PipeConnectThread
