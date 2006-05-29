#!/bin/sh
export LDFLAGS=-L/home/irusskih/colorer/src/libcolorer
export CPPFLAGS="-I/home/irusskih/colorer/src/shared"
export CFLAGS="-I/home/irusskih/colorer/src/shared"

./configure --prefix=/usr
echo $LDFLAGS $CPPFLAGS $CFLAGS
