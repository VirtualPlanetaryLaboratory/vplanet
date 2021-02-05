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

# Run simulation
subprocess.call(['vplanet', '-q', 'vpl.in'])

# Make object for each simulation set to hold data
prox = vpl.get_output()

# Time grid same for all simulations
time = prox.ProxCenELim.Time/1e6

# Define color preferences
elim_color = vpl.colors.dark_blue
rrlim_color = vpl.colors.pale_blue
auto_color = vpl.colors.orange

# Define linestyles
prox_ls = '-'
lc17_ls = ':'

# Define Labels
prox_label = "ProxCenb"
lc17_label = "LC17"
auto_label = "AtmEscAuto"

# Plot!
mpl.rcParams.update({'font.size': 10})
fig, axes = plt.subplots(nrows=3,ncols=2,figsize=(6.5,7))
axes = axes.flatten()

### Top Left Plot: L_XUV/L_tot vs Time ###
vpl.plot(axes[0],time, prox.star.LXUVTot/prox.star.Luminosity,color='k')
axes[0].set_ylabel(r"L$_{XUV}$/L$_{tot}$")

### Top Right Plot: HZ Limits ###
fbk = {'lw':0.0, 'edgecolor':None}
vpl.plot(axes[1],time, prox.ProxCenELim.HZLimRunaway,color='g')
vpl.plot(axes[1],time, prox.ProxCenELim.HZLimMaxGreenhouse,color='g')
axes[1].fill_between(time,prox.ProxCenELim.HZLimRunaway,prox.ProxCenELim.HZLimMaxGreenhouse,color='g', **fbk)
axes[1].set_ylabel('Semi-Major Axis (AU)')
axes[1].set_ylim(0,0.3)
# Plot horiz line indicating best fit semi-major axis for Prox Cen b
axes[1].axhline(y=0.0485, xmin=0.0, xmax=7.0e9, ls="--", color ="black")
# Annotate best fit
axes[1].annotate("b's orbit", xy=(0.25, 0.07), xycoords='axes fraction', fontsize=10,horizontalalignment='right', verticalalignment='bottom')
# Annotate HZ
axes[1].annotate("HZ", xy=(0.43, 0.21), xycoords='axes fraction', fontsize=15, horizontalalignment='left', verticalalignment='bottom',color='w')

### Middle Left: Envelope Mass vs Time ###
vpl.plot(axes[2],time, prox.ProxCenELim.EnvelopeMass,color=elim_color,linestyle=prox_ls,label='E-limited')
vpl.plot(axes[2],time, prox.ProxCenRRLim.EnvelopeMass,color=rrlim_color,linestyle=prox_ls,label='RR-limited')
vpl.plot(axes[2],time, prox.ProxCenAuto.EnvelopeMass,color=auto_color,linestyle=prox_ls,label='AtmEscAuto')
vpl.plot(axes[2],time, prox.LC17ELim.EnvelopeMass,color=auto_color,linestyle=lc17_ls,label='')
vpl.plot(axes[2],time, prox.LC17RRLim.EnvelopeMass,color=rrlim_color,linestyle=lc17_ls,label='')
vpl.plot(axes[2],time, prox.LC17Auto.EnvelopeMass,color=auto_color,linestyle=lc17_ls,label='')
axes[2].set_ylabel(r'Envelope Mass (M$_\oplus$)')
axes[2].legend(loc="upper right",title="Escape Model")
#leg.get_frame().set_alpha(0.0)

### Middle Right: Planet radius vs Time ###
vpl.plot(axes[3],time, prox.ProxCenELim.PlanetRadius,color='k',linestyle=prox_ls,label='ProxCenB')
vpl.plot(axes[3],time, prox.ProxCenELim.PlanetRadius,color=elim_color,linestyle=prox_ls,label='')
vpl.plot(axes[3],time, prox.ProxCenRRLim.PlanetRadius,color=rrlim_color,linestyle=prox_ls,label='')
vpl.plot(axes[3],time, prox.ProxCenAuto.PlanetRadius,color=auto_color,linestyle=prox_ls,label='')
vpl.plot(axes[3],time, prox.LC17ELim.PlanetRadius,color='k',linestyle=lc17_ls,label='Isothermal')
vpl.plot(axes[3],time, prox.LC17ELim.PlanetRadius,color=elim_color,linestyle=lc17_ls,label='')
vpl.plot(axes[3],time, prox.LC17RRLim.PlanetRadius,color=rrlim_color,linestyle=lc17_ls,label='')
vpl.plot(axes[3],time, prox.LC17Auto.PlanetRadius,color=auto_color,linestyle=lc17_ls,label='')
axes[3].set_ylabel(r'Radius (R$_\oplus$)')
axes[3].legend(loc="upper right",title="Radius Model")
axes[3].set_ylim(1,2)

### Bottom left: Water Mass vs Time ###
vpl.plot(axes[4],time, prox.ProxCenELim.SurfWaterMass,color=elim_color,linestyle=prox_ls)
vpl.plot(axes[4],time, prox.ProxCenRRLim.SurfWaterMass,color=rrlim_color,linestyle=prox_ls)
vpl.plot(axes[4],time, prox.ProxCenAuto.SurfWaterMass,color=auto_color,linestyle=prox_ls)
vpl.plot(axes[4],time, prox.LC17ELim.SurfWaterMass,color=elim_color,linestyle=lc17_ls)
vpl.plot(axes[4],time, prox.LC17RRLim.SurfWaterMass,color=rrlim_color,linestyle=lc17_ls)
vpl.plot(axes[4],time, prox.LC17Auto.SurfWaterMass,color=auto_color,linestyle=lc17_ls)
axes[4].set_ylabel("Water Mass (TO)")
axes[4].set_xlabel("Time (Myr)")
#axes[4].set_ylim(-0.0001,0.001)

### Botton Right: Oxygen Mass vs Time ###
vpl.plot(axes[5],time, prox.ProxCenELim.OxygenMass,color=elim_color,linestyle=prox_ls)
vpl.plot(axes[5],time, prox.ProxCenRRLim.OxygenMass,color=rrlim_color,linestyle=prox_ls)
vpl.plot(axes[5],time, prox.ProxCenAuto.OxygenMass,color=auto_color,linestyle=prox_ls)
vpl.plot(axes[5],time, prox.LC17ELim.OxygenMass,color=elim_color,linestyle=lc17_ls)
vpl.plot(axes[5],time, prox.LC17RRLim.OxygenMass,color=rrlim_color,linestyle=lc17_ls)
vpl.plot(axes[5],time, prox.LC17Auto.OxygenMass,color=auto_color,linestyle=lc17_ls)
axes[5].set_ylabel("Oxygen Pressure (bars)")
axes[5].set_xlabel("Time (Myr)")

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
