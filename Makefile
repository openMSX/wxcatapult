#
# File:		Makefile for openMSX catapult

VERSION:=\"0.0.1CVS\"

CXX:=$(shell wx-config --cxx)
SED:=sed

SRCDIR:=src
DIALOGSDIR:=dialogs
BITMAPSDIR:=resources/bitmaps
SEDSCRIPT:=wxg2xrc.sed

BUILD_BASE:=derived
OBJDIR:=$(BUILD_BASE)/obj
BINDIR:=$(BUILD_BASE)/bin
PROGRAM:=$(BINDIR)/catapult
XRCDIR:=$(BUILD_BASE)/resources/dialogs

OBJECTS:=$(addprefix $(OBJDIR)/, \
	wxCatapultApp.o \
	wxCatapultFrm.o \
	CatapultConfigDlg.o \
	ConfigurationData.o \
	PipeReadThread.o \
	wxToggleButtonXmlHandler.o \
	CatapultXMLParser.o \
	SessionPage.o \
	StatusPage.o \
	VideoControlPage.o \
	MiscControlPage.o \
	openMSXController.o \
	openMSXLinuxController.o \
	)

DIALOGS:=$(addprefix $(XRCDIR)/, \
	catapult.xrc \
	config.xrc \
	session.xrc \
	misccontrols.xrc \
	videocontrols.xrc \
	status.xrc \
	)

BITMAPS:=$(addprefix $(BUILD_BASE)/,$(wildcard $(BITMAPSDIR)/*.bmp))

CXXFLAGS:=-c -g
CXXFLAGS+=$(shell xml2-config --cflags) $(shell wx-config --cxxflags)
CXXFLAGS+=-DVERSION=$(VERSION)

# implementation

.PHONY: all clean

all: $(PROGRAM) $(DIALOGS) $(BITMAPS)

$(PROGRAM): $(OBJECTS)
	@echo "Linking $(@:$(BINDIR)/%=%)..."
	@mkdir -p $(@D)
	@$(CXX) -g -o $@ $(OBJECTS) `wx-config --libs` -lwx_gtk_xrc-2.4 `xml2-config --libs`

$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $(<:$(SRCDIR)/%=%)..."
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) -o $@ $<

$(DIALOGS): $(XRCDIR)/%.xrc: $(DIALOGSDIR)/%.wxg
	@echo "Converting $(@:$(XRCDIR)/%=%)..."
	@mkdir -p $(@D)
	@$(SED) -f $(SEDSCRIPT) $< > $@

$(BITMAPS): $(BUILD_BASE)/%: %
	@echo "Copying $(<:$(BITMAPSDIR)/%=%)..."
	@mkdir -p $(@D)
	@cp $< $@

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_BASE)
