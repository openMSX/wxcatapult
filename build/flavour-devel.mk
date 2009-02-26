# $Id$
#
# Configuration for "debug" flavour:
# Build with all debugging info, no optimisations.

# Debug flags.
CXXFLAGS+=-O2 -g -DDEBUG

# Strip executable?
CATAPULT_STRIP:=false
