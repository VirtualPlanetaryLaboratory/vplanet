#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
utils.py
--------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
from . import defaults
from .log import GetLog
import os
import subprocess
import sys
import numpy as np; np.seterr(divide='ignore')
import re
import imp

# Default color sequence
def_color_seq = ['#c91111', '#e09401', '#13aed5', '#1321d8', '#642197']

# Default string to include in ``vplot_config.py file`` when creating one
confstr = \
"""#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
vplot_config.py
---------------

This is an automatically generated VPLOT config file. You can override the 
defaults in `vplot/default.py` by setting custom values below.

'''
"""

# Help message
helpstr = \
"""
\x1b[1mVPLOT\x1b[0m
-----
\x1b[1musage:\x1b[0m vplot  [-h [OPTION_NAME]] [-b [BODIES [BODIES ...]]] 
              [-x XAXIS] [-y [YAXIS [YAXIS ...]]] [-a ALPHA]

\x1b[1moptional arguments:\x1b[0m
  -h [OPTION_NAME]          Show this help message or the docstring for OPTION_NAME
  -b BODIES [BODIES ...]    Bodies to plot; should match names of .in files in cwd
  -x XAXIS                  Parameter to plot on the x-axis
  -y YAXIS [YAXIS ...]      Parameter(s) to plot on the y-axis
  -a AAXIS                  Parameter to control the alpha (transparancy) axis
  
\x1b[1mversion:\x1b[0m 0.1.1

\x1b[1mvplot_config.py options:\x1b[0m
%s

\x1b[1mType `vplot -h OPTION_NAME` for info on any option\x1b[0m
""" % ', '.join(sorted([k for k in defaults.__dict__.keys() if not k.startswith('_')]))

def RowCol(N):
  '''
  Given an integer ``N``, returns the ideal number of columns and rows 
  to arrange ``N`` subplots on a grid.
  
  :param int N: The number of subplots
  
  :returns: **``(cols, rows)``**, the most aesthetically pleasing number of columns \
  and rows needed to display ``N`` subplots on a grid.
  
  '''
  rows = np.floor(np.sqrt(N)).astype(int)
  while(N % rows != 0):
    rows = rows - 1
  cols = N/rows
  while cols/rows > 2:
    cols = np.ceil(cols/2.).astype(int)
    rows *= 2
  if cols > rows:
    tmp = cols
    cols = rows
    rows = tmp
  return int(cols), int(rows)

class array(np.ndarray):
  '''
  A custom subclass of numpy ndarray with some extra
  attributes.
  
  '''
  
  def __new__(cls, input_array, unit = None, description = None, name = None, parent = None, color = None):
    # Input array is an already formed ndarray instance
    # We first cast to be our class type
    obj = np.asarray(input_array).view(cls)
    # add the new attribute to the created instance
    obj.unit = unit
    obj.description = description
    obj.name = name
    obj.parent = parent
    obj.color = color
    # Finally, we must return the newly created object:
    return obj

  def __array_finalize__(self, obj):
    # see InfoArray.__array_finalize__ for comments
    if obj is None: return
    self.unit = getattr(obj, 'unit', None)
    self.description = getattr(obj, 'description', None)
    self.name = getattr(obj, 'name', None)
    self.parent = getattr(obj, 'parent', None)
    self.color = getattr(obj, 'color', None)
    
  def __array_wrap__(self, out_arr, context = None):
    # Call the parent
    return np.ndarray.__array_wrap__(self, out_arr, context)

def ShowHelp(param = None):
  '''
  
  '''
  if param is not None:
    try:
      docstring = defaults._Docs().__dict__[param]
      print("\x1b[1m%s:\x1b[0m %s. \x1b[1mDefault:\x1b[0m %s" % (param, docstring, str(defaults.__dict__[param])))
    except:
      print("\x1b[1m%s:\x1b[0m No docstring available." % param)
  else:
    print(helpstr)

class Output(object):
  '''
  
  '''
  def __init__(self, sysname = "", pif = "", bodies = None, log = None):
    self.sysname = sysname
    self.pif = pif
    if bodies is None:
      self.bodies = list([])
    else:
      self.bodies = bodies
    self.log = log
    
  def __getitem__(self, i):
    return self.bodies[i]
    
  def __len__(self):
    return len(self.bodies)

  def __repr__(self):
    return "<VPLOT Output: %s>" % self.sysname

class Body(object):
  '''
  
  '''
  def __init__(self, name = "", infile = "", fwfile = "", climfile = "", params = [], gridparams = [], color = None):
    self.name = name
    self.infile = infile
    self.fwfile = fwfile
    self.climfile = climfile
    self.params = params
    self.gridparams = gridparams
    self.color = color

  def __getitem__(self, i):
    return self.params[i]
    
  def __len__(self):
    return len(self.params)

  def __repr__(self):
    return "<VPLOT Body: %s>" % self.name

class Param(object):
  '''
  
  '''
  def __init__(self, name = "", description = "", unit = "", array = []):
    self.name = name
    self.description = description
    self.unit = unit
    self.array = array
  
  def label(self, short = False, maxsz = np.inf):
    '''
    
    '''
    if short or len(self.description) > maxsz:
      lbl = self.name
    else:
      lbl = self.description
    if self.unit != "":
      return "%s (%s)" % (lbl, self.unit)
    else:
      return lbl

  def __repr__(self):
    return "<VPLOT Array: %s>" % self.name

def GetParamDescriptions():
  '''
  
  '''
  
  # Call vplanet help (it better be in the path!)
  try:
    if sys.version_info >= (3,0):
      help = subprocess.getoutput("vplanet -h")
    else:
      help = subprocess.check_output(["vplanet", "-h"])
  except OSError:
    raise Exception("Unable to call VPLANET. Is it in your PATH?")
  
  # Remove bold tags
  help = help.replace('\x1b[1m', '')
  help = help.replace('\x1b[0m', '')
  
  # Get only the output params
  try:
    stroutput = help.split('These options follow the argument saOutputOrder.')[1]
  except:
    # Debugging for Diego (2/16/2016), who gets an error on the line above
    np.savez('error_report', help = help, version = sys.version_info)
    raise Exception('Error report generated.')
  
  stroutput = [x for x in stroutput.split('\n') if len(x)]

  description = {}
  for out in stroutput:
    if out.startswith('[-]'):
      n, d, u = re.search('\[-\](.*) -- (.*). \[(.*)\]', out).groups()
      description.update({n: d})
    else:
      n, d = re.search('(.*) -- (.*).', out).groups()
      description.update({n: d})

  return description

def GetConf():
  '''
  
  '''
  # Load user inputs into conf
  try:
    conf = imp.load_source("conf", "vplot_config.py") 
  except IOError:
    # Create a vplot_config.py file in the output directory
    with open("vplot_config.py", 'w') as f:
      print(confstr, file = f)
      
      # Add each option in the default file
      for param in dir(defaults):
        if not param.startswith('_'):
          value = defaults.__dict__[param]
          if type(value) is str:
            value = '"%s"' % value
          print(param, '=', value, file = f)
          
    return defaults                                       

  for key, val in list(conf.__dict__.items()):
    if key.startswith('_'):
      conf.__dict__.pop(key, None)
    else:
      # Check if user provided something they shouldn't have
      if key not in defaults.__dict__.keys():                                  
        raise Exception("Invalid input parameter %s." % key)
    
  # Update default conf with user values     
  defaults.__dict__.update(conf.__dict__)   
  
  # Finally, update conf                                
  conf.__dict__.update(defaults.__dict__)

  return conf

def GetArrays(path = '.', bodies = [], benchmark = False, colors = None):
  '''
  
  '''
  
  # Is this a benchmarking run?
  if benchmark:
    logext = '.log.truth'
  else:
    logext = '.log'
  
  # Initialize
  output = Output()
  
  # Make list very large to cycle through if there are lots of planets
  if colors is None:
    colors = def_color_seq
    override_colors = False
  else:
    override_colors = True  
  colors = colors * 10
  
  # Ensure bodies is a list
  if type(bodies) is str:
    bodies = [bodies]
  
  # Get the log file
  lf = [f for f in os.listdir(path) if f.endswith(logext)]
  if len(lf) > 1:
    raise Exception("There's more than one log file in the cwd! VPLOT is confused.")
  elif len(lf) == 0:
    raise Exception("There doesn't seem to be a log file in this directory.")
  else:
    lf = os.path.join(path, lf[0])
  with open(lf, 'r') as f:
    logfile = f.readlines()
  
  # Get basic system info
  for line in logfile:
    if line.startswith('System Name:'):
      output.sysname = re.search('System Name: (.*)\n', line).groups()[0]
    elif line.startswith('Primary Input File:'):
      output.pif = re.search('Primary Input File: (.*)\n', line).groups()[0]
    elif line.startswith('Body File #'):
      file = re.search('Body File #(.*?): (.*).in\n', line).groups()[1]
      if (bodies == []) or (file in bodies):
        output.bodies.append(Body(infile = file + '.in'))
  
  # Check we got all the bodies the user wanted
  bad = list(set(bodies) - set([body.infile[:-3] for body in output.bodies]))
  if len(bad):
    raise Exception('Unable to read .in file for the following bodies: %s.' % ', '.join(bad))
  
  # Make ``logfile`` into a single string so we can search it below
  logfile = ''.join(logfile)
  
  # Grab body properties    
  for b, body in enumerate(output.bodies):

    # Grab the name
    with open(os.path.join(path, body.infile), 'r') as f:
      infile = f.readlines()
    j = np.argmax(np.array([l.startswith('sName') for l in infile]))
    if not infile[j].startswith('sName'):
      raise Exception('Unable to retrieve `sName` from %s.' % body.infile)
    body.name = infile[j].split()[1]
    body.fwfile = '%s.%s.forward' % (output.sysname, body.name)
    body.climfile = '%s.%s.Climate' % (output.sysname, body.name)
    if not os.path.exists(os.path.join(path, body.climfile)):
      body.climfile = ""

    # Grab the body color
    if override_colors:
      import pdb; pdb.set_trace()
      body.color = colors[b]
    else:
      try:
        body.color = '#' + re.search(r'- BODY: %s -.*?\nColor: (.*?)\n' % body.name,
                                     logfile, re.DOTALL).groups()[0]
      except AttributeError:
        body.color = colors[b]
      
    # Grab the forward arrays
    try:
      with open(os.path.join(path, body.fwfile), 'r') as f:
        fwfile = f.readlines()
    except IOError:
      raise Exception('Unable to open %s.' % body.fwfile)
                       
    # Now grab the output order
    outputorder = re.search(r'- BODY: %s -(.*?)\nOutput Order:(.*?)\n' % body.name, 
                            logfile, re.DOTALL).groups()[1]
    body.params = GetParams(outputorder, fwfile)

    if body.climfile != "":
      # Grab the climate arrays...
      try:
        with open(os.path.join(path, body.climfile), 'r') as f:
          climfile = f.readlines()
      except IOError:
        raise Exception('Unable to open %s.' % body.climfile)

      # ... and the grid order
      try:
        gridorder = re.search(r'- BODY: %s -(.*?)\nGrid Output Order:(.*?)\n' % body.name, 
                                logfile, re.DOTALL).groups()[1]
        body.gridparams = GetParams(gridorder, climfile)
      except:
        pass
        
  # Final check
  if len([param.name for body in output.bodies for param in body.params]) == 0:
    raise Exception("There don't seem to be any parameters to be plotted...")
  
  return output

def GetParams(outputorder, file):
    '''
    
    '''
    
    description = GetParamDescriptions()
    
    # This workaround takes care of units that contain spaces
    while True:
      foo = re.search('\[[A-Za-z0-9]+ [A-Za-z0-9]+\]', outputorder)
      if foo is not None:
        tmp = foo.group().replace(' ', '_')
        outputorder = outputorder.replace(foo.group(), tmp)
      else:
        break
    outputorder = outputorder.split()
    
    params = []
    for j, param in enumerate(outputorder):
    
      # Get the name and units
      name, unit = re.search('(.*?)\[(.*?)\]', param).groups()

      # Grab the values in the fwfile
      array = []
      for line in file:
        array.append(float(line.split()[j]))
      array = np.array(array)
      params.append(Param(name = name, unit = unit, description = description[name], array = array))
    
    return params
    
def GetOutput(path = '.', **kwargs):
  '''
  
  '''
  
  path = path.replace('~', os.path.expanduser('~'))
  
  output = GetArrays(path = path, **kwargs)

  for body in output.bodies:
    setattr(output, body.name, body)
    
    # Forward file params
    for param in getattr(output, body.name).params:
      description = param.description
      unit = param.unit
      name = param.name
      parent = body.name
      setattr(getattr(output, body.name), param.name, 
              array(param.array, unit = unit, description = description, name = name, parent = parent, color = body.color))
    
    # Grid params
    if len(getattr(output, body.name).gridparams):
      iTime = np.argmax([param.name == 'Time' for param in getattr(output, body.name).gridparams])
      Time = getattr(output, body.name).gridparams[iTime].array
      
      # Get 2d array dimensions
      J = np.where(Time[1:] > Time[:-1])[0][0] + 1
      I, r = divmod(len(Time), J)
      assert r == 0, "Irregular time grid; VPLOT is confused!"
    
      for param in getattr(output, body.name).gridparams:
        name = param.name
        if name == 'Time':
          # We don't want to overwrite the time array!
          continue
        description = param.description
        unit = param.unit
        parent = body.name
        arr = array(param.array.reshape(I, J), unit = unit, description = description, name = name, parent = parent, color = body.color)
        setattr(getattr(output, body.name), name, arr)
  
  # Grab the logfile
  output.log = GetLog(sysname = output.sysname, path = path, **kwargs)
  
  return output