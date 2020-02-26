"""

Code to generate the atmosphere escape + ocean + thermal interior potential
tidal evolution histories for Proxima Centauri b, Figure 25 from
Barnes et al. 2016

@author: David P. Fleming
@email: dflemin3 (at) uw (dot) edu

Date: Oct. 21st, 2018

"""

import matplotlib as mpl
import matplotlib.pyplot as plt
import vplot as vpl
import numpy as np
import sys
import os
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

# Run simulations
os.chdir('Proxima')
print('Running Proxima.')
subprocess.call(['vplanet', '-q', 'vpl.in'])
os.chdir('../')

# Make object for each simulation set to hold data
prox = vpl.GetOutput('Proxima')

# Time grid same for all simulations
time = prox.b.Time/1e6

# Define color preferences
prox_color = vpl.colors.dark_blue

# Define Labels
prox_label = "Prox Cen b"

# Plot!
mpl.rcParams.update({'font.size': 10})
fig, axes = plt.subplots(nrows=3,ncols=2,figsize=(6.5,7))
axes = axes.flatten()

# Set linewidth
lw = 5

### Top Left Plot: L_XUV/L_tot vs Time ###
vpl.plot(axes[0],time, prox.star.LXUVTot/prox.star.Luminosity,color='k')
#axes[0].set_ylim(1.0e-3,1.0e2)
axes[0].set_ylabel(r"L$_{XUV}$/L$_{tot}$")
#axes[0].set_yscale("log")

### Top Right Plot: HZ Limits ###
fbk = {'lw':0.0, 'edgecolor':None}
vpl.plot(axes[1],time, prox.b.HZLimRunaway,color='g')
vpl.plot(axes[1],time, prox.b.HZLimMaxGreenhouse,color='g')
axes[1].fill_between(time,prox.b.HZLimRunaway,prox.b.HZLimMaxGreenhouse,color='g', **fbk)
axes[1].set_ylabel('Semi-Major Axis (AU)')
axes[1].set_ylim(0,0.3)
# Plot horiz line indicating best fit semi-major axis for Prox Cen b
axes[1].axhline(y=0.0485, xmin=0.0, xmax=7.0e9, ls="--", color ="black")
# Annotate best fit
axes[1].annotate("b's orbit", xy=(0.25, 0.07), xycoords='axes fraction', fontsize=10,horizontalalignment='right', verticalalignment='bottom')
# Annotate HZ
axes[1].annotate("HZ", xy=(0.43, 0.21), xycoords='axes fraction', fontsize=15, horizontalalignment='left', verticalalignment='bottom',color='w')

### Middle Left: Envelope Mass vs Time ###
vpl.plot(axes[2],time, prox.b.EnvelopeMass,color=prox_color)
axes[2].set_ylabel(r'Envelope Mass (M$_\oplus$)')
#axes[2].set_ylim(-0.005,0.11)
#leg = axes[2].legend(loc="lower left")
#leg.get_frame().set_alpha(0.0)

### Middle Right: Planet radius vs Time ###
vpl.plot(axes[3],time, prox.b.PlanetRadius,color=prox_color)
axes[3].set_ylabel(r'Radius (R$_\oplus$)')

### Bottom left: Water Mass vs Time ###
vpl.plot(axes[4],time, prox.b.SurfWaterMass,color=prox_color)
axes[4].set_ylabel("Water Mass (TO)")
axes[4].set_xlabel("Time (Myr)")
#axes[4].set_ylim(-0.0001,0.001)

### Botton Right: Oxygen Mass vs Time ###
vpl.plot(axes[5],time, prox.b.OxygenMass,color=prox_color)
axes[5].set_ylabel("Oxygen Pressure (bars)")
axes[5].set_xlabel("Time (Myr)")

### Final formatting ###
#for ii, ax in enumerate(axes):
    #ax.set_xscale("log")
    #ax.set_xlim(0,200)
    #ax.set_xlabel("Time (Myr)")
    #ax.tick_params(axis='x', which='major', pad=7)

# Tweak spacing
fig.tight_layout()
fig.subplots_adjust(hspace=0.15)
#vpl.make_pretty()

# Save figure
if (sys.argv[1] == 'pdf'):
    fig.savefig('HEC.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    fig.savefig('HEC.png', bbox_inches="tight", dpi=200)

# Done!
