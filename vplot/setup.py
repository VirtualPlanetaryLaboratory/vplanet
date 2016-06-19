#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import
from setuptools import setup
from setuptools.command.install import install
import warnings

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

class move_ttf(install):
  '''
  Sets up the machinery to install custom fonts.  Subclasses the setup tools install 
  class in order to run custom commands during installation.  
  
  Taken from http://stackoverflow.com/a/34304823
  
  '''
  
  def run(self):
    '''
    Performs the usual install process and then copies the True Type fonts 
    that come with clearplot into matplotlib's True Type font directory, 
    and deletes the matplotlib fontList.cache 
    
    '''
    
    # Perform the usual install process
    install.run(self)
    
    # Try to install custom fonts
    try:
      import os, shutil
      import matplotlib as mpl

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
      warnings.warn("WARNING: An issue occured while installing the custom fonts for vplot.")

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
      cmdclass={'install' : move_ttf},
      scripts=['bin/vplot', 'bin/vtest'],
      zip_safe = False)