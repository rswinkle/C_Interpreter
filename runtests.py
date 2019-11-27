#!/usr/bin/env python

import sys, string, glob, argparse, os
#import tempfile
from subprocess import *
from os.path import *

language_tests = glob.glob("../tests/*.txt")

preprocessor_tests = [
		"../tests/preprocessor/preprocessor.txt",
		"../tests/preprocessor/include.c"
		]


def main():
	parser = argparse.ArgumentParser(description="Run C_Interpreter tests")
	parser.add_argument("-E", "--run-preproc", action="store_true", help="Run preprocessor tests too")
	args = parser.parse_args()
	#print(args)


	ret = rc1 = rc2 = rc3 = 0
	for f in language_tests:
		proc = Popen(["./cinterpreter", f], stdout=PIPE, stderr=PIPE, universal_newlines=True)

		out, err = proc.communicate()
		
		rc1 = 0
		rc3 = 0
		expected_ret = 0

		rc2 = int(out != open("{}.expected".format(f)).read())

		if os.path.exists('{}.expected.ret'.format(f)):
			expected_ret = int(open('{}.expected.ret'.format(f)).read())
			rc1 = int(proc.returncode != expected_ret)

		if os.path.exists("{}.expected.err".format(f)):
			rc3 = int(err != open("{}.expected.err".format(f)).read())
		elif err and len(err) != 0:
			print("no expected stderr output file but stderr isn't empty")
		
		if rc1 or rc2 or rc3:
			print('{: <40} failed'.format(f.rpartition('/')[-1]))
			print('ret out err = {} {} {}'.format(rc1, rc2, rc3))
			print('returned: {} (expected {})'.format(proc.returncode, expected_ret))
			print('stdout:\n{0}\nstderr:\n{1}'.format(out, err))
			ret = 1



	# TODO fix these to match above
	rc1=rc2=rc3=0
	if args.run_preproc:
		for f in preprocessor_tests:
			proc = Popen(["./cinterpreter", "-E", f], stdout=PIPE, stderr=PIPE, universal_newlines=True)
			out, err = proc.communicate()
			rc1 = proc.returncode

			rc2 += int(out != open("{}.expected".format(f)).read())

			if os.path.exists("{}.expected.err".format(f)):
				rc3 = int(err != open("{}.expected.err".format(f)).read())
			elif err and len(err) != 0:
				print("no expected stderr output file but stderr isn't empty")
			
		if rc1 or rc2 or rc3:
			print('{: <40} ..... failed'.format(f.rpartition('/')[-1]))
			print('stdout:\n{0}\nstderr:\n{1}'.format(out, err))
			ret = 1

	exit(ret)




if __name__ == "__main__":
	main()
