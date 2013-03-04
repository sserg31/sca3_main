#!/bin/sh

CUR=`pwd`
TMPPATH=/tmp/dskfnd_build

rm -rf $TMPPATH
mkdir -p $TMPPATH
git checkout-index -a -f --prefix=$TMPPATH/
cd $TMPPATH/sources/sca31_src/diskfind2
dpkg-buildpackage -rfakeroot -b
mv $TMPPATH/sources/sca31_src/diskfind*.deb $CUR/..

