# $Id: flavour-opt.mk,v 1.1 2004/10/09 06:32:02 mthuurne Exp $
#
# Generic optimisation flavour:
# does not target any specific CPU.

# Optimisation flags.
CXXFLAGS+=-O3 -DNDEBUG \
	-ffast-math -funroll-loops

# Strip executable?
CATAPULT_STRIP:=true
