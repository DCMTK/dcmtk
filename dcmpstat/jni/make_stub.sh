#!/bin/sh

JAVA_HOME=/usr/java1.2
JAVA_OPTS="-Xmx128m"
JAVA_JNI_LIB_DIR=.
JAVA_MAINCLASS=DicomTest
OUTFILE=ecr_browser

/usr/java1.2/bin/javald -H $JAVA_HOME -j $JAVA_OPTS -o $OUTFILE -R $JAVA_JNI_LIB_DIR $JAVA_MAINCLASS

