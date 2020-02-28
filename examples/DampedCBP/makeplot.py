"""
This script produces the STEEP evolution figure from Graham et al., in prep.

David P. Fleming, University of Washington, 2020
"""

import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.ticker import FormatStrFormatter
import numpy as np
import vplot as vpl
import sys

#Typical plot parameters that make for pretty plots
mpl.rcParams['font.size'] = 24.0

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Plot
fig, axes = plt.subplots(ncols=1, nrows=3, sharey=False, figsize=(16,28))

#Making an array of all folders in order to extract the data from each folder
data = ["CPL_RG", "CPL_RG_OFF",
        "CTL_RG", "CTL_RG_OFF"]

#Colorblind-proof VPL colors for each dataset. Assigning CPL model to warm colors and CTL to cool colors.
colors = [vpl.colors.red, vpl.colors.purple,
          vpl.colors.orange, vpl.colors.pale_blue]

#Extracting and plotting the data from each folder
for sim in range(len(data)):
    # Load data
    output = vpl.GetOutput(data[sim])

    # Extract data
    time = output.secondary.Time
    a_crit = output.secondary.CriticalSemiMajorAxis
    orbP = output.secondary.OrbPeriod
    ecc = output.secondary.Eccentricity

    """
    Plotting semi-major axis on the top first, orbital period in the middle,
    and then the eccentricity on the bottom panel
    """

    cpl_bool = "cpl" in data[sim] #defines whether model used was CPL or CTL
    rg_bool = "on" in data[sim] #defines whether RG evolution was on or off

    #Labeling the type of model for each simulation.
    if cpl_bool == True:
        model = "CPL"
    elif cpl_bool == False:
        model = "CTL"

    #Including in the plot label whether the RG evolution was on or not
    if rg_bool == True:
        rg_evolution = "Dynamic "
    elif rg_bool == False:
        rg_evolution = "Static "

    label = model + ", " + rg_evolution + r"$r_g$"

    #Plotting the critical semi-major axis
    axes[0].plot(time, a_crit, lw=4, label = label, color = colors[sim])

    #Plotting the orbital period
    axes[1].plot(time, orbP, lw=4, label = label, color = colors[sim])

    #Plotting the binary eccentricity
    axes[2].plot(time, ecc, lw=4, label = label, color = colors[sim])

# Standardize formating
for ax in axes:
    ax.set_xscale("log")
    ax.set_xlabel("Time [yr]", size = 35)
    ax.set_xlim(1e+5, time.max())
    ax.tick_params(axis = 'both', which = 'major', labelsize = 35, width=3, length=9)
    ax.set_rasterization_zorder(0)

#Formats that are unique to each plot

#Fomatting a_crit
axes[0].set_ylabel("Critical Semi-Major Axis [AU]", size = 35)
axes[0].set_ylim(ymin=0.145, ymax = 0.36)
axes[0].set_yticks([0.05*i + 0.15 for i in range(5)])
axes[0].yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
axes[0].legend(loc = "upper left", ncol = 2)

#Formatting the orbital period
axes[1].set_ylabel("Orbital Period [days]", size = 35)
axes[1].set_ylim(ymin=4.25 ,ymax=8)

#formatting the binary eccentricity
axes[2].set_ylabel("Eccentricity", size = 35)
axes[2].set_ylim(ymin=-0.01 ,ymax=0.32)

fig.tight_layout()
if (sys.argv[1] == 'pdf'):
    plt.savefig('dampedCBP.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('STEEPCBP.png', bbox_inches="tight", dpi=200)
