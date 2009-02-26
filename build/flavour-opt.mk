# $Id$
#
# Generic optimisation flavour:
# does not target any specific CPU.

# Optimisation flags.
CXXFLAGS+=-O3 -DNDEBUG \
	-ffast-math -funroll-loops

# Strip executable?
CATAPULT_STRIP:=true
