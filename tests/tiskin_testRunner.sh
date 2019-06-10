#!/bin/bash
#sed -e 's/.*usec  (\([0-9]\+\).*/\1/' to take the number

if [ $# != 4 ] ; then
	echo -e "Wrong number of arguments: iterations elements processors seed\n"
	exit
fi

ITERATIONS=$1
ELEMENTS=$2
PROCESSORS=$3
SEED=$4

echo -e "Iterations:\t" $ITERATIONS
echo -e "Elements:\t" $ELEMENTS
echo -e "Processors:\t" $PROCESSORS
echo -e "Seed:\t\t" $SEED


for ((i=0; i<$ITERATIONS; i++)) ; do
	echo -e "==============================\n\tI:  "$i"\n=============================="
	./bin/TiskinAlgorithm $ELEMENTS $PROCESSORS -s $SEED >> tiskinTimes
	sleep 2
done


cat tiskinTimes | grep Whole > wholeTimes
sed -e 's/.*usec  (\([0-9]\+\).*/\1/' wholeTimes > seddedTimes
cat seddedTimes
sums=$(awk '{ sum += $1 } END { print sum }' seddedTimes)


### ========================================
### ========================================
### ========================================


echo -e "sums\t"$sums "-->  "$(( $sums / $ITERATIONS ))

rm tiskinTimes wholeTimes 