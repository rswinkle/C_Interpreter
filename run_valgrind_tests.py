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
	print(args)

	ret = 0
	for f in language_tests:
		proc = Popen(["valgrind", "--leak-check=full", "-v", "./cinterpreter", f], stdout=PIPE, stderr=PIPE, universal_newlines=True)
		out, err = proc.communicate()

		rc = int(err.find('ERROR SUMMARY: 0') == -1)
		
		if rc:
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
