#!/usr/bin python
####!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division, print_function, absolute_import, unicode_literals

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
    print("Unable to locate font 'Palatino Linotype'. Please install this")
    print("font, which is located in a '.zip' file in the VPLOT directory.")
    print("Take a look at the README in the '.zip' file for details.")

from . import plot, quickplot, utils
from .utils import GetOutput
from .plot import plot, show, savefig

__version__ = "0.1.1"
__author__ = "Rodrigo Luger (rodluger@uw.edu)"
__copyright__ = "Copyright 2015 Rodrigo Luger"

