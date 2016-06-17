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
import imp
import subprocess
import os

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

def TestRun(name, compare, tolerance, path):
  '''
  
  '''
  
  # Remove old runs
  for file in os.listdir(path):
    if file.endswith('.forward') or file.endswith('.log'):
      os.remove(os.path.join(path, file))
  
  # Run vplanet
  os.chdir(path)
  subprocess.call(['vplanet', 'vpl.in', '-q'])

  # Get outputs
  log1 = GetOutput(path = path, benchmark = True).log
  log2 = GetOutput(path = path).log

  # Compare
  nerr = 0
  for attr in compare:
    val1 = deep_getattr(log1, attr)
    val2 = deep_getattr(log2, attr)
    if not isclose(val1, val2, tolerance):
      print("[%s] ERROR: %s (%.5e != %.5e)" % (name, attr, val2, val1))
      nerr += 1
  
  return nerr
  
def TestAll():
  '''
  
  '''
  
  # Get VPLANET path
  try:
    vplanet_dir = os.path.dirname(subprocess.check_output(['which', 'vplanet']).decode('utf-8'))
  except:
    raise Exception("Unable to locate vplanet executable. Please make sure it is in your $PATH.")
  
  # Compile VPLANET
  subprocess.call(['make', '-C', vplanet_dir, '-s'])
  
  # Run through each of the tests  
  nerr = 0
  folders = os.listdir(os.path.join(vplanet_dir, 'testing'))
  folders = [f for f in folders if os.path.exists(os.path.join(vplanet_dir, 'testing', f, 'test.py'))]
  for f in folders:
    testfile = os.path.join(vplanet_dir, 'testing', f, 'test.py')
    test = imp.load_source('test', testfile)
    try:
      test.tolerance
    except NameError:
      test.tolerance = 1.e-5
    try:
      test.params
    except NameError:
      test.params = []
    nerr += TestRun(f, test.params, test.tolerance, os.path.join(vplanet_dir, 'testing', f))
  
  # Tally
  if nerr == 0:
    print("SUCCESS: All tests passed.")
  elif nerr == 1:
    print("FAILURE: There was 1 error.")
  else:
    print("FAILURE: There were %d errors." % nerr)