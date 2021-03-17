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
import os
import subprocess

#Typical plot parameters that make for pretty plots
mpl.rcParams['font.size'] = 15.0

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Run the simulations
dir_path = os.path.dirname(os.path.realpath(__file__))
dataDirs = ["CPL_RG", "CPL_RG_OFF",
        "CTL_RG", "CTL_RG_OFF"]

# Run simulations
for dir in dataDirs:
    print ("Running "+dir+".")
    os.chdir(os.path.join(dir_path,dir))

    # Run simulation
    subprocess.call(['vplanet', 'vpl.in'])
# Return to top-level directory
os.chdir(dir_path)



# Plot
fig, axes = plt.subplots(ncols=1, nrows=3, sharex=True, figsize=(8, 18))

labels = [r"CPL, Dynamic $r_g$", r"CPL, Static $r_g$",
          r"CTL, Dynamic $r_g$", r"CTL, Static $r_g$"]

colors = [vpl.colors.red, vpl.colors.purple,
          vpl.colors.orange, vpl.colors.pale_blue]

#Extracting and plotting the data from each folder
for ii in range(len(dataDirs)):
    # Load data
    output = vpl.GetOutput(os.path.join(dir_path,dataDirs[ii]))

    # Extract data
    time = output.secondary.Time
    a_crit = output.secondary.CriticalSemiMajorAxis
    orbP = output.secondary.OrbPeriod
    ecc = output.secondary.Eccentricity

    # Top: Critical semi-major axis
    axes[0].plot(time, a_crit, lw=4, label=labels[ii], color = colors[ii])

    # Format
    axes[0].set_ylabel("Critical Semi-Major Axis [au]", fontsize=20)
    axes[0].set_ylim(ymin=0.145, ymax=0.36)
    axes[0].set_yticks([0.05*i + 0.15 for i in range(5)])
    axes[0].yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
    axes[0].legend(loc ="upper left", ncol=2, framealpha=0)

    # Mid: orbital period
    axes[1].plot(time, orbP, lw=4, label=labels[ii], color = colors[ii])

    # Format
    axes[1].set_ylabel("Orbital Period [d]", fontsize=20)
    axes[1].set_ylim(ymin=4.25, ymax=8)

    # Bottom: binary eccentricity
    axes[2].plot(time, ecc, lw=4, label=labels[ii], color = colors[ii])

    # Format
    axes[2].set_ylabel("Eccentricity", fontsize=20)
    axes[2].set_ylim(ymin=-0.01, ymax=0.32)
    axes[2].set_xlabel("Time [yr]", fontsize=20)


# Standardize formating
for ax in axes:
    ax.set_xscale("log")
    ax.set_xlim(1e5, time.max())
    ax.set_rasterization_zorder(0)

fig.tight_layout()
if (sys.argv[1] == 'pdf'):
    plt.savefig('steep.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('steep.png', bbox_inches="tight", dpi=200)
