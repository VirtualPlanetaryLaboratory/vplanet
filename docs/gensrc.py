#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
import os
import glob
srcdir = os.path.join(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))), 'src')
srcfiles = [os.path.basename(x)
            for x in glob.glob(os.path.join(srcdir, '*.[ch]'))]

rsttext = '''C API
=====

Detailed documentation of C source code.

.. toctree::
   :maxdepth: 3


'''

# DEBUG
srcfiles = []

# Create rst file
if not os.path.exists('src'):
    os.makedirs('src')

# Loop through each source file
for srcfile in srcfiles:

    # Get module name
    name = os.path.splitext(srcfile)[0]

    # Add to the table of contents
    rsttext += '   src/%s\n' % srcfile

    # Do we need to generate this file?
    outfile = os.path.join('src', '%s.rst' % srcfile)
    if os.path.exists(outfile):
        continue

    with open(outfile, 'w') as rstfile:

        # Create the RST file
        header = srcfile + '\n' + '=' * len(srcfile)
        print("%s\n.. autodoxygenfile:: %s" % (header, srcfile), file=rstfile)

# Create RST index
with open('src.rst', 'w') as index:
    print(rsttext, file=index)
