# $Id: flavour-debug.mk,v 1.1 2004/02/23 01:08:07 mthuurne Exp $
#
# Configuration for "debug" flavour:
# Build with all debugging info, no optimisations.

# Debug flags.
CXXFLAGS+=-O0 -g -DDEBUG

# Strip executable?
OPENMSX_STRIP:=false
