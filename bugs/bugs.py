import glob
import imp
import os
import sys
import shutil

# Colorful output
BUGGED = '\033[0;31mBUGGED\033[0m'
FIXED = '\033[0;32mFIXED\033[0m'

# Collect all bugs
bugs = glob.glob('*/bug.py')

# Test each one
for bug in bugs:
    folder = os.path.dirname(bug)
    os.chdir(folder)
    sys.stdout.write("%s: " % folder)
    try:
        imp.load_source('bug', 'bug.py')
        print(FIXED)
        infile = open("README.rst")
        line = infile.readline()
        line = "✅" + line[1:]
        outfile = open("README.rst", mode="w")
        outfile.write(line)
        shutil.copyfileobj(infile, outfile)
    except AssertionError:
        print(BUGGED)
        infile = open("README.rst")
        line = infile.readline()
        line = "❌" + line[1:]
        outfile = open("README.rst", mode="w")
        outfile.write(line)
        shutil.copyfileobj(infile, outfile)

    os.chdir('..')
