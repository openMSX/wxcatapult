# $Id: flavour-debug.mk,v 1.1 2004/11/14 18:34:26 h_oudejans Exp $
#
# Configuration for "debug" flavour:
# Build with all debugging info, no optimisations.

# Debug flags.
CXXFLAGS+=-O0 -g -DDEBUG

# Strip executable?
CATAPULT_STRIP:=false
