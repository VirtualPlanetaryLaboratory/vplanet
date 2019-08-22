#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
import os
import glob
srcdir = os.path.join(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__))), 'bugs')
srcfiles = [x for x in glob.glob(os.path.join(srcdir, '*', 'README.rst'))]

rsttext = '''Bugs
====

Scripts to reproduce current major bugs in **VPLanet**.

.. toctree::
   :maxdepth: 1


'''

# Create rst file
if not os.path.exists('bugs'):
    os.makedirs('bugs')

with open('bugs.rst', 'w') as index:
    print(rsttext, file=index)
    for filename in srcfiles:
        shortname = filename.split(os.sep)[-2]
        with open(os.path.join('bugs', shortname + '.rst'), 'w') as incl:
            print(".. include:: ../../bugs/%s/README.rst" % shortname,
                  file=incl)
        print('   bugs/%s' % shortname, file=index)
