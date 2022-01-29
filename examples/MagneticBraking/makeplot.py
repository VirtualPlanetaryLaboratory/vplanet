"""
This script produces a figure comparing VPLanet's various magnetic braking
implementations.
David P. Fleming, University of Washington, 2018
"""

import pathlib
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import vplot
from matplotlib.lines import Line2D
from matplotlib.patches import Patch

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Typical plot parameters that make for pretty plot
mpl.rcParams["figure.figsize"] = (9, 8)
mpl.rcParams["font.size"] = 18.0

# Run vplanet
vplanet.run(path / "vpl.in")

### Magnetic braking validation figure ###

ms = ["a_matt", "a_reiners", "a_sk"]
gs = ["b_matt", "b_reiners", "b_sk"]
labels = ["Matt et al. (2015)", "Reiners & Mohanty (2012)", "Repetto & Nelemans (2014)"]
colors = [vplot.colors.orange, vplot.colors.dark_blue, vplot.colors.pale_blue]

fig, ax = plt.subplots()

# saOutputOrder Time -TotEn -TotAngMom -Luminosity -Radius Temperature -RotPer -LXUVTot RadGyra

for ii in range(len(ms)):
    # Load in data
    m = np.genfromtxt(path / ("system." + ms[ii] + ".forward"))
    g = np.genfromtxt(path / ("system." + gs[ii] + ".forward"))

    # Plot!
    ax.plot(m[:, 0], m[:, 6], lw=3, ls="-", color=colors[ii])
    ax.plot(g[:, 0], g[:, 6], lw=3, ls="--", color=colors[ii])

# Annotate
ax.plot(
    [500], [500], lw=3, ls="-", color=vplot.colors.orange, label="Matt et al. (2015)"
)
ax.plot(
    [500],
    [500],
    lw=3,
    ls="-",
    color=vplot.colors.dark_blue,
    label="Reiners & Mohanty (2012)",
)
ax.plot(
    [500],
    [500],
    lw=3,
    ls="-",
    color=vplot.colors.pale_blue,
    label="Repetto & Nelemans (2014)",
)
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

# Save the figure
ext = get_args().ext
fig.savefig(path / f"MagneticBraking.{ext}", bbox_inches="tight", dpi=600)

### Now make Kepler comparison figure ###

# Typical plot parameters that make for pretty plots
mpl.rcParams["font.size"] = 17

## for Palatino and other serif fonts use:
mpl.rc("font", **{"family": "serif"})
mpl.rc("text", usetex=True)

# Load data
kep = pd.read_csv(path / "mcSingleMarch27.csv")
mcq = pd.read_csv(path / "mcquillan2014.tsv", delimiter="\t", comment="#", header=0)

# Plot distribution within 100 Myr of age ~ 4 Gyr
mask = np.fabs(kep["Age"].values - 4.0e9) < 1.0e8
massesUp = kep["Pri_dMass"].values[mask]
protsUp = kep["Pri_ProtAge"].values[mask]

fig, ax = plt.subplots(figsize=(6, 5))

ax.scatter(massesUp, protsUp, color="k", s=10, edgecolor=None, zorder=10)
ax.scatter(
    mcq["Mass"], mcq["Prot"], color="r", s=2, edgecolor=None, zorder=0, alpha=0.2
)
ax.scatter([1.0], [26.3], marker="*", color="C0", s=100, edgecolor=None, zorder=20)

# Format
ax.set_xlabel(r"Stellar Mass [$M_{\odot}$]")
ax.set_ylabel("Rotation Period [d]")
ax.set_xlim(0.1, 1.025)
ax.set_ylim(0.1, 60)
ax.set_yscale("log")
legend_elements = [
    Line2D(
        [0],
        [0],
        marker="o",
        color="w",
        label="VPLanet Age = 4 Gyr",
        markerfacecolor="k",
        markersize=10,
    ),
    Line2D(
        [0],
        [0],
        marker="o",
        color="w",
        label="McQuillan et al. (2014)",
        markerfacecolor="red",
        markersize=10,
    ),
    Line2D(
        [0],
        [0],
        marker="*",
        color="w",
        label="Sun",
        markerfacecolor="C0",
        markersize=15,
    ),
]
ax.legend(handles=legend_elements, loc="best", fontsize=12)

fig.savefig(path / f"kepler.{ext}", bbox_inches="tight", dpi=600)
