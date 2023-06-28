"""
This script produces the STEEP evolution figure from Graham et al., in prep.

David P. Fleming, University of Washington, 2020
"""

import pathlib
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot
from matplotlib.ticker import FormatStrFormatter
from tqdm import tqdm

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Typical plot parameters that make for pretty plots
mpl.rcParams["font.size"] = 15.0

# Plot
fig, axes = plt.subplots(ncols=1, nrows=3, sharex=True, figsize=(8, 18))

labels = [
    r"CPL, Dynamic $r_g$",
    r"CPL, Static $r_g$",
    r"CTL, Dynamic $r_g$",
    r"CTL, Static $r_g$",
]

colors = [
    vplot.colors.red,
    vplot.colors.purple,
    vplot.colors.orange,
    vplot.colors.pale_blue,
]

# Loop over directories to run simulations
dataDirs = ["CPL_RG", "CPL_RG_OFF", "CTL_RG", "CTL_RG_OFF"]
for ii, dataDir in tqdm(enumerate(dataDirs), total=len(dataDirs)):

    # Run vplanet
    output = vplanet.run(path / dataDir / "vpl.in", units=False, quiet=True)

    # Extract data
    time = output.secondary.Time
    a_crit = output.secondary.CriticalSemiMajorAxis
    orbP = output.secondary.OrbPeriod
    ecc = output.secondary.Eccentricity

    # Top: Critical semi-major axis
    axes[0].plot(time, a_crit, lw=4, label=labels[ii], color=colors[ii])

    # Format
    axes[0].set_ylabel("Critical Semi-Major Axis [au]", fontsize=20)
    axes[0].set_ylim(ymin=0.145, ymax=0.36)
    axes[0].set_yticks([0.05 * i + 0.15 for i in range(5)])
    axes[0].yaxis.set_major_formatter(FormatStrFormatter("%.2f"))
    axes[0].legend(loc="upper left", ncol=2, framealpha=0)

    # Mid: orbital period
    axes[1].plot(time, orbP, lw=4, label=labels[ii], color=colors[ii])

    # Format
    axes[1].set_ylabel("Orbital Period [d]", fontsize=20)
    axes[1].set_ylim(ymin=4.25, ymax=8)

    # Bottom: binary eccentricity
    axes[2].plot(time, ecc, lw=4, label=labels[ii], color=colors[ii])

    # Format
    axes[2].set_ylabel("Eccentricity", fontsize=20)
    axes[2].set_ylim(ymin=-0.01, ymax=0.32)
    axes[2].set_xlabel("Time [yr]", fontsize=20)


# Standardize formating
for ax in axes:
    ax.set_xscale("log")
    ax.set_xlim(1e5, time.max())
    ax.set_rasterization_zorder(0)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"steep.{ext}", bbox_inches="tight", dpi=600)
