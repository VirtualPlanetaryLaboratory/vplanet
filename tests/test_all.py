#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
test_all.py
-----------

'''

from __future__ import division, print_function, absolute_import
from vplot.utils import GetOutput
import subprocess
from functools import reduce
import numpy as np
import imp
import subprocess
import os, sys
import glob
import threading
import time
import re

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

    if (val1 == 0) and (val2 == 0):
        return True
    else:
        try:
            res = np.abs(1. - val1 / val2) <= tol
        except:
            res = False
        return res

def run(name, compare, tolerance, path, maxtime = 100.):
    '''

    '''

    # Remove old runs
    for file in os.listdir(path):
        if file.endswith('.forward') or file.endswith('.log'):
            os.remove(os.path.join(path, file))

    # Run vplanet for maxtime. If it takes longer than that, kill it
    os.chdir(path)
    status = {'timeout': False}
    proc = subprocess.Popen(['vplanet', 'vpl.in', '-q'],
                            stdout = sys.stdout, stderr = sys.stderr)
    timer = threading.Timer(maxtime, on_timeout, (proc, status))
    timer.start()
    proc.wait()
    timer.cancel()
    if status['timeout']:
        raise Exception('Test ``%s`` exceeded maximum time. Please '
                        'limit all test runs to less than %.1f seconds.'
                        % (name, maxtime))

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

def test_all():
    '''

    '''

    # Get all tests in current directory
    test_dir = os.path.dirname(os.path.abspath(__file__))
    tests = glob.glob(os.path.join(test_dir, '*', 'test.py'))

    # Run through each of the tests
    nerr = 0
    for t in tests:
        name = t.split(os.sep)[-2]
        print("Testing %s..." % name)
        test = imp.load_source('test', t)
        try:
            test.tolerance
        except NameError:
            test.tolerance = 1.e-5
        try:
            test.params
        except NameError:
            test.params = []
        nerr += run(name, test.params, test.tolerance, os.path.dirname(t))
