#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
import os
import glob
srcdir = os.path.join(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))), 'examples')
srcfiles = [x for x in glob.glob(os.path.join(srcdir, '*', 'README.rst'))]

rsttext = '''Examples
========

A collection of example input files and scripts using **vplanet** that
showcase the various applications of the code.

.. toctree::
   :maxdepth: 1


'''

# Create rst file
if not os.path.exists('examples'):
    os.makedirs('examples')

with open('examples.rst', 'w') as index:
    print(rsttext, file=index)
    for filename in srcfiles:
        shortname = filename.split(os.sep)[-2]
        with open(os.path.join('examples', shortname + '.rst'), 'w') as incl:
            print(".. include:: ../../examples/%s/README.rst" % shortname,
                  file=incl)
        print('   examples/%s' % shortname, file=index)
