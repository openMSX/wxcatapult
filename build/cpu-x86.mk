# $Id: cpu-x86.mk,v 1.1 2004/07/10 15:25:01 mthuurne Exp $
#
# Configuration for x86 CPUs.

# Big or little endian?
BIG_ENDIAN:=false

# Default build flavour.
# To run openMSX with decent speed, at least a Pentium 2 class machine
# is needed, so let's optimise for that.
CATAPULT_FLAVOUR?=i686

