#!/bin/sh
# $Id: detectsys.sh,v 1.5 2004/10/22 21:39:17 mthuurne Exp $

MYDIR=`dirname $0` || exit
#MYDIR=/usr/share/automake-1.7
#echo "  I'm located in $MYDIR" 1>&2

TIMESTAMP=`$MYDIR/config.guess --time-stamp` || exit
echo "  Using config.guess of $TIMESTAMP..." 1>&2

GUESSED_CONFIG=`$MYDIR/config.guess` || exit
echo "  Detected system: $GUESSED_CONFIG" 1>&2

case "$GUESSED_CONFIG" in
	*-*-*-*)
		GUESSED_CPU=${GUESSED_CONFIG%-*-*-*}
		GUESSED_OS=${GUESSED_CONFIG#*-*-}
		;;
	*-*-*)
		GUESSED_CPU=${GUESSED_CONFIG%-*-*}
		GUESSED_OS=${GUESSED_CONFIG#*-*-}
		;;
	*)
		echo "  Unknown format!" 1>&2
		exit 1
		;;
esac
#echo "  CPU: ${GUESSED_CPU}" 1>&2
#echo "  OS:  ${GUESSED_OS}" 1>&2

case "$GUESSED_CPU" in
	i?86)
		CATAPULT_TARGET_CPU=x86;;
	x86_64)
		CATAPULT_TARGET_CPU=x86_64;;
	powerpc)
		CATAPULT_TARGET_CPU=ppc;;
	*)
		echo "  Unknown CPU \"$GUESSED_CPU\"!" 1>&2
		exit 1
		;;
esac
case "$GUESSED_OS" in
	*linux*)
		CATAPULT_TARGET_OS=linux;;
	*darwin*)
		CATAPULT_TARGET_OS=darwin;;
	*freebsd4*)
		CATAPULT_TARGET_OS=freebsd4;;
	*freebsd5*)
		CATAPULT_TARGET_OS=freebsd5;;
	*netbsd*)
		CATAPULT_TARGET_OS=netbsd;;
	*mingw*)
		CATAPULT_TARGET_OS=mingw32;;
	*)
		echo "  Unknown OS \"$GUESSED_OS\"!" 1>&2
		exit 1
		;;
esac

echo "CATAPULT_TARGET_CPU=$CATAPULT_TARGET_CPU"
echo "CATAPULT_TARGET_OS=$CATAPULT_TARGET_OS"
