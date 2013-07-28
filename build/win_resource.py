# $Id$
# Generates Windows resource header.

from outpututils import rewriteIfChanged
from version import extractRevisionNumber, packageVersion, extractRevision

import sys

def iterResourceHeader():
	if '-' in packageVersion:
		versionNumber = packageVersion[ : packageVersion.index('-')]
	else:
		versionNumber = packageVersion
	versionComponents = versionNumber.split('.') + [ str(extractRevisionNumber()) ]
	assert len(versionComponents) == 4, versionComponents

	versionStr = packageVersion
	extractedRevision = extractRevision()
	if extractedRevision is not None:
		versionStr += "-" + extractedRevision

	yield '#define CATAPULT_VERSION_INT %s' % ', '.join(versionComponents)
	yield '#define CATAPULT_VERSION_STR "%s\\0"' % versionStr

if __name__ == '__main__':
	if len(sys.argv) == 2:
		rewriteIfChanged(sys.argv[1], iterResourceHeader())
	else:
		print >> sys.stderr, \
			'Usage: python win-resource.py RESOURCE_HEADER'
		sys.exit(2)
