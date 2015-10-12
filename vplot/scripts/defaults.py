#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
defaults.py
-----------

These are the default config values used by VPLOT. You can override these by placing
a file named `conf.py` in the VPLANET output directory and setting custom values for
these parameters.

'''

# A
alog =                    3                                                           # If alpha-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale

# F
figheight =               2                                                           # Figure height in inches / subplot
figname =                 'vplot.png'                                                 # Figure file name
figwidth =                12                                                          # Figure width in inches

# I
interactive =             True                                                        # If true, figure pops up in an interactive window

# L
legend_loc =              'best'                                                      # Legend location. See http://matplotlib.org/api/legend_api.html#matplotlib.legend.Legend
legend_all =              True                                                        # Show legend on all subplots? If False, only show on top subplot
legend_fontsize =         10                                                          # Legend font size
line_styles =             ['b-', 'g-', 'r-', 'c-', 'k-',                              # Line styles for each body
                           'b:', 'g:', 'r:', 'c:', 'k:',
                           'b--', 'g--', 'r--', 'c--', 'k--']
linewidth =               1.5                                                         # Plot line widths

# M
maxylabelsize =           20                                                          # Maximum length for y label (in characters) when plotting extended descriptions. Above this, plots the short label instead
maxplots =                4                                                           # Maximum number of subplots per figure

# S
short_labels =            False                                                       # Use short axis labels?
skip_xzero_log =          True                                                        # If first value in x array is zero and `xlog` is set, skips the first value so that log plotting will work

# T
tight_layout =            False                                                       # Calls plt.tight_layout() for a snug fit
title =                   True                                                        # Add a title to the plot?

# X
xlabel_fontsize =         16                                                          # X-axis label font size
xlog =                    3                                                           # If x-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale
xticklabel_fontsize =     14                                                          # X-axis tick labels font size

# Y
ylabel_fontsize =         12                                                          # Y-axis label font size
ylog =                    3                                                           # If y-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale
ymargin =                 0.1                                                         # Y-axis margins in figure fraction
yticklabel_fontsize =     12                                                          # Y-axis tick labels font size

#
# DOCS
#

class _Docs(object):
  '''
  
  '''
  
  def __init__(self):
  
    # A
    self.alog =               'If alpha-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
  
    # F
    self.figheight =          'Figure height in inches per subplot'
    self.figname =            'Figure file name'
    self.figwidth =           'Figure width in inches'
    
    # I
    self.interactive =        'If true, figure pops up in an interactive window'

    # L
    self.legend_loc =         'Legend location. See http://matplotlib.org/api/legend_api.html#matplotlib.legend.Legend'
    self.legend_all =         'Show legend on all subplots? If False, only show on top subplot'
    self.legend_fontsize =    'Legend font size'
    self.line_styles =        'Line styles for each body'
    self.linewidth =          'Plot line widths'

    # M
    self.maxylabelsize =      'Maximum length for y label (in characters) when plotting extended descriptions. Above this, plots the short label instead'
    self.maxplots =           'Maximum number of subplots per figure'

    # S
    self.short_labels =       'Use short axis labels?'
    self.skip_xzero_log =     'If first value in x array is zero and `xlog` is set, skips the first value so that log plotting will work'

    # T
    self.tight_layout =       'Calls plt.tight_layout() for a snug fit'
    self.title =              'Add a title to the plot?'

    # X
    self.xlabel_fontsize =    'X-axis label font size'
    self.xlog =               'If x-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
    self.xticklabel_fontsize ='X-axis tick labels font size'

    # Y
    self.ylabel_fontsize =    'Y-axis label font size'
    self.ylog =               'If y-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
    self.ymargin =            'Y-axis margins in figure fraction'
    self.yticklabel_fontsize ='Y-axis tick labels font size'