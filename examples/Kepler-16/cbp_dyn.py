"""
This script produces a reproduction of Figure 4 of Leung and Lee (2013), the orbital
dynamics of Kepler-16b, using VPLANET's binary module

David P. Fleming, University of Washington, 2018
"""

from __future__ import division, print_function

import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import vplot as vpl

#Typical plot parameters that make for pretty plot
mpl.rcParams['figure.figsize'] = (10,8)
mpl.rcParams['font.size'] = 18.0

# Load data
output = vpl.GetOutput()

# Extract data
time = output.cbp.Time
ecc = output.cbp.Eccentricity
longa = output.cbp.LongA
varpi = output.cbp.LongP
inc = output.cbp.Inc

# Plot
fig, axes = plt.subplots(nrows=2, ncols=2, sharex=True)
color = "k"

## Upper left: eccentricity ##
axes[0,0].plot(time, ecc, color=color, zorder=-1)

# Format
axes[0,0].set_xlim(time.min(),time.max())
axes[0,0].set_ylabel("Eccentricity")

## Upper left: inclination ##
axes[0,1].plot(time, inc, color=color, zorder=-1)

# Format
axes[0,1].set_xlim(time.min(),time.max())
axes[0,1].set_ylabel("Inclination [$^{\circ}$]")

## Lower left: Longitude of periapse ##
axes[1,0].scatter(time, varpi, color=color, s=10, zorder=-1)

# Format
axes[1,0].set_xlim(time.min(),time.max())
axes[1,0].set_ylim(0,360)
axes[1,0].set_xlabel("Time [yr]")
axes[1,0].set_ylabel("Longitude of the Periapsis [$^{\circ}$]")

## Lower right: Longitude of the ascending node ##
axes[1,1].scatter(time, longa, color=color, s=10, zorder=-1)

# Format
axes[1,1].set_xlim(time.min(),time.max())
axes[1,1].set_ylim(0,360)
axes[1,1].set_xlabel("Time [yr]")
axes[1,1].set_ylabel("Longitude of the\n Ascending Node [$^{\circ}$]")

fig.tight_layout()
fig.subplots_adjust(wspace=0.4)
fig.subplots_adjust(hspace=0.05)

for ax in axes.flatten():
    ax.set_rasterization_zorder(0)

fig.savefig("LL13Fig4Comp.pdf", bbox_inches="tight", dpi=600)
