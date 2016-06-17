#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
benchmark.py
------------

'''

from .utils import GetOutput
import subprocess
from functools import reduce
import numpy as np

def deep_getattr(obj, attr):
  '''
  Recurses through an attribute chain to get the ultimate value.
  See `this page <http://pingfive.typepad.com/blog/2010/04/deep-getattr-python-function.htmls>`_.
  '''
  
  return reduce(getattr, attr.split('.'), obj)
 
def isclose(val1, val2, tol):
  '''
  Checks whether two floats are the same within a certain tolerance.
  
  '''
  
  return np.abs(1. - val1 / val2) <= tol

def Benchmark(compare, tolerance = 1.e-5, infile = 'vpl.in'):
  '''
  
  '''
  
  subprocess.call(['vplanet', infile, '-q'])

  log1 = GetOutput(benchmark = True).log
  log2 = GetOutput().log

  for attr in compare:
    val1 = deep_getattr(log1, attr)
    val2 = deep_getattr(log2, attr)
    assert isclose(val1, val2, tolerance), "ERROR: %s different from benchmark value." % attr