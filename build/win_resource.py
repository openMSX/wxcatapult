# Generates Windows resource header.

from outpututils import rewriteIfChanged
from version import extractRevisionNumber, packageVersion, extractRevision

import sys

def iterResourceHeader():
	if '-' in packageVersion:
		versionNumber = packageVersion[ : packageVersion.index('-')]
	else:
		versionNumber = packageVersion
	versionComponents = versionNumber.split('.')
	assert len(versionComponents) in [2, 3], versionComponents
	versionComponents = versionComponents + (4 - len(versionComponents)) * ['0']
	revisionNumber = extractRevisionNumber()
	if revisionNumber:
		versionComponents[3] = str(revisionNumber)

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
		print('Usage: python3 win-resource.py RESOURCE_HEADER', file=sys.stderr)
		sys.exit(2)
