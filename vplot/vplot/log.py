#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
log.py
------

'''

import re
import os

class ufloat(float):
  '''
  Simple subclass to add a ``unit`` attribute to a float
  object. Note that the unit does **not** propagate through
  when operations are done to ``ufloat``. I'd have to
  implement ``__mult__``, ``__sum__``, etc. methods for that.
  '''
  
  def __new__(self, value, unit = ''):
    return float.__new__(self, value)
    
  def __init__(self, value, unit = ''):
    float.__init__(value)
    self.unit = unit
  
  def __repr__(self):
    return '%s %s' % (str(self), self.unit)
  
def Unitify(param):
  '''
  
  '''
  
  unit = re.search('\[(.*?)\]', param)
  if unit:
    unit = unit.groups()[0]
  else:
    unit = ''
  return unit
  
def Variablify(param):
  '''
  
  '''
  
  repl = [(' ', '_'),
          ('#', '')]
  for a, b in repl:
    param = param.replace(a, b)
  
  par = re.search('\((.*?)\)', param)
  if par:
    param = par.groups()[0]
  
  return param

def Quantify(val, unit):
  '''
  
  '''
  
  val = val.lstrip().rstrip()
  int_chars = ['0', '1', '2', '3', '4', 
               '5', '6', '7', '8', '9', '-']
  float_chars = ['0', '1', '2', '3', '4', 
                 '5', '6', '7', '8', '9', 
                 '.', '-', '+', 'e']
  if all([c in int_chars for c in val]):
    try:
      val = int(val)
    except:
      pass
  elif all([c in float_chars for c in val]):
    try:
      val = ufloat(val, unit)
    except:
      pass
  elif (val == 'True') or (val == 'Yes'):
    val = True
  elif (val == 'False') or (val == 'No'):
    val = False
  return val

class Log(object):
  '''
  
  '''
  
  def __init__(self, sysname = ""):
    self.sysname = sysname
    self.header = LogStage('Header')
    self.initial = LogStage('Initial')
    self.final = LogStage('Final')
  
  def __repr__(self):
    return "<VPLOT Log File: %s>" % self.sysname

class LogStage(object):
  '''
  
  '''
  
  def __init__(self, name = 'Header'):
    self._name = name
    self._bodies = []
    
  def __getitem__(self, i):
    return self._bodies[i]
  
  def __len__(self):
    return len(self._bodies)
  
  def __repr__(self):
    return "<VPLOT Log Object: %s>" % self._name

class LogBody(object):
  '''
  
  '''
  
  def __init__(self, name = ''):
    self._name = name
    
  def __repr__(self):
    return "<VPLOT Log Object: %s>" % self._name

def GetLog(sysname = '', path = '.', benchmark = False, logfile = None, **kwargs):
  '''
  
  '''
  
  # Is this a benchmarking run?
  if benchmark:
    logext = '.log.truth'
  else:
    logext = '.log'
    
  # Get the log file
  lf = [f for f in os.listdir(path) if f.endswith(logext)]
  if len(lf) > 1:
    lf = logfile
    if lf is None:
      raise Exception("There's more than one log file in the cwd! VPLOT is confused.")
  elif len(lf) == 0:
    raise Exception("There doesn't seem to be a log file in this directory.")
  else:
    lf = os.path.join(path, lf[0])
  with open(lf, 'r') as f:
    tmp = f.readlines()

  # Remove newlines and blank lines
  header = []
  initial = []
  final = []
  stage = 0
  for line in tmp:
    line = line.replace('\n', '')
    if 'INITIAL SYSTEM PROPERTIES' in line:
      stage = 1
    elif 'FINAL SYSTEM PROPERTIES' in line:
      stage = 2
    if len(line):
      if stage == 0:
        if ('Log file' not in line) and ('FORMATTING' not in line):
          header.append(line)
      elif stage == 1:
        if ('INITIAL SYSTEM PROPERTIES' not in line) and ('PARAMETERS' not in line):
          initial.append(line)
      elif stage == 2:
        final.append(line)

  log = Log('xuvevol')

  for line in header:
    param, val = line.split(': ')
    unit = Unitify(param)
    param = Variablify(param)
    val = Quantify(val, unit)
    setattr(log.header, param, val)

  body = 'system'
  setattr(log.initial, body, LogBody(body))
  for line in initial:
    if 'BODY:' not in line:
      try:
        param, val = line.split(': ')
      except:
        continue
      unit = Unitify(param)
      param = Variablify(param)
      val = Quantify(val, unit)
      setattr(getattr(log.initial, body), param, val)
    else:
      try:
        match = re.search('BODY:\s(.*?)\s-', line)
        body = match.groups()[0]
      except:
        raise ValueError("Can't understand body name in: %s." % line)
      setattr(log.initial, body, LogBody(body))

  body = 'system'
  setattr(log.final, body, LogBody(body))
  for line in final:
    if 'BODY:' not in line:
      try:
        param, val = line.split(': ')
      except:
        continue
      unit = Unitify(param)
      param = Variablify(param)
      val = Quantify(val, unit)
      setattr(getattr(log.final, body), param, val)
    else:
      try:
        match = re.search('BODY:\s(.*?)\s-', line)
        body = match.groups()[0]
      except:
        raise ValueError("Can't understand body name in: %s." % line)
      setattr(log.final, body, LogBody(body))
  
  return log