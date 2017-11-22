#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
from setuptools import setup
from setuptools.command.install import install
from setuptools.command.develop import develop
import warnings
import os, shutil
import subprocess
import matplotlib as mpl

# Font config file
fontconfig = \
'''<?xml version="1.0"?>
<!DOCTYPE fontconfig SYSTEM "fonts.dtd">
<!-- ~/.fonts.conf for per-user font configuration -->
<fontconfig>
<dir>~/.fonts</dir>
</fontconfig>'''

# Hackishly inject a constant into builtins to enable importing of the
# module. Stolen from `kplr`
import sys
if sys.version_info[0] < 3:
  import __builtin__ as builtins
else:
  import builtins
builtins.__VPLOT_SETUP__ = True
import vplot

# Get the long description from the README
def readme():
  with open('README.rst') as f:
    return f.read()

def InstallFont():
    '''

    '''

    # Try to install custom fonts in matplotlib source directory
    try:

      # Find where matplotlib stores its True Type fonts
      mpl_data_dir = os.path.dirname(mpl.matplotlib_fname())
      mpl_ttf_dir = os.path.join(mpl_data_dir, 'fonts', 'ttf')

      # Copy the font files to matplotlib's True Type font directory
      # (I originally tried to move the font files instead of copy them,
      # but it did not seem to work, so I gave up.)
      cp_ttf_dir = os.path.join(os.path.dirname(vplot.__file__), 'fonts')
      for file_name in os.listdir(cp_ttf_dir):
        if file_name[-4:] == '.ttf':
          old_path = os.path.join(cp_ttf_dir, file_name)
          new_path = os.path.join(mpl_ttf_dir, file_name)
          shutil.copyfile(old_path, new_path)
          print("Copying " + old_path + " -> " + new_path)

      # Try to delete matplotlib's fontList cache
      mpl_cache_dir = mpl.get_cachedir()
      mpl_cache_dir_ls = os.listdir(mpl_cache_dir)
      for file in mpl_cache_dir_ls:
        if file.startswith('fontList') and file.endswith('.cache'):
          fontList_path = os.path.join(mpl_cache_dir, file)
          os.remove(fontList_path)
          print("Deleted %s." % fontList_path)

    except:
      # Install them locally
      # https://gist.github.com/pastewka/2293b6ec8998fc36c684
      if not os.path.exists(os.path.join(os.path.expanduser('~'), '.fonts')):
          os.makedirs(os.path.join(os.path.expanduser('~'), '.fonts'))
      cp_ttf_dir = os.path.join(os.path.dirname(vplot.__file__), 'fonts')
      for file_name in os.listdir(cp_ttf_dir):
        if file_name[-4:] == '.ttf':
          old_path = os.path.join(cp_ttf_dir, file_name)
          new_path = os.path.join(os.path.expanduser('~'), '.fonts', file_name)
          shutil.copyfile(old_path, new_path)
          print("Copying " + old_path + " -> " + new_path)
      # Tell matplotlib where to find them
      dir = os.path.join(os.path.expanduser('~'), '.config', 'fontconfig')
      file = os.path.join(dir, 'fonts.conf')
      if not os.path.exists(dir):
          os.makedirs(dir)
      if os.path.exists(file):
          print("Unable to install Palatino Linotype font.")
          print("You may have to do it manually.")
      else:
          # Create the font config file
          with open(file, 'w') as f:
              print(fontconfig, file = f)
          # Clear the cache
          print("Clearing the matplotlib cache. This might take a minute or two...")
          subprocess.call(["fc-cache", "--force"])
          shutil.rmtree(mpl.get_cachedir())

class move_ttf_install(install):
  '''
  Sets up the machinery to install custom fonts.  Subclasses the setup tools install
  class in order to run custom commands during installation.

  Taken from http://stackoverflow.com/a/34304823

  '''

  def run(self):
    '''
    Performs the usual install process and then copies the True Type fonts
    into matplotlib's True Type font directory, and deletes the matplotlib
    fontList.cache

    '''

    # Perform the usual install process
    install.run(self)

    # Install the font
    InstallFont()

class move_ttf_develop(develop):
  '''
  Sets up the machinery to install custom fonts.  Subclasses the setup tools
  develop class in order to run custom commands during installation.

  Taken from http://stackoverflow.com/a/34304823

  '''

  def run(self):
    '''
    Performs the usual install process and then copies the True Type fonts
    into matplotlib's True Type font directory, and deletes the matplotlib
    fontList.cache

    '''

    # Perform the usual install process
    develop.run(self)

    # Install the font
    InstallFont()

# Setup!
setup(name = 'vplot',
      version = vplot.__version__,
      description = 'VPLANET Plotting Tools',
      long_description = readme(),
      classifiers = [
                      'Development Status :: 3 - Alpha',
                      'License :: OSI Approved :: MIT License',
                      'Programming Language :: Python',
                      'Programming Language :: Python :: 3',
                      'Topic :: Scientific/Engineering :: Astronomy',
                    ],
      url = 'https://bitbucket.org/bitbucket_vpl/vplanet/vplot/',
      author = 'Rodrigo Luger',
      author_email = 'rodluger@uw.edu',
      license = 'MIT',
      packages = ['vplot'],
      install_requires = [
                          'numpy',
                          'matplotlib >= 1.4.0'
                         ],
      include_package_data = True,
      package_data = {'vplot' : ['fonts/*.ttf']},
      cmdclass={'install' : move_ttf_install, 'develop': move_ttf_develop},
      scripts=['bin/vplot'],
      zip_safe = False)
