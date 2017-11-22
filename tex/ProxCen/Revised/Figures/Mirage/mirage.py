#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
mirage.py
---------

'''

from __future__ import division, print_function, absolute_import, unicode_literals
import vplot as vpl
import matplotlib.pyplot as pl
import numpy as np

# Grab the output from a run
output = vpl.GetOutput()
time = output.star.Time
planets = output.bodies[1:]
lines = [None for i in planets]

# Plot
fig, ax = pl.subplots(2, 2, figsize = (14, 8))
fig.subplots_adjust(wspace = 0.2)

ls = ['-', '-', '-', '-', '--', '--', '--', '--']
for i, planet in enumerate(planets):
  lines[i], = vpl.plot(ax[0,0], time, planet.SurfWaterMass, ls = ls[i])
  if planet.OxygenMantleMass[-1]:
    vpl.plot(ax[1,0], time, planet.OxygenMantleMass, ls = ls[i])
  else:
    vpl.plot(ax[1,0], time, planet.OxygenMass, ls = ls[i])
  vpl.plot(ax[0,1], time, planet.XO, ls = ls[i])

# Legend
leg = fig.legend(lines, (r'$1\ \mathrm{TO},\ \mathrm{No\ O_2\ Sink}$', 
                   r'$3\ \mathrm{TO},\ \mathrm{No\ O_2\ Sink}$', 
                   r'$5\ \mathrm{TO},\ \mathrm{No\ O_2\ Sink}$', 
                   r'$10\ \mathrm{TO},\ \mathrm{No\ O_2\ Sink}$',
                   r'$1\ \mathrm{TO},\ \mathrm{Instant\ Sink}$', 
                   r'$3\ \mathrm{TO},\ \mathrm{Instant\ Sink}$', 
                   r'$5\ \mathrm{TO},\ \mathrm{Instant\ Sink}$',
                   r'$10\ \mathrm{TO},\ \mathrm{Instant\ Sink}$'), 
                   loc = (0.565, 0.175), ncol = 2, 
                   handletextpad = 0.1,
                   title = 'Initial Water Content')
pl.setp(leg.get_title(),fontsize=14)

# Tweaks
ax[0,0].set_xlabel('Time (years)')
ax[0,1].set_xlabel('Time (years)')
ax[1,0].set_xlabel('Time (years)')
ax[0,0].set_ylabel('Water Mass (TO)')
ax[0,0].set_yscale('log')
ax[1,0].set_ylabel('O$_2$ pressure (bar)')
ax[1,0].set_ylim(-20,ax[1,0].get_ylim()[1])
ax[0,1].set_ylabel(r'O$_2$ Mixing Ratio')
ax[0,1].set_ylim(-0.04,ax[0,1].get_ylim()[1])
ax[1,1].set_visible(False)
ax[0,0].set_xlim(1e6, 1e10)
ax[0,1].set_xlim(1e6, 1e10)
ax[1,0].set_xlim(1e6, 1e10)
ax[1,1].set_xlim(1e6, 1e10)

for axis in ax.flatten():
  axis.set_xscale('log')

# Save
fig.savefig('mirage.pdf', bbox_inches = 'tight')