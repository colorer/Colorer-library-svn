#!/bin/bash

# Self runnable version of Colorer library JNI interface
# Uses SWT API.

# To use it, please provide full path to colorer/catalog.xml file
# in %HOME%/.colorer5catalog  file.

ECLIPSE_HOME=/opt/eclipse
ECLIPSE_VERSION=2.1.0
# Requires inclusion of additional jars:
#ECLIPSE_TOOLKIT=gtk
ECLIPSE_TOOLKIT=motif 

OS=linux
ARCH=x86

SWT_JAR=${ECLIPSE_HOME}/plugins/org.eclipse.swt.${ECLIPSE_TOOLKIT}_${ECLIPSE_VERSION}/ws/${ECLIPSE_TOOLKIT}/swt.jar
SWT_LIB=${ECLIPSE_HOME}/plugins/org.eclipse.swt.${ECLIPSE_TOOLKIT}_${ECLIPSE_VERSION}/os/${OS}/${ARCH}

env LD_LIBRARY_PATH=os/${OS}/${ARCH}:${SWT_LIB}:${LD_LIBRARY_PATH} java -classpath colorer.jar:${SWT_JAR} net.sf.colorer.swt.ColorerDemo
