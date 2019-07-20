#!/bin/bash

# 2^15: 32768
# 2^20: 1048576
# 2^25: 33554432
# 2^30: 1073741824

#elements_num=(32768 1048576 33554432 1073741824)
elements_exp=(15 20 25 30)
processors_num=(1 2 4 8 16 32 64 128 256 512)

rm -fr integrations_results
mkdir integrations_results

for e in ${elements_exp[*]}
do
	for p in ${processors_num[*]}
	do
		for i in 1 2 3 4 5
		do
			let els=$((2**$e))
			echo $els " --> " $p
			filename="result_$e""_$p"
			./bin/TiskinAlgorithm $els $p -s 100 >> integrations_results/$filename
			sleep 2
		done
	done
done