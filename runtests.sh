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
	echo "executing $f"
	./cinterpreter $f > $f.out

	# for if I ever update expected output
	#with stderr
	#./cinterpreter $f &> $f.out
done

for f in ../tests/*.txt.expected
do
	short=${f/.expected}
	#echo "$f     $short.out"
	
	echo "comparing with $f"
	diff $f $short.out
	#echo "filename = $f"
done
