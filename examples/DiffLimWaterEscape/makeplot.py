"""
Reproduces the T1-e curve from Figure 2 of Gialluca et al (2023, in prep)
and the T1-b curve from that Figure except with 10 TO initial instead of 200 TO

Megan Gialluca, UW, 2023

"""

import sys
import os
import pathlib

import matplotlib.pyplot as plt
import numpy as np

import vplanet
import vplot

# Get Path
#path = os.getcwd()
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

model = vplanet.run(path / 'vpl.in', quiet=True)

# Define Time Array
Time = model.b.Time

# Find time of diffusion-lim escape for both bodies
bdifftime = Time[np.where(model.b.WaterEscapeRegime == 4)[0][0]].value
edifftime = Time[np.where(model.e.WaterEscapeRegime == 4)[0][0]].value

# Plot figure
fig = plt.figure(figsize=(8,10))
wl = fig.add_subplot(2,1,1) # Water Lost
ob = fig.add_subplot(2,1,2) # Oxygen Buildup

wid=2

wl_b = [10-x.value for x in model.b.SurfWaterMass]
wl_e = [10-x.value for x in model.e.SurfWaterMass]

bcolor = vplot.colors.red
ecolor = vplot.colors.dark_blue

wl.plot(Time*1e-9, wl_b, color=bcolor, lw=wid, label='TRAPPIST-1b')
wl.plot(Time*1e-9, wl_e, color=ecolor, lw=wid, label='TRAPPIST-1e')
# Doesn't matter what this is, just to get the label in the legend:
wl.plot([-8,-9], [0,0], color='xkcd:grey', lw=wid, linestyle='--', label='Onset of Diffusion-Lim Escape')

ob.plot(Time*1e-9, model.b.OxygenMass, color=bcolor, lw=wid)
ob.plot(Time*1e-9, model.e.OxygenMass, color=ecolor, lw=wid)

for ax in [wl, ob]:
    ax.tick_params(length=4, width=2, labelsize=18)
    ax.tick_params(which='minor', width=1, length=2)
    ax.spines['top'].set_linewidth(2)
    ax.spines['bottom'].set_linewidth(2)
    ax.spines['left'].set_linewidth(2)
    ax.spines['right'].set_linewidth(2)
    ax.axvline(bdifftime*1e-9, color=bcolor, linestyle='--', lw=wid)
    ax.axvline(edifftime*1e-9, color=ecolor, linestyle='--', lw=wid)
    ax.set_xlim([0,1])

wl.legend(fontsize=16)
wl.set_xticklabels([])
wl.set_ylabel('Water Lost [TO]', size=18)
ob.set_ylabel('Oxygen Produced [Bar]', size=18)
ob.set_xlabel('Time [Gyr]', size=18)

plt.tight_layout()

ext = get_args().ext
fig.savefig(path / f"T1e-b_EscapeThroughTime.{ext}", dpi=200)
