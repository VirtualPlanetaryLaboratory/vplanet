#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
call_vplot.py
-------------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
from vplot.plot import Plot
from vplot.utils import ShowHelp
import argparse

if __name__ == '__main__':
  parser = argparse.ArgumentParser(prog = 'VPLOT', add_help = False)
  parser.add_argument("-h", "--help", nargs = '?', default = False, const = 1, help = 'Display the help')
  parser.add_argument("-b", "--bodies", nargs = '*', default = None, help = 'Bodies to plot; should match names of .in files')
  parser.add_argument("-x", "--xaxis", default = None, help = 'Parameter to plot on the x-axis')
  parser.add_argument("-y", "--yaxis", nargs = '*', default = None, help = 'Parameter(s) to plot on the y-axis')
  parser.add_argument("-a", "--aaxis", default = None, help = 'Parameter to control line alpha')
  args = parser.parse_args()

  # Help?
  if args.help:
    if args.help == 1:
      ShowHelp()
    else:
      ShowHelp(args.help)
    quit()

  # Plot
  fig, _ = Plot(bodies = args.bodies, xaxis = args.xaxis, yaxis = args.yaxis, aaxis = args.aaxis)