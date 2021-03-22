"""
This script produces a reproduction of Figure 1 of Zahn & Bouchet (1989)
using a coupled EQTIDE and STELLAR VPLANET run.

David P. Fleming, University of Washington, 2018
"""

from __future__ import division, print_function, absolute_import, \
    unicode_literals
import numpy as np
import os
import matplotlib as mpl
import matplotlib.pyplot as plt
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')
import sys
import subprocess

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)


# Runs vplanet
subprocess.call(['vplanet', 'vpl.in'])

#Typical plot parameters that make for pretty plot
mpl.rcParams['figure.figsize'] = (10,8)
mpl.rcParams['font.size'] = 18.0


# Load data
output = vpl.GetOutput()
time = output.primary.Time

# Extract important quantities
# saOutputOrder	Time -TotEn -TotAngMom -Semim -Radius -RotPer Ecce -RotRate -MeanMotion -OrbPer -SurfEnFluxTotal
Omega = output.secondary.RotRate # Rotation Rate
omega = output.secondary.MeanMotion # Mean Motion
e = output.secondary.Eccentricity
period = 2.0*np.pi/omega
om = Omega/omega

# Plot it!
fig, ax = plt.subplots()

# Twin the x-axis twice to make independent y-axes.
axes = [ax, ax.twinx(), ax.twinx()]

# Make some space on the right side for the extra y-axis.
fig.subplots_adjust(right=0.75)

# Move the last y-axis spine over to the right by 20% of the width of the axes
axes[-1].spines['right'].set_position(('axes', 1.2))

# To make the border of the right-most axis visible, we need to turn the frame
# on. This hides the other plots, however, so we need to turn its fill off.
axes[-1].set_frame_on(True)
axes[-1].patch.set_visible(False)

# And finally we get to plot things...
data = [e, period, om]
labels = [r"Eccentricity", r"Orbital Period [days]", r"$\Omega$/$n$"]
legend_labels = [r"Eccentricity", r"Orbital Period", r"$\Omega$/$n$"]
linestyle = ["-","--","-."]
colors = ['#C91111','#642197','#1321D8']

# Plot each quantities, format axes
for i in range(0,len(axes)):
    axes[i].plot(time,data[i],lw=3, ls=linestyle[i], color=colors[i], label="")
    axes[i].set_ylabel(labels[i], color=colors[i])
    axes[i].tick_params(axis='y', colors = colors[i])
    axes[i].set_xlim(time[1],time[-1])
    axes[i].set_xscale("log")
    axes[i].grid(False)

for ii in range(len(axes)):
    ax.plot([100],[100], color=colors[ii], ls=linestyle[ii], label=legend_labels[ii], lw=3)
ax.legend(loc="lower left")

# Additional formatting
axes[1].set_ylabel(labels[1],color=colors[1],labelpad=15)
axes[0].set_ylim(0.0,0.35)
axes[0].set_xlabel("Time [yr]")
axes[0].tick_params(axis='x', pad=10)

#Add text to plot to mirror Zahn+1989 figure
plt.text(0.77, 0.9,r"1 M$_{\odot}$ + 1 M$_{\odot}$",
     horizontalalignment='center',
     verticalalignment='center',
     transform = axes[0].transAxes)

# Save figure
fig.tight_layout()
if (sys.argv[1] == 'pdf'):
    fig.savefig('BinaryTides.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('BinaryTides.png', bbox_inches="tight", dpi=600)

plt.close()
