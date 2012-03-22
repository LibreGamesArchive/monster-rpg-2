#!/bin/bash

DISTRO=ubuntu
while getopts “hv:d:” OPTION
do
	case "$OPTION" in
		d) DISTRO=$OPTARG
			;;
		v) VERSION=$OPTARG
			;;
		h) echo "Usage: $0 [-h] [-d <distro-name>] -v <version>"
			echo "  -d  pick which distro you want to update the version for"
			echo "        options are debian and ubuntu, default is ubuntu"
			echo "  -v  set the version to <verstion>"
			echo "  -h  show this help info"
			exit 0
			;;
		:) echo "Option $OPTION requires an argument"
			exit 0
			;;
	esac
done

dch -c $DISTRO/changelog -v $VERSION
dch -c ${DISTRO}-data/changelog -v $VERSION

