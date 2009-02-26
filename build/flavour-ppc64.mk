# $Id$
#
# Configuration for "ppc64" flavour:

# Optimisation flags.
CXXFLAGS+=-O3 -DNDEBUG -mpowerpc-gfxopt

# Strip executable?
CATAPULT_STRIP:=true

