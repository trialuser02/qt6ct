#!/bin/sh

NAME=qt6ct

MAJOR=`cat src/qt6ct-common/qt6ct.h | grep "#define QT6CT_VERSION_MAJOR" | cut -d " " -f3`
MINOR=`cat src/qt6ct-common/qt6ct.h | grep "#define QT6CT_VERSION_MINOR" | cut -d " " -f3`

VERSION=$MAJOR.$MINOR

TARBALL=$NAME-$VERSION

echo $TARBALL

mkdir -p extras/package/sources
git archive --format=tar --prefix=$TARBALL/ $VERSION | xz > extras/package/sources/$TARBALL.tar.xz
