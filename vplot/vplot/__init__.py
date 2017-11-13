#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import, unicode_literals

__version__ = "0.2.1"
__author__ = "Rodrigo Luger (rodluger@uw.edu)"
__copyright__ = "Copyright 2016 Rodrigo Luger"

# Was vplot imported from setup.py?
try:
  __VPLOT_SETUP__
except NameError:
  __VPLOT_SETUP__ = False

# This is a regular vplot run
if not __VPLOT_SETUP__:

  # Set up the matplotlib stylesheet
  import os
  import matplotlib.pyplot as pl
  pl.style.use(os.path.join(os.path.dirname(
               os.path.abspath(__file__)),
               'vplot.mplstyle'))

  # Force matplotlib to use Palatino Linotype
  import sys, warnings
  from matplotlib.font_manager import findfont
  with warnings.catch_warnings(record=True) as w:
    warnings.simplefilter("always")
    findfont('Palatino Linotype')
    if len(w):
      print(w[-1].message)
      print("Unable to locate font 'Palatino Linotype'. Please contact")
      print("Rodrigo Luger (rodluger@uw.edu) for help installing it.")

  from . import plot, quickplot, utils, log
  from .utils import GetOutput
  from .plot import plot, show, savefig, colors, make_pretty
