#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
import warnings
import os

try:
  from setuptools import setup
  from setuptools.command.install import install
  setup
except ImportError:
  from distutils.core import setup
  setup

# Get the long description from the README
def readme():
  with open('README.rst') as f:
    return f.read()

# Setup!
setup(name = 'bigplanet',
      version = "0.1",
      description = 'VPLANET Data Analysis Tools',
      long_description = readme(),
      classifiers = [
                      'Development Status :: 3 - Alpha',
                      'License :: OSI Approved :: MIT License',
                      'Programming Language :: Python',
                      'Programming Language :: Python :: 2',
                      'Topic :: Scientific/Engineering :: Astronomy',
                    ],
      url = 'https://bitbucket.org/bitbucket_vpl/vplanet/bigplanet/',
      author = 'David Fleming',
      author_email = 'dflemin3@uw.edu',
      license = 'MIT',
      packages = ['bigplanet'],
      install_requires = [
                          'numpy',
                          'matplotlib >= 1.4.0',
                          'pandas'
                         ],
      include_package_data = True,
      zip_safe = False)
