#!/bin/bash

echo $PWD > tempfile0
sed '1s/\/src/"/' tempfile0 > tempfile1
sed '1s/^/"/' tempfile1 > tempfile0
sed '1s/^/\#define PLOTTING_ROUTINES_DIR /' tempfile0 > tempfile2
ls -l ../include/ | grep -v orig | grep "\.h" > tempfile3
awk '{print $9;}' tempfile3 > tempfile4
INCLUDE_FILENAME=`cat tempfile4`
# echo "New variable INCLUDE_FILENAME is \"${INCLUDE_FILENAME}\"."
# cp -p ../include/${INCLUDE_FILENAME} ../include/orig_${INCLUDE_FILENAME}
sed '/PLOTTING_ROUTINES_DIR/d' ../include/${INCLUDE_FILENAME} > tempfile5
cat tempfile5 tempfile2  > tempfile6
mv tempfile6 ../include/${INCLUDE_FILENAME}
# tail -3 ../include/${INCLUDE_FILENAME}
rm tempfile0 tempfile1 tempfile2 tempfile3 tempfile4 tempfile5
