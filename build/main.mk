# Makefile for openMSX Catapult
# =============================
#
# Uses a similar approach as the openMSX build system.

# Verbosity
# =========

# V=0: Summary only
# V=1: Command only
# V=2: Summary + command
V?=0
SUM:=@echo
ifeq ($V,0)
CMD:=@
else
CMD:=
ifeq ($V,1)
SUM:=@\#
else ifneq ($V,2)
$(warning Unsupported value for verbosity flag "V": $V)
endif
endif

# Python Interpreter
# ==================

PYTHON?=python3

# Functions
# =========

# Function to check a variable has been defined and has a non-empty value.
# Usage: $(call DEFCHECK,VARIABLE_NAME)
DEFCHECK=$(strip \
	$(if $(filter _undefined,_$(origin $(1))), \
		$(error Variable $(1) is undefined) ) \
	)

# Function to check a boolean variable has value "true" or "false".
# Usage: $(call BOOLCHECK,VARIABLE_NAME)
BOOLCHECK=$(strip \
	$(call DEFCHECK,$(1)) \
	$(if $(filter-out _true _false,_$($(1))), \
		$(error Value of $(1) ("$($(1))") should be "true" or "false") ) \
	)

# Will be added to by platform specific Makefile, by flavour specific Makefile
# and by this Makefile.
# Note: LDFLAGS are passed to the linker itself, LINK_FLAGS are passed to the
#       compiler in the link phase.
CXXFLAGS:=
LDFLAGS:=
LDADD:=
LINK_FLAGS:=
SOURCES:=

# Logical Targets
# ===============

# Logical targets which require dependency files.
DEPEND_TARGETS:=all default install
# Logical targets which do not require dependency files.
NODEPEND_TARGETS:=clean probe
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

# Customisation
# =============

include $(MAKE_PATH)/custom.mk
$(call DEFCHECK,INSTALL_BASE)

# Version
# =======

VERSION_MAKE:=$(MAKE_PATH)/version.mk
include $(VERSION_MAKE)
PACKAGE_FULL:=$(PACKAGE_NAME)-$(PACKAGE_VERSION)

# Platforms
# =========

# Note:
# A platform currently specifies both the host platform (performing the build)
# and the target platform (running the created binary). When we have real
# experience with cross-compilation, a more sophisticated system can be
# designed.

ifeq ($(origin CATAPULT_TARGET_OS),environment)
# Do not perform autodetection if platform was specified by the user.
else # CATAPULT_TARGET_OS not from environment

DETECTSYS_PATH:=$(BUILD_BASE)/detectsys
DETECTSYS_MAKE:=$(DETECTSYS_PATH)/detectsys.mk
DETECTSYS_SCRIPT:=$(MAKE_PATH)/detectsys.py

-include $(DETECTSYS_MAKE)

$(DETECTSYS_MAKE): $(DETECTSYS_SCRIPT)
	$(SUM) "Autodetecting native system:"
	$(CMD)mkdir -p $(@D)
	$(CMD)$(PYTHON) $< > $@

endif # CATAPULT_TARGET_OS

# Ignore rest of Makefile if autodetection was not performed yet.
# Note that the include above will force a reload of the Makefile.
ifneq ($(origin CATAPULT_TARGET_OS),undefined)

# Load OS specific settings.
$(call DEFCHECK,CATAPULT_TARGET_OS)
include $(MAKE_PATH)/platform-$(CATAPULT_TARGET_OS).mk
# Check that all expected variables were defined by OS specific Makefile:
# - executable file name extension
#$(call DEFCHECK,EXEEXT)
# - platform supports symlinks?
#$(call BOOLCHECK,USE_SYMLINK)

# Flavours
# ========

# Load flavour specific settings.
CATAPULT_FLAVOUR?=opt
include $(MAKE_PATH)/flavour-$(CATAPULT_FLAVOUR).mk

# Paths
# =====

SEDSCRIPT:=$(MAKE_PATH)/wxg2xrc.sed
CUSTOM_MAKE:=$(MAKE_PATH)/custom.mk
PROBE_SCRIPT:=$(MAKE_PATH)/probe.mk
COMPONENTS_MAKE:=$(MAKE_PATH)/components.mk

BUILD_PATH:=$(BUILD_BASE)/$(CATAPULT_TARGET_OS)-$(CATAPULT_FLAVOUR)

OBJECTS_PATH:=$(BUILD_PATH)/obj

BINARY_PATH:=$(BUILD_PATH)/bin
BINARY_FILE:=catapult$(EXEEXT)
BINARY_FULL=$(BINARY_PATH)/$(BINARY_FILE) # allow override
REVISION:=$(shell PYTHONPATH=build $(PYTHON) -c \
     "import version; print(version.extractRevisionString())" \
     )

LOG_PATH:=$(BUILD_PATH)/log
RESOURCES_PATH:=$(BUILD_PATH)/resources
XRC_PATH:=$(RESOURCES_PATH)/dialogs

CONFIG_PATH:=$(BUILD_PATH)/config
PROBE_MAKE:=$(CONFIG_PATH)/probed_defs.mk
CONFIG_HEADER:=$(CONFIG_PATH)/config.h
VERSION_HEADER:=$(CONFIG_PATH)/Version.ii

# Configuration
# =============

include $(MAKE_PATH)/info2code.mk
ifneq ($(filter $(DEPEND_TARGETS),$(MAKECMDGOALS)),)
-include $(PROBE_MAKE)
ifeq ($(PROBE_MAKE_INCLUDED),true)
include $(COMPONENTS_MAKE)
$(call BOOLCHECK,COMPONENT_CORE)
endif
endif

ifeq ($(PROBE_MAKE_INCLUDED),true)
# If probe was succesful, it's safe to use wx-config.
CXX:=$(shell wx-config --cxx)
endif


# Filesets
# ========

SOURCES+= \
	CatapultPage \
	wxCatapultApp \
	wxCatapultFrm \
	CatapultConfigDlg \
	ConfigurationData \
	PipeReadThread \
	CatapultXMLParser \
	SessionPage \
	InputPage \
	StatusPage \
	VideoControlPage \
	AudioControlPage \
	MiscControlPage \
	openMSXController \
	FullScreenDlg \
	ScreenShotDlg \
	Version \
	RomTypeDlg \
	IPSSelectionDlg \
	AboutDlg \
	CheckConfigsDlg \
	utils

OBJECTS_FULL:=$(addprefix $(OBJECTS_PATH)/, $(addsuffix .o,$(SOURCES)))

ifeq ($(CATAPULT_TARGET_OS),mingw32)
RESOURCE_SRC:=src/catapult.rc
RESOURCE_OBJ:=$(OBJECTS_PATH)/resources.o
RESOURCE_HEADER:=$(CONFIG_PATH)/resource-info.h
else
RESOURCE_OBJ:=
endif

XRC_FULL:=$(addprefix $(XRC_PATH)/, \
	catapult.xrc \
	config.xrc \
	session.xrc \
	misccontrols.xrc \
	videocontrols.xrc \
	audiocontrols.xrc \
	status.xrc \
	input.xrc \
	fullscreen.xrc \
	screenshot.xrc \
	romtype.xrc \
	ipsselect.xrc \
	about.xrc \
	checkconfigs.xrc \
	)

BITMAPS:=$(addprefix $(BUILD_PATH)/,$(wildcard $(BITMAPS_PATH)/*.png))

DEPEND_PATH:=$(BUILD_PATH)/dep
DEPEND_FULL:=$(addprefix $(DEPEND_PATH)/,$(addsuffix .d, $(SOURCES)))
DEPEND_FLAGS:=
# Empty definition of used headers, so header removal doesn't break things.
DEPEND_FLAGS+=-MP


# Compiler flags
# ==============

CXXFLAGS+=-pipe -Wall -Wextra -Wno-unused-parameter -fno-strict-aliasing
# Suppress warnings triggered by wx headers:
CXXFLAGS+=$(shell \
  echo | $(CXX) -E -Wno-deprecated-copy - >/dev/null 2>&1 \
  && echo -Wno-deprecated-copy \
  )
ifneq ($(filter %clang++,$(CXX))$(filter clang++%,$(CXX)),)
# Clang-specific flags go here.
else
ifneq ($(filter %g++,$(CXX))$(filter g++%,$(CXX))$(findstring /g++-,$(CXX)),)
# GCC-specific flags go here.
CXXFLAGS+=$(shell \
  echo | $(CXX) -E -Wno-cast-function-type - >/dev/null 2>&1 \
  && echo -Wno-cast-function-type \
  )
endif # GCC
endif # not-Clang
CXXFLAGS+=-I$(CONFIG_PATH)
CXXFLAGS+=$(XRC_CFLAGS) $(XML_CFLAGS)
LDADD+=$(XRC_LDFLAGS) $(XML_LDFLAGS)
LINK_FLAGS_PREFIX:=-Wl,
LINK_FLAGS+=$(addprefix $(LINK_FLAGS_PREFIX),$(LDFLAGS))

# Strip binary?
CATAPULT_STRIP?=false
$(call BOOLCHECK,CATAPULT_STRIP)
ifeq ($(CATAPULT_STRIP),true)
  LINK_FLAGS+=-s
endif

# Build Rules
# ===========

# Do not build if core component dependencies are not met.
ifeq ($(COMPONENT_CORE),false)
DUMMY:=$(shell rm -f $(PROBE_MAKE))
$(error Cannot build Catapult because essential libraries are unavailable. \
Please install the needed libraries and rerun (g)make)
endif

# Force a probe if "probe" target is passed explicitly.
ifneq ($(filter probe,$(MAKECMDGOALS)),)
probe: $(PROBE_MAKE)
.PHONY: $(PROBE_MAKE)
endif

.PHONY: forceversionextraction

# Probe for libraries.
# TODO: It would be cleaner to include probe.mk and probe-results.mk,
#       instead of executing them in a sub-make.
$(PROBE_MAKE): $(PROBE_SCRIPT)
	$(CMD)OUTDIR=$(@D) COMPILE=g++ MAKE_LOCATION=$(MAKE_PATH) CURRENT_OS=$(CATAPULT_TARGET_OS)\
		$(MAKE) --no-print-directory -f $<
	$(CMD)PROBE_MAKE=$(PROBE_MAKE) MAKE_PATH=$(MAKE_PATH) \
		$(MAKE) --no-print-directory -f $(MAKE_PATH)/probe-results.mk

# TODO: Relying on CONFIG_HEADER being built before BINARY_FULL,
#       this might break parallelized builds.
all: $(CONFIG_HEADER) $(VERSION_HEADER) config $(BINARY_FULL) $(XRC_FULL) $(BITMAPS)

config:
	@echo "Build configuration"
	@echo "  Target OS: $(CATAPULT_TARGET_OS)"
	@echo "  Flavour:   $(CATAPULT_FLAVOUR)"

$(BINARY_FULL): $(OBJECTS_FULL) $(RESOURCE_OBJ)
	$(SUM) "Linking $(BINARY_FILE)..."
	$(CMD)mkdir -p $(@D)
	$(CMD)$(CXX) $(LINK_FLAGS) -o $@ $^ $(LDADD)

# Compile and generate dependency files in one go.
DEPEND_SUBST=$(patsubst $(SOURCES_PATH)/%.cpp,$(DEPEND_PATH)/%.d,$<)
$(OBJECTS_FULL): $(OBJECTS_PATH)/%.o: $(SOURCES_PATH)/%.cpp $(DEPEND_PATH)/%.d
	$(SUM) "Compiling $(<:$(SOURCES_PATH)/%.cpp=%)..."
	$(CMD)mkdir -p $(@D)
	$(CMD)mkdir -p $(patsubst $(OBJECTS_PATH)%,$(DEPEND_PATH)%,$(@D))
	$(CMD)$(CXX) $(DEPEND_FLAGS) -MMD -MF $(DEPEND_SUBST) \
		-o $@ $(CXXFLAGS) -c $<
	$(CMD)touch $@ # Force .o file to be newer than .d file.

ifeq ($(CATAPULT_TARGET_OS),mingw32)
$(RESOURCE_HEADER): $(VERSION_MAKE) forceversionextraction
	$(PYTHON) $(BUILD_PATH)/win_resource.py $@
$(RESOURCE_OBJ): $(RESOURCE_SRC) $(RESOURCE_HEADER)
	$(SUM) "Compiling resources..."
	$(CMD)windres $(addprefix --include-dir=,$(^D)) -o $@ -i $<
endif

$(XRC_FULL): $(XRC_PATH)/%.xrc: $(DIALOGS_PATH)/%.wxg $(SEDSCRIPT)
	$(SUM) "Converting $(@:$(XRC_PATH)/%=%)..."
	$(CMD)mkdir -p $(@D)
	$(CMD)sed -f $(SEDSCRIPT) $< > $@

$(BITMAPS): $(BUILD_PATH)/%: %
	$(SUM) "Copying $(<:$(BITMAPS_PATH)/%=%)..."
	$(CMD)mkdir -p $(@D)
	$(CMD)cp $< $@

clean:
	$(SUM) "Cleaning up..."
	$(CMD)rm -rf $(BUILD_PATH)


# Installation and Binary Packaging
# =================================

CATAPULT_PREBUILT?=false
$(call BOOLCHECK,CATAPULT_PREBUILT)

INSTALL_DOCS:=authors.txt GPL.txt release-history.txt release-notes.txt
CATAPULT_INSTALL?=$(INSTALL_BASE)
# Allow full customization of locations, used by Debian packaging.
INSTALL_BINARY_DIR?=$(CATAPULT_INSTALL)/bin
INSTALL_SHARE_DIR?=$(CATAPULT_INSTALL)
INSTALL_DOC_DIR?=$(CATAPULT_INSTALL)/doc


ifeq ($(CATAPULT_PREBUILT),true)
# TODO: Prebuilt is used only on win32, but using this is not clean.
BINARY_FILE:=catapult.exe
FILES_ONLY:=true
install: check_build
else
FILES_ONLY:=false

# DESTDIR is a convention shared by at least GNU and FreeBSD to specify a path
# prefix that will be used for all installed files.
INSTALL_PREFIX:=$(if $(DESTDIR),$(DESTDIR)/,)

install: all
endif
	$(SUM) "Installing to $(INSTALL_PREFIX)$(CATAPULT_INSTALL):"
	$(SUM) "  Executable..."
	$(CMD)mkdir -p $(INSTALL_PREFIX)$(INSTALL_BINARY_DIR)
ifeq ($(CATAPULT_PREBUILT),true)
	$(CMD)cp -f $(BINARY_FULL) $(INSTALL_PREFIX)$(INSTALL_BINARY_DIR)/$(BINARY_FILE)
else
	$(CMD)strip -o $(INSTALL_PREFIX)$(INSTALL_BINARY_DIR)/$(BINARY_FILE) $(BINARY_FULL)
endif
	$(SUM) "  Data files..."
	$(CMD)mkdir -p $(INSTALL_PREFIX)$(INSTALL_SHARE_DIR)
	$(CMD)cp -rf $(RESOURCES_PATH) $(INSTALL_PREFIX)$(INSTALL_SHARE_DIR)/
	$(SUM) "  Documentation..."
	$(CMD)mkdir -p $(INSTALL_PREFIX)$(INSTALL_DOC_DIR)
	$(CMD)cp -f README $(INSTALL_PREFIX)$(INSTALL_DOC_DIR)
	$(CMD)cp -f $(addprefix doc/,$(INSTALL_DOCS)) $(INSTALL_PREFIX)$(INSTALL_DOC_DIR)
	$(CMD)mkdir -p $(INSTALL_PREFIX)$(INSTALL_DOC_DIR)/manual
	$(CMD)cp -f $(addprefix doc/manual/,*.html *.css *.png) \
		$(INSTALL_PREFIX)$(INSTALL_DOC_DIR)/manual
ifeq ($(CATAPULT_PREBUILT),false)
ifneq ($(CATAPULT_NO_DESKTOP_HOOKS),true)
	$(SUM) "  Desktop hooks..."
	$(CMD)mkdir -p $(INSTALL_PREFIX)$(INSTALL_SHARE_DIR)/resources/icons
	$(CMD)cp -rf src/catapult.xpm $(INSTALL_PREFIX)$(INSTALL_SHARE_DIR)/resources/icons
	$(CMD)if [ -d $(INSTALL_PREFIX)/usr/share/applications -a -w $(INSTALL_PREFIX)/usr/share/applications ]; \
		then sed -e "s|%INSTALL_BASE%|$(INSTALL_SHARE_DIR)|" \
			desktop/openMSX-Catapult.desktop \
			> $(INSTALL_PREFIX)/usr/share/applications/openMSX-Catapult.desktop; \
		else mkdir -p ~/.local/share/applications && \
			sed -e "s|%INSTALL_BASE%|$(INSTALL_SHARE_DIR)|" \
			desktop/openMSX-Catapult.desktop \
			> ~/.local/share/applications/openMSX-Catapult.desktop; \
		fi
endif
ifeq ($(SYMLINK_FOR_BINARY),true)
	$(SUM) "  Creating symlink..."
	$(CMD)if [ -d /usr/local/bin -a -w /usr/local/bin ]; \
		then ln -sf $(INSTALL_BINARY_DIR)/$(BINARY_FILE) \
			/usr/local/bin/$(BINARY_FILE); \
		else if [ -d ~/bin ]; \
			then ln -sf $(INSTALL_BINARY_DIR)/$(BINARY_FILE) \
				~/bin/$(BINARY_FILE); \
			fi; \
		fi
endif
	$(SUM) "  Setting permissions..."
	$(CMD)chmod -R a+rX $(INSTALL_PREFIX)$(INSTALL_SHARE_DIR)
endif # CATAPULT_PREBUILT
	$(SUM) "Installation complete... have fun!"

TARGET_CATAPULT:=$(wildcard $(BINARY_PATH)/$(BINARY_FILE))

check_build:
ifeq ($(TARGET_CATAPULT),)
	$(error Create Catapult first)
endif

# Source Packaging
# ================

DIST_BASE:=$(BUILD_BASE)/dist
DIST_PATH:=$(DIST_BASE)/$(PACKAGE_FULL)

DIST_FULL:= \
	GNUmakefile README
DIST_FULL+=$(addprefix $(SOURCES_PATH)/, \
	*.h *.cpp *.rc *.ico *.xpm \
	)
DIST_FULL+=$(addprefix $(MAKE_PATH)/, \
	*.mk *.sed *.py \
	msvc/*.py msvc/*.sln msvc/*.vcxproj msvc/*.vcxproj.filters \
	3rdparty/*.sln 3rdparty/*.props 3rdparty/*.vcxproj \
	3rdparty/*.vcxproj.filters 3rdparty/*.patch msvc/sed/* \
	)
DIST_FULL+=$(DIALOGS_PATH)/*.wxg
DIST_FULL+=$(BITMAPS_PATH)/*.png
DIST_FULL+=$(addprefix doc/, \
	*.txt \
	$(addprefix manual/, \
		*.html *.css *.png \
		) \
	)
DIST_FULL+=$(addprefix desktop/, \
	openMSX-Catapult.desktop \
	)

dist:
	$(SUM) "Removing any old distribution files..."
	$(CMD)rm -rf $(DIST_PATH)
	$(SUM) "Gathering files for distribution..."
	$(CMD)mkdir -p $(DIST_PATH)
	$(CMD)cp -p --parents $(DIST_FULL) $(DIST_PATH)
	$(SUM) "Creating tarball..."
	$(CMD)cd $(DIST_BASE) ; GZIP=--best tar zcf $(PACKAGE_FULL).tar.gz $(PACKAGE_FULL)


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

endif # CATAPULT_TARGET_OS
