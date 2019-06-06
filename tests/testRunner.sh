#!/bin/bash
#sed -e 's/.*usec  (\([0-9]\+\).*/\1/' to take the number

if [ $# != 6 ] ; then
	echo -e "Rong number of arguments: iterations elements p q compDelay commDelay\n"
	exit
fi

ITERATIONS=$1
ELEMENTS=$2
P=$3
Q=$4
COMP_DELAY=$5
COMM_DELAY=$6

R=$(( $P*$Q ))

echo -e "Iterations:\t" $ITERATIONS
echo -e "Elements:\t" $ELEMENTS
echo -e "P:\t\t" $P
echo -e "Q:\t\t" $Q
echo -e "P*Q:\t\t" $R
echo -e "Computation:\t" $COMP_DELAY
echo -e "Communication:\t" $COMM_DELAY

rm mTimes
rm msTimes


echo -e "==============================\n==============================\n\tPerfEval_M\n==============================\n==============================\n\n"
touch mTimes
for ((i=0; i<$ITERATIONS; i++)) ; do
	echo -e "==============================\n\tI:  "$i"\n=============================="
	./bin/PerfEval_M $ELEMENTS $(( $P*$Q )) $COMP_DELAY $COMM_DELAY >> mTimes
	sleep 2
done

cat mTimes | grep whole > wholeMTimes
sed -e 's/.*usec  (\([0-9]\+\).*/\1/' wholeMTimes > seddedMTimes
cat seddedMTimes
Msums=$(awk '{ sum += $1 } END { print sum }' seddedMTimes)


### ========================================
### ========================================
### ========================================


echo -e "\n\n\n==============================\n==============================\n\tPerfEval_MS\n==============================\n==============================\n\n"
touch msTimes
for ((i=0; i<$ITERATIONS; i++)) ; do
	echo -e "==============================\n\tI:  "$i"\n=============================="
	./bin/PerfEval_MS $ELEMENTS $P $Q $COMP_DELAY $COMM_DELAY >> msTimes
	sleep 2
done
cat msTimes | grep whole > wholeMsTimes
sed -e 's/.*usec  (\([0-9]\+\).*/\1/' wholeMsTimes > seddedMsTimes
cat seddedMsTimes
MSsums=$(awk '{ sum += $1 } END { print sum }' seddedMsTimes)


### ========================================
### ========================================
### ========================================


echo -e "M sums\t"$Msums "-->  "$(( $Msums / $ITERATIONS ))
echo -e "MS sums\t"$MSsums "-->  " $(( $MSsums / $ITERATIONS ))

rm mTimes msTimes sedded*Times whole*Times