# Contains the Catapult version number and versioning related functions.

from executils import captureStdout
from makeutils import filterLines

from os import makedirs
from os.path import isdir

import re

# Name used for packaging.
packageName = 'catapult'

# Version number.
packageVersion = '0.14.0-rc1'

# Is this a release version ("True") or development version ("False").
releaseFlag = True

# TODO: Before extraction of SVN or git-SVN revision number can be done, we
#       should figure out a way to avoid rewriting Version.ii on every build.
#       Option 1: Read Version.ii and do not write if new contents are the same.
#       Option 2: Persist the extracted revision number.
#       I prefer option 2, since it is more modular (separate extraction and
#       include generation steps) but option 1 might be easier to implement at
#       first (no need to persist anything).

def _extractRevisionFromStdout(log, command, regex):
	text = captureStdout(log, command)
	if text is None:
		# Error logging already done by captureStdout().
		return None
	# pylint 0.18.0 somehow thinks captureStdout() returns a list, not a string.
	lines = text.split('\n') # pylint: disable-msg=E1103
	for revision, in filterLines(lines, regex):
		print >> log, 'Revision number found by "%s": %s' % (command, revision)
		return revision
	else:
		print >> log, 'Revision number not found in "%s" output:' % command
		print >> log, text
		return None

def extractGitRevision(log):
	return _extractRevisionFromStdout(
		log, 'git describe', r'\S+?-(\S+)$'
		)

def extractNumberFromGitRevision(revisionStr):
	if revisionStr is None:
		return None
	return re.match(r'(\d+)+', revisionStr).group(0)

def extractRevision():
	if releaseFlag:
		# Not necessary, we do not append revision for a release build.
		return None
	if not isdir('derived'):
		makedirs('derived')
	log = open('derived/version.log', 'w')
	print >> log, 'Extracting revision info...'
	try:
		revision = extractGitRevision(log)
		print >> log, 'Revision string: %s' % revision
		print >> log, 'Revision number: %s' % extractNumberFromGitRevision(revision)
	finally:
		log.close()
	return revision

def extractRevisionNumber():
	return int(extractNumberFromGitRevision(extractRevision()) or 1)

def extractRevisionString():
	return extractRevision() or 'unknown'

def getVersionedPackageName():
	if releaseFlag:
		return '%s-%s' % (packageName, packageVersion)
	else:
		return '%s-%s-%s' % (packageName, packageVersion, extractRevision())
