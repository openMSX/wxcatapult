# $Id: flavour-ppc.mk,v 1.2 2005/01/26 02:36:57 mthuurne Exp $
#
# Configuration for "ppc64" flavour:

# Optimisation flags.
CXXFLAGS+=-O3 -DNDEBUG -mpowerpc-gfxopt

# Strip executable?
CATAPULT_STRIP:=true

