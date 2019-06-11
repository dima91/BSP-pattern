#!/bin/bash

FILE=$1
ITERATIONS=5

echo -e "Averaging completion time for $FILE"


cat $FILE | grep Whole > wholeTimes
sed -e 's/.*usec  (\([0-9]\+\).*/\1/' wholeTimes > seddedTimes
cat seddedTimes
sums=$(awk '{ sum += $1 } END { print sum }' seddedTimes)

echo -e "-> sums\t"$sums "-->  "$(( $sums / $ITERATIONS ))




rm wholeTimes seddedTimes
