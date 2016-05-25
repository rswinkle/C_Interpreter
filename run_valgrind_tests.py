#!/usr/bin/env python

import sys, os, glob

language_tests = glob.glob("../tests/*.txt")

preprocessor_tests = [
		"../tests/preprocessor/preprocessor.txt",
		"../tests/preprocessor/include.c"
		]


ret_array = []

for f in language_tests:
	rc = os.system('valgrind --leak-check=full -v > tmp.txt ./cinterpreter {0} 2>&1'.format(f))
	rc = os.system('grep "ERROR SUMMARY: 0" tmp.txt > /dev/null')
	ret_array += [rc]
	
	if rc:
		print('{: <40}'.format(f.rpartition('/')[-1]), "..... failed")
		os.system('cat tmp.txt')



if len(sys.argv) > 1:
	for f in preprocessor_tests:
		rc = os.system('valgrind --leak-check=full -v ./cinterpreter -E {0} 2>&1 | grep "ERROR SUMMARY: 0" > /dev/null'.format(f))
		ret_array += [rc]

		if rc:
			print('{: <40}'.format(f.rpartition('/')[-1]), "..... failed")

if any(ret_array):
	exit(1)

exit(0)

