#!/bin/bash

OPERANDS=0

for f in $@ ; do
	echo "Inspecting  " $f
	cat $FILE | grep "C++ sort algorithm computed" >> cppTimes
done

OPERANDS=$(wc -l cppTimes)

sed -e 's/.*usec  (\([0-9]\+\).*/\1/' cppTimes > seddedTimes
cat seddedTimes
#sums=$(awk '{ sum += $1 } END { print sum }' seddedTimes)

#echo -e "-> sums\t"$sums "-->  "$(( $sums / $ITERATIONS ))