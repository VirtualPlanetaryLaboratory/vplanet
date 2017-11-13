#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
hec.py
------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import vplot as vpl
import matplotlib.pyplot as pl
import numpy as np

# Grab the output from a run
output = vpl.GetOutput()
time = output.star.Time
planets = output.bodies[1:][::-1]
lines = [None for i in planets]

# Plot
fig, ax = pl.subplots(2, 2, figsize = (14, 8))
fig.subplots_adjust(left = 0.075, right = 0.82, wspace = 0.2)
for i, planet in enumerate(planets):
  lines[i], = vpl.plot(ax[0,0], time, planet.EnvelopeMass)
  vpl.plot(ax[0,1], time, planet.SurfWaterMass)
  vpl.plot(ax[1,0], time, planet.OxygenMass)
  vpl.plot(ax[1,1], time, planet.XO)

# Legend
fig.legend(lines, (r'$f_H = 0.01$', r'$f_H = 0.001$', r'$f_H = 0.0005$', r'$f_H = 0.0001$', r'$f_H = 0$'), 
           'right', title = 'Initial Envelope\n Mass Fraction')

# Tweaks
ax[0,0].set_xlabel('Time (years)')
ax[0,1].set_xlabel('Time (years)')
ax[1,0].set_xlabel('Time (years)')
ax[1,1].set_xlabel('Time (years)')
ax[0,1].set_ylabel('Water Mass (TO)')
ax[1,0].set_ylabel(r'O$_2$ Pressure (bars)')
ax[1,1].set_ylabel(r'O$_2$ Mixing Ratio')
ax[0,0].set_yscale('log')
ax[0,1].set_yscale('log')
ax[1,0].set_ylim(-20,ax[1,0].get_ylim()[1])
ax[1,1].set_ylim(0,ax[1,1].get_ylim()[1])

for axis in ax.flatten():
  axis.set_xscale('log')

# Save
fig.savefig('hec.pdf', bbox_inches = 'tight')