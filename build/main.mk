# $Id: Makefile,v 1.4 2004/02/07 06:47:35 mthuurne Exp $
#
# Makefile for openMSX Catapult
# =============================

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

all: $(PROGRAM) $(DIALOGS) $(BITMAPS)

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
