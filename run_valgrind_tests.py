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

	ret = 0
	for f in language_tests:
		proc = Popen(["valgrind", "--leak-check=full", "-v", "./cinterpreter", f], stdout=PIPE, stderr=PIPE, universal_newlines=True)
		out, err = proc.communicate()

		# sometimes valgrind has too many false positives, so use this line instead of
		# the following one to only look for memory that isn't freed
		# ie a potential leak depending on whether I still had a pointer to it at the end
		# rc = int(err.find('in use at exit: 0 bytes in 0 blocks') == -1)

		rc = int(err.find('ERROR SUMMARY: 0') == -1)
		
		# I don't really care if there's a "leak" in a program that had an error
		# since I exit immediately without freeing everything.  It would be nice to clean up
		# everything under all conditions but as long as everything is freed properly when
		# a program runs to completion
		if rc and (not os.path.exists("{}.expected.err".format(f)) or open("{}.expected.err".format(f)).read().find('Error') == -1):
			print('{: <40} ..... failed'.format(f.rpartition('/')[-1]))
			print("stdout:")
			print(out)
			ret = rc



	if len(sys.argv) > 1:
		for f in preprocessor_tests:
			proc = Popen(["valgrind", "--leak-check=full", "-v", "./cinterpreter", "-E", f], stdout=PIPE, stderr=PIPE, universal_newlines=True)
			out, err = proc.communicate()

			rc = int(err.find('ERROR SUMMARY: 0') == -1)
			
			if rc:
				print('{: <40} ..... failed'.format(f.rpartition('/')[-1]))
				print("stdout:")
				print(out)
				ret = rc

	exit(ret)



if __name__ == "__main__":
	main()
