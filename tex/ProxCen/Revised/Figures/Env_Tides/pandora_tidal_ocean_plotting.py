"""

Code to generate the atmosphere escape + ocean + thermal interior potential
tidal evolution histories for Proxima Centauri b figure from Barnes et al. 2017

@author: David P. Fleming
@email: dflemin3 (at) uw (dot) edu

Date: March 2017

"""

from __future__ import division, print_function, absolute_import, unicode_literals
import matplotlib as mpl
import matplotlib.pyplot as plt
import vplot as vpl
import numpy as np

mpl.rcParams.update({'font.size': 24})

# Flags to control functionality
show_plot = True
save_plot = False

# Load in simulation results using vplot
ocean_path = "Data/ocean/"
no_ocean_path = "Data/no_ocean/"
cp_path = "Data/cpl/"
env_path = "Data/env_ocean/"

# Make object for each simulation set to hold data
ocean = vpl.GetOutput(ocean_path)
no_ocean = vpl.GetOutput(no_ocean_path)
cpl = vpl.GetOutput(cp_path)
env = vpl.GetOutput(env_path)

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

# Save?
if save_plot:
    fig.savefig("tidal_hec.pdf")

# Display plot?
if show_plot:
    plt.show()

# Done!
