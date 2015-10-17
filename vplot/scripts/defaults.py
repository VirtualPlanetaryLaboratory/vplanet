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
aaxis =                   None                                                        # Alpha-axis param     
alog =                    3                                                           # If alpha-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale

# B
bodies =                  []                                                          # Body names to plot

# C
columns =                 True                                                        # Plot in several columns?

# F
figheight =               10                                                          # Figure height in inches
figname =                 'vplot.png'                                                 # Figure file name
figwidth =                16                                                          # Figure width in inches

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
maxrows =                 4                                                           # Maximum number of rows in figure

# S
short_labels =            False                                                       # Use short axis labels?
skip_xzero_log =          True                                                        # If first value in x array is zero and `xlog` is set, skips the first value so that log plotting will work

# T
tight_layout =            False                                                       # Calls plt.tight_layout() for a snug fit
title =                   True                                                        # Add a title to the plot?

# X
xaxis =                   'Time'                                                      # Param to plot on the x-axis
xlabel_fontsize =         16                                                          # X-axis label font size
xlog =                    3                                                           # If x-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale
xticklabel_fontsize =     14                                                          # X-axis tick labels font size

# Y
yaxis =                   []                                                          # List of params to plot on the y-axis
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
    self.aaxis =              'Parameter to plot on the alpha (transparancy) axis. Default is None'
    self.alog =               'If alpha-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
  
    # B
    self.bodies =             'List of body names to plot. Should match names of .in files. Default is [], which plots curves for all bodies in the cwd.'
    
    # C
    self.columns =            'If True, arranges subplots into multiple columns. If False, plots up to a maximum of `maxrows` plots in a single column.'
    
    # F
    self.figheight =          'Figure height in inches'
    self.figname =            'Figure file name'
    self.figwidth =           'Figure width in inches'
    
    # I
    self.interactive =        'If True, figure pops up in an interactive window. If False, saves figure directly to disk.'

    # L
    self.legend_loc =         'Legend location. See http://matplotlib.org/api/legend_api.html#matplotlib.legend.Legend'
    self.legend_all =         'Show legend on all subplots? If False, only show on top subplot'
    self.legend_fontsize =    'Legend font size'
    self.line_styles =        'Line styles for each body'
    self.linewidth =          'Plot line widths'

    # M
    self.maxylabelsize =      'Maximum length for y label (in characters) when plotting extended descriptions. Above this, plots the short label instead'
    self.maxrows =            'Maximum number of rows in figure'

    # S
    self.short_labels =       'Use short axis labels?'
    self.skip_xzero_log =     'If first value in x array is zero and `xlog` is set, skips the first value so that log plotting will work'

    # T
    self.tight_layout =       'Calls plt.tight_layout() for a snug fit'
    self.title =              'Add a title to the plot?'

    # X
    self.xaxis =              'Name of parameter to plot on the x-axis. Default is "Time".'
    self.xlabel_fontsize =    'X-axis label font size'
    self.xlog =               'If x-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
    self.xticklabel_fontsize ='X-axis tick labels font size'

    # Y
    self.yaxis =              'String or list of strings corresponding to the names of the parameters to plot on the y-axis. Default is [], which plots all parameters up to a maximum of `maxplots.`'
    self.ylabel_fontsize =    'Y-axis label font size'
    self.ylog =               'If y-axis range is more than this many orders of magnitude, make it log-scaled. Set to False to force linear scale'
    self.ymargin =            'Y-axis margins in figure fraction'
    self.yticklabel_fontsize ='Y-axis tick labels font size'