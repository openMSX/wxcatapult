# $Id: flavour-i686.mk,v 1.2 2004/05/09 19:04:58 mthuurne Exp $
#
# Configuration for "i686" flavour:
# Optimised for Pentium 2 and higher.

# Optimisation flags.
CXXFLAGS+=-O3 -DNDEBUG \
	-march=i686 -ffast-math -funroll-loops

# Strip executable?
CATAPULT_STRIP:=true
