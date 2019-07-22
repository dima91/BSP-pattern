#!/bin/bash

if [ $# != 1 ] ; then
	echo "usage: extractor.sh <filename>"
	exit
fi

if [ -f tmpValsExtractor ] ; then 
	rm tmpValsExtractor
fi

grep "Final res" $1 | grep -Eo '[0-9\.]+' > tmpValsExtractor
SUM=$(awk '{ sum += $1 } END { print sum }' tmpValsExtractor )

echo $SUM
echo $(($SUM/5))


rm tmpValsExtractor