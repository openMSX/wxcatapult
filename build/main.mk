# $Id: main.mk,v 1.1 2004/03/06 14:13:52 mthuurne Exp $
#
# Makefile for openMSX Catapult
# =============================

VERSION:=0.0.1CVS

CONFIG_MAKEFILE:=build/config.mk

include $(CONFIG_MAKEFILE)

CXX:=$(shell wx-config --cxx)
SED:=sed

SOURCES_PATH:=src
DIALOGSDIR:=dialogs
BITMAPSDIR:=resources/bitmaps
SEDSCRIPT:=build/wxg2xrc.sed

BUILD_BASE:=derived
BUILD_PATH:=$(BUILD_BASE)
OBJECTS_PATH:=$(BUILD_PATH)/obj
BINDIR:=$(BUILD_PATH)/bin
PROGRAM:=$(BINDIR)/catapult
XRCDIR:=$(BUILD_PATH)/resources/dialogs

CONFIG_PATH:=$(BUILD_BASE)/config
CONFIG_HEADER:=$(CONFIG_PATH)/config.h

SOURCES:= \
	wxCatapultApp \
	wxCatapultFrm \
	CatapultConfigDlg \
	ConfigurationData \
	PipeReadThread \
	wxToggleButtonXmlHandler \
	CatapultXMLParser \
	SessionPage \
	StatusPage \
	VideoControlPage \
	MiscControlPage \
	openMSXController \
	openMSXLinuxController

OBJECTS_FULL:=$(addprefix $(OBJECTS_PATH)/, $(addsuffix .o,$(SOURCES)))

DIALOGS:=$(addprefix $(XRCDIR)/, \
	catapult.xrc \
	config.xrc \
	session.xrc \
	misccontrols.xrc \
	videocontrols.xrc \
	status.xrc \
	)

BITMAPS:=$(addprefix $(BUILD_PATH)/,$(wildcard $(BITMAPSDIR)/*.bmp))

DEPEND_PATH:=$(BUILD_PATH)/dep
DEPEND_FULL:=$(addprefix $(DEPEND_PATH)/,$(addsuffix .d, $(SOURCES)))
DEPEND_FLAGS:=
# Empty definition of used headers, so header removal doesn't break things.
DEPEND_FLAGS+=-MP

CXXFLAGS:=-g -pipe -Wall
CXXFLAGS+=-I$(dir $(CONFIG_HEADER))
CXXFLAGS+=$(shell xml2-config --cflags) $(shell wx-config --cxxflags)


# Logical Targets
# ===============

# Logical targets which require dependency files.
DEPEND_TARGETS:=all default
# Logical targets which do not require dependency files.
NODEPEND_TARGETS:=clean
# Mark all logical targets as such.
.PHONY: $(DEPEND_TARGETS) $(NODEPEND_TARGETS)

# Default target; make sure this is always the first target in this Makefile.
MAKECMDGOALS?=default
default: all


# Build Rules
# ===========

# TODO: Relying on CONFIG_HEADER being built before PROGRAM, this might break
#       parallelized builds.
all: $(CONFIG_HEADER) $(PROGRAM) $(DIALOGS) $(BITMAPS)

$(PROGRAM): $(OBJECTS_FULL)
	@echo "Linking $(@:$(BINDIR)/%=%)..."
	@mkdir -p $(@D)
	@$(CXX) -g -o $@ $(OBJECTS_FULL) \
		`wx-config --libs` -lwx_gtk_xrc-2.4 `xml2-config --libs`

# Compile and generate dependency files in one go.
DEPEND_SUBST=$(patsubst $(SOURCES_PATH)/%.cpp,$(DEPEND_PATH)/%.d,$<)
$(OBJECTS_FULL): $(OBJECTS_PATH)/%.o: $(SOURCES_PATH)/%.cpp $(DEPEND_PATH)/%.d
	@echo "Compiling $(<:$(SOURCES_PATH)/%.cpp=%)..."
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJECTS_PATH)%,$(DEPEND_PATH)%,$(@D))
	@$(CXX) $(DEPEND_FLAGS) -MMD -MF $(DEPEND_SUBST) \
		-o $@ $(CXXFLAGS) -c $<
	@touch $@ # Force .o file to be newer than .d file.

$(CONFIG_HEADER): $(CONFIG_MAKEFILE)
	@echo "Creating $@..."
	@mkdir -p $(@D)
	@echo "// Automatically generated by build process." > $@
	@echo "#ifndef __CONFIG_H_INCLUDED" >> $@
	@echo "#define __CONFIG_H_INCLUDED" >> $@
	@echo "#define VERSION \"$(VERSION)\"" >> $@
	@echo "#define RESOURCEDIR \"$(INSTALL_BASE)/resources\"" >> $@
	@echo "#endif" >> $@

$(DIALOGS): $(XRCDIR)/%.xrc: $(DIALOGSDIR)/%.wxg
	@echo "Converting $(@:$(XRCDIR)/%=%)..."
	@mkdir -p $(@D)
	@$(SED) -f $(SEDSCRIPT) $< > $@

$(BITMAPS): $(BUILD_PATH)/%: %
	@echo "Copying $(<:$(BITMAPSDIR)/%=%)..."
	@mkdir -p $(@D)
	@cp $< $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_PATH)


# Dependencies
# ============

# Include dependency files.
ifneq ($(filter $(DEPEND_TARGETS),$(MAKECMDGOALS)),)
  -include $(DEPEND_FULL)
endif

# Generate dependencies that do not exist yet.
# This is only in case some .d files have been deleted;
# in normal operation this rule is never triggered.
$(DEPEND_FULL):
