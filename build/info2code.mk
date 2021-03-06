# Write build info to C++ constants, so source can access it.
# Advantages of this approach:
# - file dates used for dependency checks (as opposed to "-D" compile flag)
# - inactive code is still checked by compiler (as opposed to "#if")

$(CONFIG_HEADER): $(MAKE_PATH)/info2code.mk $(CUSTOM_MAKE)
	@echo "Creating $@..."
	@mkdir -p $(@D)
	@echo "// Automatically generated by build process." > $@
	@echo "" >> $@
	@echo "#ifndef CONFIG_H" >> $@
	@echo "#define CONFIG_H" >> $@
	@echo "" >> $@
	@echo "#include <wx/string.h>" >> $@
	@echo "" >> $@
	@echo "#ifndef __WXMSW__" >> $@
	@echo "static const wxString RESOURCEDIR = wxT(\"$(INSTALL_BASE)/resources\");" >> $@
	@echo "static const wxString CATAPULT_OPENMSX_BINARY = wxT(\"$(CATAPULT_OPENMSX_BINARY)\");" >> $@
	@echo "static const wxString CATAPULT_OPENMSX_SHARE = wxT(\"$(CATAPULT_OPENMSX_SHARE)\");" >> $@
	@echo "#endif" >> $@
	@echo "" >> $@
	@echo "#endif" >> $@

$(VERSION_HEADER): forceversionextraction $(MAKE_PATH)/info2code.mk $(MAKE_PATH)/version.mk
	@echo "Creating $@..."
	@mkdir -p $(@D)
	@echo "// Automatically generated by build process." > $@
	@echo "" >> $@
	@echo "const bool Version::RELEASE = $(RELEASE_FLAG);" >> $@
	@echo "const std::string Version::VERSION = \"$(PACKAGE_VERSION)\";" >> $@
	@echo "const std::string Version::REVISION = \"$(REVISION)\";" >> $@

