# $Id: flavour-debug.mk,v 1.2 2004/11/14 19:05:18 h_oudejans Exp $
#
# Configuration for "debug" flavour:
# Build with all debugging info, no optimisations.

# Debug flags.
CXXFLAGS+=-O2 -g -DDEBUG

# Strip executable?
CATAPULT_STRIP:=false
