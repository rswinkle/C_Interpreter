#!/usr/bin/env python3

import sys, os, glob

language_tests = glob.glob("../tests/*.txt")

preprocessor_tests = [
		"../tests/preprocessor/preprocessor.txt",
		"../tests/preprocessor/include.c"
		]


ret_array = []

for f in language_tests:
	os.system("./cinterpreter {0} 1> {0}.out 2> {0}.err".format(f))

	rc = os.system("diff {0}.out {0}.expected".format(f))
	ret_array += [rc]

	if os.path.exists("{}.expected.err".format(f)):
		ret_array += [os.system("diff {0}.err {0}.expected.err".format(f))]
	
	if rc or ret_array[-1]:
		print("test {} failed".format(f))



if len(sys.argv) > 1:
	for f in preprocessor_tests:
		os.system("./cinterpreter -E {0} 1> {0}.out 2> {0}.err".format(f))

		rc = os.system("diff {0}.out {0}.expected".format(f))
		ret_array += [rc]

		if os.path.exists("{}.expected.err".format(f)):
			ret_array += [os.system("diff {0}.err {0}.expected.err".format(f))]
		
		if rc or ret_array[-1]:
			print("test {} failed".format(f))

if any(ret_array):
	exit(1)

exit(0)




