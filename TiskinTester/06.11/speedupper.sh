#!/bin/bash


for f in $@ ; do
	echo "Inspecting  " $f
	cat $FILE | grep Whole > wholeTimes
	sed -e 's/.*usec  (\([0-9]\+\).*/\1/' wholeTimes > seddedTimes
	cat seddedTimes
	sums=$(awk '{ sum += $1 } END { print sum }' seddedTimes)

	echo -e "-> sums\t"$sums "-->  "$(( $sums / $ITERATIONS ))
done