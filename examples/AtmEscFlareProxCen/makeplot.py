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

# Overwritten old files
directory = ["./davenport", "./lacy", "./stellar"]

for i in directory:
    os.chdir(i)
    os.system("rm *.log")
    os.system("rm *.forward")
    os.chdir(path)


# Running the simulations
run = ["./davenport/vpl.in", "./lacy/vpl.in", "./stellar/vpl.in"]

davenport = []
lacy = []
stellar = []

directory = [davenport, lacy, stellar]

for i in range(0, 3):
    directory[i] = vplanet.run(path / run[i], units=False)

# Plot!
fig, axes = plt.subplots(nrows=2, ncols=4, sharex="col", figsize=(12, 5))

style = ["-", "--", "-."]
color = [vpl.colors.red, vpl.colors.orange, vpl.colors.dark_blue]

a = 1

for i in range(0, 3):
    axes[0, 0].plot(
        directory[i].b.Time,
        directory[i].b.SurfWaterMass,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[0, 1].plot(
        directory[i].b.Time,
        directory[i].b.EnvelopeMass,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[0, 2].plot(
        directory[i].b.Time,
        directory[i].b.PlanetRadius,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[1, 0].plot(
        directory[i].b.Time,
        directory[i].b.OxygenMass,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[1, 3].plot(
        directory[i].b.Time,
        directory[i].b.FXUV,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )
    axes[0, 3].plot(
        directory[i].star.Time,
        directory[i].star.LXUVTot / directory[i].star.Luminosity,
        color=color[i],
        linewidth=a,
        linestyle=style[i],
        alpha=0.5,
    )

axes[1, 2].plot(
    directory[0].b.Time,
    directory[0].b.DEnvMassDt,
    color=color[0],
    linewidth=a,
    linestyle=style[0],
    alpha=0.5,
    label="Flare(Davenport mode)+Stellar",
)
axes[1, 2].plot(
    directory[1].b.Time,
    directory[1].b.DEnvMassDt,
    color=color[1],
    linewidth=a,
    linestyle=style[1],
    alpha=0.5,
    label="Flare(Lacy mode)+Stellar",
)
axes[1, 2].plot(
    directory[2].b.Time,
    directory[2].b.DEnvMassDt,
    color=color[2],
    linewidth=a,
    linestyle=style[2],
    alpha=0.5,
    label="Stellar",
)

axes[1, 2].legend(loc="lower left", ncol=1, fontsize=8)

axes[1, 1].fill_between(
    directory[0].b.Time,
    directory[0].b.HZLimRecVenus,
    directory[0].b.HZLimEarlyMars,
    color=vpl.colors.purple,
    alpha=0.5,
)
axes[1, 1].annotate(
    "HZ",
    xy=(0.1, 0.35),
    xycoords="axes fraction",
    horizontalalignment="left",
    verticalalignment="bottom",
    color="w",
)

axes[0, 0].set_ylabel("Surface Water (TO)")
axes[0, 1].set_ylabel(r"Envelope Mass (M$_{\oplus}$)")
axes[0, 2].set_ylabel(r"Planetary Radius (R$_{\oplus}$)")
axes[0, 3].set_ylabel(r"L$_{XUV}$/L$_{bol}$")
axes[1, 0].set_ylabel("Oxygen Pressure (bars)")
axes[1, 1].set_ylabel("Semi-Major Axis (AU)")
axes[1, 2].set_ylabel(r"DEnvMassDt (M$_{\oplus}$ Myr$^{-1}$)")
axes[1, 3].set_ylabel(r"XUV flux (W/m$^{2}$)")

for i in range(0, 4):
    axes[0, i].set_xlim(1e6, 1e9)
    axes[1, i].set_xlim(1e6, 1e9)
    axes[0, i].set_xscale("log")
    axes[1, i].set_xscale("log")
    axes[0, i].set_xlabel("  ")
    axes[1, i].set_xlabel("System Age (year)")

axes[0, 1].set_yscale("log")

axes[1, 1].annotate(
    "Prox Cen b's orbit",
    xy=(0.03, 0.07),
    xycoords="axes fraction",
    fontsize=9,
    horizontalalignment="left",
    verticalalignment="bottom",
    color="k",
)
axes[1, 1].axhline(y=0.0485, xmin=0.0, xmax=1e11, color="k", lw=0.5)

# Save figure
if sys.argv[1] == "pdf":
    fig.savefig("AtmEscFlareProxCen.pdf", bbox_inches="tight", dpi=300)
if sys.argv[1] == "png":
    fig.savefig("AtmEscFlareProxCen.png", bbox_inches="tight", dpi=300)
