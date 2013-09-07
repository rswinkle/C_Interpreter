#!/bin/bash

#cd tests
regex=*.txt


#echo "regex = $regex"
#FILES


#for f in ./tests/*.txt.expected
#do
#	echo "$f"
	#echo "filename = $f"
#done


for f in ../tests/*.txt
do
	echo "$f"
	./cinterpreter $f > $f.out
	#echo "filename = $f"
done

for f in ../tests/*.txt.expected
do
	short=${f/.expected}
	#echo "$f     $short.out"
	
	echo "$f"
	diff $f $short.out
	#echo "filename = $f"
done
