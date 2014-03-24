#!/bin/bash
RAGEL_INPUT=$1
SRC_FILE=$(basename $RAGEL_INPUT .rl).cpp
OBJ_FILE=$(basename $RAGEL_INPUT .rl).o
CC=avr-g++

$CC --version

for style in T0 T1 F0 F1 G0 G1 G2
do
    ragel -$style -o $SRC_FILE $RAGEL_INPUT && \
    $CC -Os $SRC_FILE -o $OBJ_FILE -c && \
    (ls -lAh $OBJ_FILE | echo "$style: $(awk '{ print $5 }')")
done
