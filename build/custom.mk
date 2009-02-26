# $Id$
#
# This file contains user-adjustable settings for the build and installation
# process.

# Directory to install to.
# Catapult is always installed into a single self-contained directory.
# But you can change that directory to for example /usr/local/openMSX-Catapult
# or /usr/games/openMSX-Catapult if you like.
INSTALL_BASE:=/opt/openMSX-Catapult

# Create a symbolic link to the installed binary?
# This link is placed in a location that is typically in a user's path:
# /usr/local/bin for system-wide installs and ~/bin for personal installs.
SYMLINK_FOR_BINARY:=true

# Locations for openMSX binaries and share directory
# these are suggested as initial values when you run catapult
# for the first time
CATAPULT_OPENMSX_BINARY:=/opt/openMSX/bin/openmsx
CATAPULT_OPENMSX_SHARE:=/opt/openMSX/share
