# $Id: flavour-ppc.mk,v 1.1 2004/11/14 18:34:26 h_oudejans Exp $
#
# Configuration for "ppc" flavour:
# Optimised for PPC-G4 and higher.

# Optimisation flags.
# Needs a manual setting ('export POWERPCG4=true') to optimise for PPC74xx/G4
CXXFLAGS+=-O3 -DNDEBUG -mpowerpc-gfxopt
ifeq ($(POWERPCG4),true)
CXXFLAGS+=-mcpu=7450 -maltivec -mabi=altivec
else
CXXFLAGS+=-mcpu=750
endif

# Strip executable?
CATAPULT_STRIP:=true

