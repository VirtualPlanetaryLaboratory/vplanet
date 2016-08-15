#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
plot.py
-------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import matplotlib.pyplot as pl

__all__ = ['plot', 'colors']

class colors(object):
  red = '#c91111'
  orange = '#e09401'
  pale_blue = '#13aed5'
  dark_blue = '#1321d8'
  purple = '#642197'
  
def plot(ax, x, y, **kwargs):
  '''
  
  '''
  
  # Determine the label text
  label = kwargs.pop('label', None)
  if label is None:
    label = '%s (%s)' % (y.name, y.parent)
  
  # Determine the color
  color = kwargs.pop('color', None)
  if color is None:
    color = y.color

  # Axis labels
  if len(x.unit):
    ax.set_xlabel('%s (%s)' % (x.description, x.unit))
  else:
    ax.set_xlabel(x.description)
  if len(y.unit):
    ax.set_ylabel('%s (%s)' % (y.description, y.unit))
  else:
    ax.set_ylabel(y.description)

  # Plot
  l = ax.plot(x, y, color = color, label = label, **kwargs)

  return l

def make_pretty(fig, tight_layout = True):
  '''
  
  '''
  
  # Fix ugly axes offsets
  for axis in fig.get_axes():
  
    # Hide the offsets
    xoffset = axis.get_xaxis().get_offset_text()
    xoffset.set_visible(False)
    yoffset = axis.get_yaxis().get_offset_text()
    yoffset.set_visible(False)
  
    # Redraw to grab the offset texts
    fig.canvas.draw()
    xoffset_text = xoffset.get_text().replace('\\times', '')
    yoffset_text = yoffset.get_text().replace('\\times', '')
  
    # Incorporate the offset into the axis label
    if len(xoffset_text):
      xlabel = axis.get_xlabel()
      if '(' in xlabel:
        axis.set_xlabel(xlabel.replace('(', '(%s ' % xoffset_text))
      else:
        axis.set_xlabel('%s (%s)' % (xlabel, xoffset_text))
    if len(yoffset_text):
      ylabel = axis.get_ylabel()
      if '(' in ylabel:
        axis.set_ylabel(ylabel.replace('(', '(%s ' % yoffset_text))
      else:
        axis.set_ylabel('%s (%s)' % (ylabel, yoffset_text))
  
  # Use tight layout
  if tight_layout:
    pl.tight_layout()
  
  # Make space for a title, if any
  if any([t.get_position()[1] > 0.95 for t in fig.texts]):
    fig.subplots_adjust(top=0.9)

def show(tight_layout = True, **kwargs):
  '''
  
  '''
  
  make_pretty(pl.gcf(), tight_layout = tight_layout)
  pl.show(**kwargs)

def savefig(bbox_inches = 'tight', tight_layout = True, **kwargs):
  '''
  
  '''
  
  fig = pl.gcf()
  make_pretty(fig, tight_layout = tight_layout)
  fig.savefig(bbox_inches = bbox_inches, **kwargs)
