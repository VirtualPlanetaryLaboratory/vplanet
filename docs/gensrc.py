#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
import os
import glob
srcdir = os.path.join(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))), 'src')
cfiles = [os.path.basename(x)[:-2]
          for x in glob.glob(os.path.join(srcdir, '*.c'))]
hfiles = [os.path.basename(x)[:-2]
          for x in glob.glob(os.path.join(srcdir, '*.h'))]
modules = [x for x in cfiles if x in hfiles]
cmodules = [x for x in hfiles if x not in cfiles]
hmodules = [x for x in cfiles if x not in hfiles]

rsttext = '''C API
=====

Detailed documentation of C source code.

.. toctree::
   :maxdepth: 1


'''

# Create rst file
if not os.path.exists('src'):
    os.makedirs('src')

# Loop through each module
for name in modules:

    # Add to the table of contents
    rsttext += '   src/%s\n' % name

    # Do we need to generate this file?
    outfile = os.path.join('src', '%s.rst' % name)
    if os.path.exists(outfile):
        continue

    with open(outfile, 'w') as rstfile:

        # Create the RST file
        header = name + '\n' + '=' * len(name)
        hheader = name + ".h" + '\n' + '-' * (len(name) + 2)
        cheader = name + ".c" + '\n' + '-' * (len(name) + 2)
        print(header, file=rstfile)
        print("", file=rstfile)
        print("**Contents**\n", file=rstfile)
        print(".. contents:: :local:\n", file=rstfile)
        print("%s\n.. autodoxygenfile:: %s" % (cheader, name + ".c"),
              file=rstfile)
        print("", file=rstfile)
        print("%s\n.. autodoxygenfile:: %s" % (hheader, name + ".h"),
              file=rstfile)

# Create RST index
with open('src.rst', 'w') as index:
    print(rsttext, file=index)
