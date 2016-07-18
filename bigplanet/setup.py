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

# Add bigplanet to python path
print("\x1b[01m%s\x1b[39;49;00m" % "Setting up bigplanet...")

# Get home directory
home = os.path.expanduser('~')

# Command we're adding to the user profile
pythonpath = os.path.join(os.path.dirname(os.path.realpath(__file__)))
cmd = '\n\n# Added by bigplanet setup script\nexport PYTHONPATH=${PYTHONPATH}:%s' % (pythonpath)

# Setup success?
success = False

for file in [foo for foo in ['.bash_profile', '.profile', '.bashrc'] if foo in os.listdir(home)]:
  
  with open(os.path.join(home, file), 'r') as f:
    contents = f.read()
  
  # Check if the path is already there  
  if cmd in contents:
    success = True
    break
  
  # We're going to add it
  else:
    with open(os.path.join(home, file), 'a') as f:
      print(cmd, file = f)
    success = True
        
if success == False:
  print("Error adding \x1b[01mvspace\x1b[39;49;00m to your PATH.")
  print("You will have to add `%s` manually." % path)
else:
  print("Success. Navigate to the bigplanet directory and read the README.")
  print("You may need to restart your terminal.")
