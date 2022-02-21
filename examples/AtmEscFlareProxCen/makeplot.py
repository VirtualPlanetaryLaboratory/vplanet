"""
This script produces a figure that show the time evolution of planetary and stellar parameters for
Proxima Centuri b, using VPLANET's ATMESC, STELLAR
and FLARE modules.

@autor: Laura N.  R. do Amaral, Universidad Nacional Autónoma de México, 2021
@email: laura.nevesdoamaral@gmail.com
Date: Nov. 22st, 2021
"""

import os
import pathlib
import subprocess
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot as vpl
from matplotlib.ticker import MaxNLocator
from mpl_toolkits.axes_grid1 import make_axes_locatable

import vplanet

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Check correct number of arguments
if len(sys.argv) != 2:
    print("ERROR: Incorrect number of arguments.")
    print("Usage: " + sys.argv[0] + " <pdf | png>")
    exit(1)
if sys.argv[1] != "pdf" and sys.argv[1] != "png":
    print("ERROR: Unknown file format: " + sys.argv[1])
    print("Options are: pdf, png")
    exit(1)

# Running the simulations
run = ["./flare/vpl.in", "./stellar/vpl.in"]

for i in range(0, 2):
    vplanet.run(path / run[i], units=False)


# Make object for each simulation set to hold data
f11649c = np.loadtxt("./flare/flare.b.forward")
s11649c = np.loadtxt("./stellar/stellar.b.forward")

f11649cstar = np.loadtxt("./flare/flare.flare.forward")
s11649cstar = np.loadtxt("./stellar/stellar.stellar.forward")


f11649ctime = f11649c[:, 0]
s11649ctime = s11649c[:, 0]

flare_SurfWaterMass = f11649c[:, 2]
stellar_SurfWaterMass = s11649c[:, 2]

flare_EnvelopeMass = f11649c[:, 3]
stellar_EnvelopeMass = s11649c[:, 3]

flare_OxygenMass = f11649c[:, 4]
stellar_OxygenMass = s11649c[:, 4]

flare_FXUV = f11649c[:, 10]
stellar_FXUV = s11649c[:, 10]

flare_HZLimEarlyMars = f11649c[:, 9]
stellar_HZLimEarlyMars = s11649c[:, 9]

flare_HZLimRecVenus = f11649c[:, 8]  # HZLimRunaway
stellar_HZLimRecVenus = s11649c[:, 8]

flare_LXUVTot = f11649cstar[:, 5]
stellar_LXUVTot = s11649cstar[:, 5]

flare_Luminosity = f11649cstar[:, 1]
stellar_Luminosity = s11649cstar[:, 1]

# Plot!
fig, axes = plt.subplots(nrows=2, ncols=3, sharex="col", figsize=(6, 5))

_1649time = [s11649ctime, f11649ctime]
_1649dirs = [s11649c, f11649c]

SurfWaterMass = [flare_SurfWaterMass, stellar_SurfWaterMass]
EnvelopeMass = [flare_EnvelopeMass, stellar_EnvelopeMass]
OxygenMass = [flare_OxygenMass, stellar_OxygenMass]
FXUV = [flare_FXUV, stellar_FXUV]
HZLimEarlyMars = [flare_HZLimEarlyMars, stellar_HZLimEarlyMars]
HZLimRecVenus = [flare_HZLimRecVenus, stellar_HZLimRecVenus]  #
LXUVTot = [flare_LXUVTot, stellar_LXUVTot]
Luminosity = [flare_Luminosity, stellar_Luminosity]

style = ["-", "--"]
color = ["red", "midnightblue"]

a = 1

for i in range(0, 2):
    axes[0, 0].plot(
        _1649time[i],
        SurfWaterMass[i],
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[0, 1].plot(
        _1649time[i],
        EnvelopeMass[i],
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[1, 0].plot(
        _1649time[i],
        OxygenMass[i],
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[1, 2].plot(
        _1649time[i],
        FXUV[i],
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )


axes[0, 2].plot(
    _1649time[0],
    LXUVTot[0] / Luminosity[0],
    color=color[0],
    linewidth=a,
    linestyle=style[0],
    alpha=0.5,
    label="flare + stellar",
)
axes[0, 2].plot(
    _1649time[1],
    LXUVTot[1] / Luminosity[1],
    color=color[1],
    linewidth=a,
    linestyle=style[1],
    alpha=0.5,
    label="stellar",
)
axes[0, 2].legend(loc="lower right", ncol=1, fontsize=7)


axes[1, 1].annotate(
    "HZ",
    xy=(0.1, 0.3),
    xycoords="axes fraction",
    horizontalalignment="left",
    verticalalignment="bottom",
    color="w",
)
axes[0, 2].set_ylabel(r"L$_{XUV}$/L$_{bol}$")
axes[1, 1].set_ylabel("Semi-Major Axis (AU)")
axes[0, 0].set_ylabel("Surface Water (TO)")
axes[0, 1].set_ylabel(r"Envelope Mass (M$_{\oplus}$)")
axes[1, 2].set_ylabel(r"XUV flux (W/m$^{2}$)")
axes[1, 0].set_ylabel("Oxygen Pressure (bars)")
axes[0, 0].set_xlim(1e6, 1e9)
axes[0, 1].set_xlim(1e6, 1e9)
axes[0, 2].set_xlim(1e6, 1e9)
axes[1, 0].set_xlim(1e6, 1e9)
axes[1, 1].set_xlim(1e6, 1e9)
axes[1, 2].set_xlim(1e6, 1e9)


axes[0, 0].set_xscale("log")
axes[0, 1].set_xscale("log")
axes[0, 2].set_xscale("log")
axes[1, 0].set_xscale("log")
axes[1, 1].set_xscale("log")
axes[1, 2].set_xscale("log")


axes[0, 1].set_yscale("log")
axes[0, 2].set_yscale("log")
axes[1, 2].set_yscale("log")

axes[0, 0].set_xlabel("  ")
axes[0, 1].set_xlabel("  ")
axes[0, 2].set_xlabel("  ")

axes[1, 1].set_xlabel("System Age (year)")

axes[1, 1].fill_between(
    _1649time[0], HZLimRecVenus[0], HZLimEarlyMars[0], color="gold", alpha=0.5
)
axes[1, 1].annotate(
    "Prox Cen b's orbit",
    xy=(0.03, 0.07),
    xycoords="axes fraction",
    fontsize=7,
    horizontalalignment="left",
    verticalalignment="bottom",
    color="k",
)

# Exoplanet Archive
axes[1, 1].axhline(y=0.0485, xmin=0.0, xmax=1e11, color="k", lw=0.5)

g = "-"

# Save figure
if sys.argv[1] == "pdf":
    fig.savefig("AtmEscFlareProxCen.pdf", bbox_inches="tight", dpi=300)
if sys.argv[1] == "png":
    fig.savefig("AtmEscFlareProxCen.png", bbox_inches="tight", dpi=300)
