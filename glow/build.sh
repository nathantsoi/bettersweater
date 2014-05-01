#!/bin/bash
HEADER_FILE=pattern_loader.h
echo "#ifndef pattern_loader_h"
echo "#define pattern_loader_h"
for file in patterns/*.h
do
  echo "#include \"$file\"" >> $HEADER
done
echo "#endif pattern_loader_h"

