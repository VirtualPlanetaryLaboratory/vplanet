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
os.chdir('ocean')
print('Running ocean.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../no_ocean')
print('Running no_ocean.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../cpl')
print('Running CPL.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../env_ocean')
print('Running env_ocean.')
subprocess.call(['vplanet', 'vpl.in'])
os.chdir('../')

# Make object for each simulation set to hold data
ocean = vpl.GetOutput('ocean')
no_ocean = vpl.GetOutput('no_ocean')
cpl = vpl.GetOutput('cpl')
env = vpl.GetOutput('env_ocean')

# Time grid same for all simulations
time = ocean.pandora.Time

# Define color preferences
ocean_color = vpl.colors.dark_blue
no_ocean_color = vpl.colors.red
cpl_color = vpl.colors.pale_blue
env_color = vpl.colors.orange

# Define Labels
ocean_label = "Ocean"
no_ocean_label = "No Ocean"
cpl_label = "CPL"
env_label = "Envelope"

# Define linestyles
ocean_ls = "-"
no_ocean_ls = "-"
cpl_ls = "-"
env_ls = "--"

# Plot!
mpl.rcParams.update({'font.size': 24})
fig, axes = plt.subplots(nrows=3,ncols=2,figsize=(18,22))
axes = axes.flatten()

# Set linewidth
lw = 5

### Top Left Plot: SurfEnFlux vs Time ###
vpl.plot(axes[0],time, ocean.pandora.SurfEnFluxTotal, color = ocean_color, lw=lw,label=ocean_label,
        ls=ocean_ls)
vpl.plot(axes[0],time, no_ocean.pandora.SurfEnFluxTotal, color = no_ocean_color, lw=lw,label=no_ocean_label,
        ls=no_ocean_ls)
vpl.plot(axes[0],time, cpl.pandora.SurfEnFluxTotal, color = cpl_color,lw=lw,label=cpl_label,
        ls=cpl_ls)
vpl.plot(axes[0],time, env.pandora.SurfEnFluxTotal, color = env_color,lw=lw,label=env_label,
        ls=env_ls)
axes[0].set_ylim(1.0e-3,1.0e2)
axes[0].set_ylabel(r"Surface Flux (W/m$^2$)")
axes[0].set_yscale("log")

### Top Right Plot: TidalQ vs Time ###
vpl.plot(axes[1],time, ocean.pandora.TidalQ, color = ocean_color, lw=lw,label=ocean_label,
        ls=ocean_ls)
vpl.plot(axes[1],time, no_ocean.pandora.TidalQ, color = no_ocean_color, lw=lw,label=no_ocean_label,
        ls=no_ocean_ls)
vpl.plot(axes[1],time, cpl.pandora.TidalQ, color = cpl_color,lw=lw,label=cpl_label,
        ls=cpl_ls)
vpl.plot(axes[1],time, env.pandora.TidalQ, color = env_color,lw=lw,label=env_label,
        ls=env_ls)
axes[1].set_ylim(-10,600)

### Middle Left: Eccentricity vs Time ###
vpl.plot(axes[2],time, cpl.pandora.Eccentricity, color = cpl_color,lw=lw,label=cpl_label)
vpl.plot(axes[2],time, no_ocean.pandora.Eccentricity, color = no_ocean_color,lw=lw,label=no_ocean_label)
vpl.plot(axes[2],time, ocean.pandora.Eccentricity, color = ocean_color,lw=lw,label=ocean_label)
vpl.plot(axes[2],time, env.pandora.Eccentricity, color = env_color,lw=lw,label=env_label,ls="--")
axes[2].set_ylim(-0.005,0.11)
leg = axes[2].legend(loc="lower left")
leg.get_frame().set_alpha(0.0)

### Middle Right: Semimajor Axis vs Time ###
vpl.plot(axes[3],time, ocean.pandora.SemiMajorAxis, color = ocean_color,lw=lw,label=ocean_label)
vpl.plot(axes[3],time, no_ocean.pandora.SemiMajorAxis, color = no_ocean_color,lw=lw,label=no_ocean_label)
vpl.plot(axes[3],time, cpl.pandora.SemiMajorAxis, color = cpl_color,lw=lw,label=cpl_label)
vpl.plot(axes[3],time, env.pandora.SemiMajorAxis, color = env_color,lw=lw,label=env_label,ls="--")

# Plot horiz line indicating best fit semi-major axis for Prox Cen b
axes[3].axhline(y=0.0485, xmin=0.0, xmax=7.0e9, lw=lw, ls="--", color ="black")

# Annotate best fit
axes[3].annotate('Best Fit', xy=(0.2, 0.08), xycoords='axes fraction', fontsize=25,
                horizontalalignment='right', verticalalignment='bottom')

### Bottom left: Envelope Mass vs Time ###
vpl.plot(axes[4],time, ocean.pandora.EnvelopeMass, color = ocean_color,lw=lw,label=ocean_label)
vpl.plot(axes[4],time, env.pandora.EnvelopeMass, color = env_color,lw=lw,label=env_label,ls="--")
axes[4].set_ylabel("Envelope Mass (M$_{\oplus}$)")
axes[4].set_ylim(-0.0001,0.001)

### Botton Right: Water Mass vs Time ###
vpl.plot(axes[5],time, ocean.pandora.SurfWaterMass, color = ocean_color,lw=lw,label=ocean_label)
vpl.plot(axes[5],time, env.pandora.SurfWaterMass, color = env_color,lw=lw,label=env_label,ls="--")
axes[5].set_ylabel("Water Mass (Earth Oceans)")

### Final formatting ###
for ii, ax in enumerate(axes):
    ax.set_xscale("log")
    ax.set_xlim(9.0e5,7.0e9)
    ax.set_xlabel("Time (years)")
    ax.tick_params(axis='x', which='major', pad=7)

    # Shade in when Prox Cen b is in the HZ (using value consistent with error bars)
    ax.axvspan(1.46e8, 7.0e9, alpha=0.25, color='grey')

# Tweak spacing
fig.tight_layout()
fig.subplots_adjust(hspace=0.175)

# Save figure
if (sys.argv[1] == 'pdf'):
    fig.savefig('HEC.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('HEC.png', bbox_inches="tight", dpi=600)

# Done!
