# $Id: main.mk,v 1.7 2004/04/17 15:49:54 h_oudejans Exp $
#
# Makefile for openMSX Catapult
# =============================
#
# Uses a similar approach as the openMSX build system.


# Logical Targets
# ===============

# Logical targets which require dependency files.
DEPEND_TARGETS:=all default install
# Logical targets which do not require dependency files.
NODEPEND_TARGETS:=clean
# Mark all logical targets as such.
.PHONY: $(DEPEND_TARGETS) $(NODEPEND_TARGETS)

# Default target; make sure this is always the first target in this Makefile.
MAKECMDGOALS?=default
default: all


# Base Directories
# ================

SOURCES_PATH:=src
DIALOGS_PATH:=dialogs
BITMAPS_PATH:=resources/bitmaps
MAKE_PATH:=build
BUILD_BASE:=derived


# Paths
# =====

SEDSCRIPT:=$(MAKE_PATH)/wxg2xrc.sed
CONFIG_MAKEFILE:=$(MAKE_PATH)/config.mk

BUILD_PATH:=$(BUILD_BASE)

OBJECTS_PATH:=$(BUILD_PATH)/obj

BINARY_PATH:=$(BUILD_PATH)/bin
BINARY_FILE:=catapult
BINARY_FULL:=$(BINARY_PATH)/$(BINARY_FILE)

RESOURCES_PATH:=$(BUILD_PATH)/resources
XRC_PATH:=$(RESOURCES_PATH)/dialogs

CONFIG_PATH:=$(BUILD_BASE)/config
CONFIG_HEADER:=$(CONFIG_PATH)/config.h
VERSION_HEADER:=$(CONFIG_PATH)/Version.ii


# Configuration
# =============

include $(MAKE_PATH)/version.mk
PACKAGE_FULL:=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
CHANGELOG_REVISION:=\
	$(shell sed -ne "s/\$$Id: ChangeLog,v \([^ ]*\).*/\1/p" ChangeLog)

include $(CONFIG_MAKEFILE)

include $(MAKE_PATH)/info2code.mk


# Filesets
# ========

SOURCES:= \
	CatapultPage \
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
	AudioControlPage \
	MiscControlPage \
	openMSXController \
	openMSXLinuxController \
	FullScreenDlg \
	Version

OBJECTS_FULL:=$(addprefix $(OBJECTS_PATH)/, $(addsuffix .o,$(SOURCES)))

XRC_FULL:=$(addprefix $(XRC_PATH)/, \
	catapult.xrc \
	config.xrc \
	session.xrc \
	misccontrols.xrc \
	videocontrols.xrc \
	audiocontrols.xrc \
	status.xrc \
	fullscreen.xrc \
	)

BITMAPS:=$(addprefix $(BUILD_PATH)/,$(wildcard $(BITMAPS_PATH)/*.bmp))

DEPEND_PATH:=$(BUILD_PATH)/dep
DEPEND_FULL:=$(addprefix $(DEPEND_PATH)/,$(addsuffix .d, $(SOURCES)))
DEPEND_FLAGS:=
# Empty definition of used headers, so header removal doesn't break things.
DEPEND_FLAGS+=-MP


# Compiler and Flags
# ==================

CXX:=$(shell wx-config --cxx)

CXXFLAGS:=-g -pipe -Wall
CXXFLAGS+=-I$(CONFIG_PATH)
CXXFLAGS+=$(shell xml2-config --cflags) $(shell wx-config --cxxflags)

LDFLAGS:=-g `wx-config --libs` -lwx_gtk_xrc-2.4 `xml2-config --libs`


# Build Rules
# ===========

# TODO: Relying on CONFIG_HEADER being built before BINARY_FULL,
#       this might break parallelized builds.
all: $(CONFIG_HEADER) $(VERSION_HEADER) $(BINARY_FULL) $(XRC_FULL) $(BITMAPS)

$(BINARY_FULL): $(OBJECTS_FULL)
	@echo "Linking $(BINARY_FILE)..."
	@mkdir -p $(@D)
	@$(CXX) $(LDFLAGS) -o $@ $(OBJECTS_FULL)

# Compile and generate dependency files in one go.
DEPEND_SUBST=$(patsubst $(SOURCES_PATH)/%.cpp,$(DEPEND_PATH)/%.d,$<)
$(OBJECTS_FULL): $(OBJECTS_PATH)/%.o: $(SOURCES_PATH)/%.cpp $(DEPEND_PATH)/%.d
	@echo "Compiling $(<:$(SOURCES_PATH)/%.cpp=%)..."
	@mkdir -p $(@D)
	@mkdir -p $(patsubst $(OBJECTS_PATH)%,$(DEPEND_PATH)%,$(@D))
	@$(CXX) $(DEPEND_FLAGS) -MMD -MF $(DEPEND_SUBST) \
		-o $@ $(CXXFLAGS) -c $<
	@touch $@ # Force .o file to be newer than .d file.

$(XRC_FULL): $(XRC_PATH)/%.xrc: $(DIALOGS_PATH)/%.wxg $(SEDSCRIPT)
	@echo "Converting $(@:$(XRC_PATH)/%=%)..."
	@mkdir -p $(@D)
	@sed -f $(SEDSCRIPT) $< > $@

$(BITMAPS): $(BUILD_PATH)/%: %
	@echo "Copying $(<:$(BITMAPS_PATH)/%=%)..."
	@mkdir -p $(@D)
	@cp $< $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_PATH)


# Installation
# ============

install: all
	@echo "Installing to $(INSTALL_BASE):"
	@echo "  Executable..."
	@mkdir -p $(INSTALL_BASE)/bin
	@cp $(BINARY_FULL) $(INSTALL_BASE)/bin/$(BINARY_FILE)
	@echo "  Data files..."
	@cp -r $(RESOURCES_PATH) $(INSTALL_BASE)/
	@echo "  Creating symlink..."
	@if [ `id -u` -eq 0 ]; \
		then ln -sf $(INSTALL_BASE)/bin/$(BINARY_FILE) \
			/usr/local/bin/$(BINARY_FILE); \
		else if test -d ~/bin; \
			then ln -sf $(INSTALL_BASE)/bin/$(BINARY_FILE) \
				~/bin/$(BINARY_FILE); \
			fi; \
		fi
	@echo "  Setting permissions..."
	@chmod -R a+rX $(INSTALL_BASE)
	@echo "Installation complete... have fun!"


# Packaging
# =========

DIST_BASE:=$(BUILD_BASE)/dist
DIST_PATH:=$(DIST_BASE)/$(PACKAGE_FULL)

dist: $(DETECTSYS_SCRIPT)
	@echo "Removing any old distribution files..."
	@rm -rf $(DIST_PATH)
	@echo "Gathering files for distribution..."
	@mkdir -p $(DIST_PATH)
	@cp -pr --parents $(DIST_FULL) $(DIST_PATH)
	@cp -p --parents $(HEADERS_FULL) $(DIST_PATH)
	@cp -p --parents $(SOURCES_FULL) $(DIST_PATH)
	@echo "Creating tarball..."
	@cd $(DIST_BASE) ; GZIP=--best tar zcf $(PACKAGE_FULL).tar.gz $(PACKAGE_FULL)


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
