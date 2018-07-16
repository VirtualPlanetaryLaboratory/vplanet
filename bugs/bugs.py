import glob
import imp
import os
import sys

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
        with open("README.rst", "r") as infile:
            lines = infile.readlines()
        lines[0] = "✅" + lines[0][1:]
        lines[1] = '=' * (len(lines[0]) + 1)
        with open("README.rst", mode="w") as outfile:
            for line in lines:
                outfile.write(line)
    except AssertionError:
        print(BUGGED)
        with open("README.rst", "r") as infile:
            lines = infile.readlines()
        lines[0] = "❌" + lines[0][1:]
        lines[1] = '=' * (len(lines[0]) + 1)
        with open("README.rst", mode="w") as outfile:
            for line in lines:
                outfile.write(line)

    os.chdir('..')
