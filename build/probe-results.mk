# $Id: probe-results.mk,v 1.4 2004/05/06 00:20:11 mthuurne Exp $
#
# Display probe results, so user can decide whether to start the build,
# or to change system configuration and rerun "configure".

# This Makefile needs parameters to operate; check that they were specified:
# - PROBE_MAKE: Makefile with probe results
ifeq ($(PROBE_MAKE),)
$(error Missing parameter: PROBE_MAKE)
endif
# - MAKE_PATH: Directory containing global Makefiles
ifeq ($(MAKE_PATH),)
$(error Missing parameter: MAKE_PATH)
endif

include $(PROBE_MAKE)
include $(MAKE_PATH)/custom.mk

# Usage: $(call FOUND,LIB_NAME)
FOUND=$(if $(HAVE_$(1)_LIB),$(HAVE_$(1)_LIB),no)

# Usage: $(call HEADER,LIB1 LIB2 ...)
HEADER=$(if $(strip $(foreach LIB,$(1),$(HAVE_$(LIB)_H))),yes,no)

all:
	@echo ""
	@echo "Found libraries:"
	@echo "  wxWidgets:       $(call FOUND,WX)"
	@echo "  wxWidgets XRC:   $(call FOUND,XRC)"
	@echo "  libxml2:         $(call FOUND,XML)"
	@echo ""
	@echo "Found headers:"
	@echo "  wxWidgets:       $(call HEADER,WX)"
	@echo "  wxWidgets XRC:   $(call HEADER,XRC)"
	@echo "  libxml2:         $(call HEADER,XML)"
	@echo ""
	@echo "Customizable options:"
	@echo "  Install to:      $(INSTALL_BASE)"
	@echo "  (you can edit these in build/custom.mk)"
	@echo ""

