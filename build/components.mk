# $Id: components.mk,v 1.1 2004/05/09 15:25:05 mthuurne Exp $
# Defines the building blocks of Catapult and their dependencies.

ifneq ($(PROBE_MAKE_INCLUDED),true)
$(error Include probe results before including "components.mk")
endif

CORE_LIBS:=WX XRC XML
ifneq ($(filter x,$(foreach LIB,$(CORE_LIBS),x$(HAVE_$(LIB)_LIB))),)
COMPONENT_CORE:=false
endif
ifneq ($(filter x,$(foreach LIB,$(CORE_LIBS),x$(HAVE_$(LIB)_H))),)
COMPONENT_CORE:=false
endif
COMPONENT_CORE?=true

