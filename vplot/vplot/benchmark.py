#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
benchmark.py
------------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
from .utils import GetOutput
import subprocess
from functools import reduce
import numpy as np
import imp
import subprocess
import os, sys
import threading
import time
import re

certificate = \
'''
All tests passed! If you're submitting a pull request, copy and paste
this certificate into the ``Description`` box:

![Commit](https://img.shields.io/badge/commit-%s-lightgrey.svg)
![Branch](https://img.shields.io/badge/branch-%s-lightgrey.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
'''

certificate_bad = \
'''
One ore more tests failed. If you still want to submit a pull request, copy and paste
this certificate into the ``Description`` box:

![Commit](https://img.shields.io/badge/commit-%s-lightgrey.svg)
![Branch](https://img.shields.io/badge/branch-%s-lightgrey.svg)
![Build](https://img.shields.io/badge/build-failing-red.svg)
'''

def on_timeout(proc, status):
  '''
  Kill process on timeout and note as status['timeout']=True
  
  '''
  
  # A container used to pass status back to calling thread
  status['timeout'] = True
  proc.kill()

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

def TestRun(name, compare, tolerance, path, maxtime = 10.):
  '''
  
  '''
  
  # Remove old runs
  for file in os.listdir(path):
    if file.endswith('.forward') or file.endswith('.log'):
      os.remove(os.path.join(path, file))
  
  # Run vplanet for maxtime. If it takes longer than that, kill it
  os.chdir(path)
  status = {'timeout': False}
  proc = subprocess.Popen(['vplanet', 'vpl.in', '-q'], stdout = sys.stdout, stderr = sys.stderr)
  timer = threading.Timer(maxtime, on_timeout, (proc, status))
  timer.start()
  proc.wait()
  timer.cancel()
  if status['timeout']:
    raise Exception('Test ``%s`` exceeded maximum time. Please limit all test runs to less than %.1f seconds.' % (name, maxtime))
  
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
  
  # Generate certificate
  try:
    GIT_DIR = os.path.join(vplanet_dir, '.git')
    branches = subprocess.check_output(['git', '--git-dir', GIT_DIR,
               'branch']).decode('utf-8').replace('\n', '')
    git_branch = re.findall('\*\s([a-zA-Z0-9_]*)', branches)[0]
    git_hash = subprocess.check_output(['git', '--git-dir', GIT_DIR, 'rev-parse', 
               '--verify', '--short', 'HEAD']).decode('utf-8').replace('\n', '')
  except:
    git_branch = '???'
    git_hash = '???'
  
  # Tally
  if nerr == 1:
    print("FAILURE: There was 1 error.")
    print(certificate_bad % (git_hash, git_branch))
  elif nerr > 1:
    print("FAILURE: There were %d errors." % nerr)
    print(certificate_bad % (git_hash, git_branch))
  else:
    print(certificate % (git_hash, git_branch))

