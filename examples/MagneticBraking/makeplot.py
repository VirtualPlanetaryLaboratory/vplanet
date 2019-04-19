"""
This script produces a figure comparing VPLanet's various magnetic braking
implementations.

David P. Fleming, University of Washington, 2018
"""

from __future__ import division, print_function

import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import sys
import vplot as vpl

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Make the plot!
mpl.rcParams['figure.figsize'] = (9,8)
mpl.rcParams['font.size'] = 18.0

ms = ["a_matt", "a_reiners", "a_sk"]
gs = ["b_matt", "b_reiners", "b_sk"]
labels = ["Matt et al. (2015)", "Reiners & Mohanty (2012)",
          "Repetto & Nelemans (2014)"]
colors = [vpl.colors.orange, vpl.colors.dark_blue, vpl.colors.pale_blue]

fig, ax = plt.subplots()

# saOutputOrder Time -TotEn -TotAngMom -Luminosity -Radius Temperature -RotPer -LXUVTot RadGyra

for ii in range(len(ms)):
    # Load in data
    m = np.genfromtxt("system." + ms[ii] + ".forward")
    g = np.genfromtxt("system." + gs[ii] + ".forward")

    # Plot!
    ax.plot(m[:,0], m[:,6], lw=3, ls="-", color=colors[ii])
    ax.plot(g[:,0], g[:,6], lw=3, ls="--", color=colors[ii])

# Annotate
ax.plot([500], [500], lw=3, ls="-", color=vpl.colors.orange, label="Matt et al. (2015)")
ax.plot([500], [500], lw=3, ls="-", color=vpl.colors.dark_blue, label="Reiners & Mohanty (2012)")
ax.plot([500], [500], lw=3, ls="-", color=vpl.colors.pale_blue, label="Repetto & Nelemans (2014)")
ax.plot([500], [500], lw=3, ls="-", color="C7", label="M = 0.1 M$_{\odot}$")
ax.plot([500], [500], lw=3, ls="--", color="C7", label="M = 1 M$_{\odot}$")

# Format plot
ax.set_xlim(1.0e6, 5.0e9)
ax.set_ylim(0.1, 70)
ax.set_xscale("log")
ax.set_yscale("log")
ax.set_xlabel("Time [yr]")
ax.set_ylabel("Rotation Period [d]")
ax.legend(loc="best", fontsize=12)

if (sys.argv[1] == 'pdf'):
    plt.savefig('MagneticBraking.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    plt.savefig('MagneticBraking.png', bbox_inches="tight", dpi=600)
