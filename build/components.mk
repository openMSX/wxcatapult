# $Id$
# Defines the building blocks of Catapult and their dependencies.

ifneq ($(PROBE_MAKE_INCLUDED),true)
$(error Include probe results before including "components.mk")
endif

# Use XRC2 if available, otherwise XRC1.
HAVE_XRC_LIB:=$(if $(HAVE_XRC2_LIB),$(HAVE_XRC2_LIB),$(HAVE_XRC1_LIB))
XRC_LDFLAGS:=$(if $$(HAVE_XRC2_LIB),$(XRC2_LDFLAGS),$(XRC1_LDFLAGS))

CORE_LIBS:=WX XRC XML
ifneq ($(filter x,$(foreach LIB,$(CORE_LIBS),x$(HAVE_$(LIB)_LIB))),)
COMPONENT_CORE:=false
endif
ifneq ($(filter x,$(foreach LIB,$(CORE_LIBS),x$(HAVE_$(LIB)_H))),)
COMPONENT_CORE:=false
endif
COMPONENT_CORE?=true

