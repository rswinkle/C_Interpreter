#!/usr/bin/python3

# Copyright (c) 2016 Robert Winkler
# 
# This software is MIT licensed see link for details
# http://www.opensource.org/licenses/MIT

import glob, markdown, argparse, time
import itertools








def main():
	#in case I decide to use extensions or change output type etc.
	md = markdown.Markdown(output_format="html5", extensions=['markdown.extensions.tables']);
	open("index.html", "w").write(md.convert(open("README.md").read()));




if __name__=="__main__":
	main()


