# Detect the native OS.
# Actually we rely on the Python "platform" module and map its output to names
# that the Catapult build understands.

from platform import machine, python_version, system
import sys

def detectOS():
	'''Detects the operating system of the machine were are running on.
	Raises ValueError if no known OS is detected.
	'''
	os = system().lower()
	if os in ('linux', 'darwin', 'freebsd', 'netbsd', 'openbsd', 'gnu'):
		return os
	elif os.startswith('gnu/'):
		# GNU userland on non-Hurd kernel, for example Debian GNU/kFreeBSD.
		# For openMSX the kernel is not really relevant, so treat it like
		# a generic GNU system.
		return 'gnu'
	elif os.startswith('mingw') or os == 'windows':
		return 'mingw32'
	elif os == 'sunos':
		return 'solaris'
	elif os == '':
		# Python couldn't figure it out.
		raise ValueError('Unable to detect OS')
	else:
		raise ValueError('Unsupported or unrecognised OS "%s"' % os)

if __name__ == '__main__':
	try:
		print(
			'  Using Python %s native system detection...' % python_version(),
			file=sys.stderr
			)
		hostOS = detectOS()
		print('  Detected OS: %s' % (hostOS,), file=sys.stderr)
		print('CATAPULT_TARGET_OS=%s' % hostOS)
	except ValueError as ex:
		print(ex, file=sys.stderr)
		sys.exit(1)
