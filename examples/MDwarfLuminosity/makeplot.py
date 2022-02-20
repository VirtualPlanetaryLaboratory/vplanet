"""
This script produces a version of the Figure 5 from Amaral+2022, the
Stellar luminosity evolution of M dwarf stars between 0.2 and 0.6 Msun,
 using VPLANET's STELLAR and FLARE modules.

Laura N. R. do Amaral, Universidad Nacional Autónoma de México, 2021
Date:  July 26th 2021
"""
import os
import pathlib
import subprocess
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.lines import Line2D
from matplotlib.patches import Patch

import vplanet

try:
    import vplot as vpl
except:
    print("Cannot import vplot. Please install vplot.")

# Check correct number of arguments
if len(sys.argv) != 2:
    print("ERROR: Incorrect number of arguments.")
    print("Usage: " + sys.argv[0] + " <pdf | png>")
    exit(1)
if sys.argv[1] != "pdf" and sys.argv[1] != "png":
    print("ERROR: Unknown file format: " + sys.argv[1])
    print("Options are: pdf, png")
    exit(1)

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))


# Running the simulations
run = ["./LumEvolFlare/vpl.in", "./LumEvolStellar/vpl.in"]

for i in range(0, 2):
    vplanet.run(path / run[i], units=False)


# Defining the directory where the data is
flare = [
    "./LumEvolFlare/FlareEvol.fstar_02.forward",
    "./LumEvolFlare/FlareEvol.fstar_03.forward",
    "./LumEvolFlare/FlareEvol.fstar_04.forward",
    "./LumEvolFlare/FlareEvol.fstar_05.forward",
    "./LumEvolFlare/FlareEvol.fstar_06.forward",
]
stellar = [
    "./LumEvolStellar/StellarEvol.sstar_02.forward",
    "./LumEvolStellar/StellarEvol.sstar_03.forward",
    "./LumEvolStellar/StellarEvol.sstar_04.forward",
    "./LumEvolStellar/StellarEvol.sstar_05.forward",
    "./LumEvolStellar/StellarEvol.sstar_06.forward",
]

# Loading the data
ftime = []
fage = []
flumbol = []
flumxuv = []
flumtot = []
flumflare = []

stime = []
sage = []
slumbol = []
slumxuv = []
slumtot = []
slumflare = []

for i in range(0, 5):
    ftime.append(np.genfromtxt(flare[i], usecols=(0), unpack=True))
    fage.append(np.genfromtxt(flare[i], usecols=(8), unpack=True))
    flumbol.append(np.genfromtxt(flare[i], usecols=(1), unpack=True))
    flumxuv.append(np.genfromtxt(flare[i], usecols=(6), unpack=True))
    flumtot.append(np.genfromtxt(flare[i], usecols=(5), unpack=True))
    flumflare.append(np.genfromtxt(flare[i], usecols=(7), unpack=True))

for i in range(0, 5):
    stime.append(np.genfromtxt(stellar[i], usecols=(0), unpack=True))
    sage.append(np.genfromtxt(stellar[i], usecols=(7), unpack=True))
    slumbol.append(np.genfromtxt(stellar[i], usecols=(1), unpack=True))
    slumxuv.append(np.genfromtxt(stellar[i], usecols=(6), unpack=True))
    slumtot.append(np.genfromtxt(stellar[i], usecols=(5), unpack=True))


# Plot
fig, axes = plt.subplots(nrows=3, ncols=2, sharex=True, figsize=(5, 7))

time02s = sage[0]
time03s = sage[1]
time04s = sage[2]
time05s = sage[3]
time06s = sage[4]

time02f = fage[0]
time03f = fage[1]
time04f = fage[2]
time05f = fage[3]
time06f = fage[4]


cmap = sns.color_palette("inferno", n_colors=5)

# alpha value
a = 0.6
d = 0.99

# linewidth
b = 1.5
c = 2

## Upper left: Bolometric Luminosity #
axes[0, 0].plot(time02s, slumbol[0], color=cmap[0], alpha=d, lw=b)
axes[0, 0].plot(time03s, slumbol[1], color=cmap[1], alpha=d, lw=b)
axes[0, 0].plot(time04s, slumbol[2], color=cmap[2], alpha=d, lw=b)
axes[0, 0].plot(time05s, slumbol[3], color=cmap[3], alpha=d, lw=b)
axes[0, 0].plot(time06s, slumbol[4], color=cmap[4], alpha=d, lw=b)

axes[0, 0].set_xscale("log")
axes[0, 0].set_yscale("log")
axes[0, 0].set_ylabel(r"Bolometric Luminosity ($L_\odot$)")
axes[0, 0].set_title("a", loc="left")

# center left: XUV luminosity by flares
axes[1, 0].plot(time02f, flumflare[0], color=cmap[0], alpha=d, lw=b)
axes[1, 0].plot(time03f, flumflare[1], color=cmap[1], alpha=d, lw=b)
axes[1, 0].plot(time04f, flumflare[2], color=cmap[2], alpha=d, lw=b)
axes[1, 0].plot(time05f, flumflare[3], color=cmap[3], alpha=d, lw=b)
axes[1, 0].plot(time06f, flumflare[4], color=cmap[4], alpha=d, lw=b)
axes[1, 0].set_yscale("log")
axes[1, 0].set_ylabel(r"XUV Flare Luminosity  ($L_\odot$)")
axes[1, 0].set_title("c", loc="left")

# Down left: XUV luminosity by flares / XUV quiescent luminosity
axes[2, 0].plot(
    time02s, flumflare[0] / flumbol[0], color=cmap[0], alpha=d, lw=b
)  # , label = r'0.2M$_{\odot}$ STELLAR')
axes[2, 0].plot(
    time03s, flumflare[1] / flumbol[1], color=cmap[1], alpha=d, lw=b
)  # , label = r'0.3M$_{\odot}$ STELLAR')
axes[2, 0].plot(
    time04s, flumflare[2] / flumbol[2], color=cmap[2], alpha=d, lw=b
)  # , label = r'0.4M$_{\odot}$ STELLAR')
axes[2, 0].plot(
    time05s, flumflare[3] / flumbol[3], color=cmap[3], alpha=d, lw=b
)  # , label = r'0.5M$_{\odot}$ STELLAR')
axes[2, 0].plot(
    time06s, flumflare[4] / flumbol[4], color=cmap[4], alpha=d, lw=b
)  # , label = r'0.6M$_{\odot}$ STELLAR')

axes[2, 0].set_title("e", loc="left")
axes[2, 0].set_yscale("log")
axes[2, 0].set_xscale("log")
axes[2, 0].set_xlabel("Stellar Age (year)")
axes[2, 0].set_ylabel(r"L$_{XUV,flare}$/L$_{bol}$ ($L_\odot$)")

# Upper right: XUV quiescent luminosity
axes[0, 1].plot(
    time02s, slumxuv[0], color=cmap[0], alpha=d, lw=b, label=r"0.2M$_{\odot}$"
)
axes[0, 1].plot(
    time03s, slumxuv[1], color=cmap[1], alpha=d, lw=b, label=r"0.3M$_{\odot}$"
)
axes[0, 1].plot(
    time04s, slumxuv[2], color=cmap[2], alpha=d, lw=b, label=r"0.4M$_{\odot}$"
)
axes[0, 1].plot(
    time05s, slumxuv[3], color=cmap[3], alpha=d, lw=b, label=r"0.5M$_{\odot}$"
)
axes[0, 1].plot(
    time06s, slumxuv[4], color=cmap[4], alpha=d, lw=b, label=r"0.6M$_{\odot}$"
)

axes[0, 1].legend(loc="lower left", ncol=1, fontsize=7)
axes[0, 1].set_yscale("log")
axes[0, 1].set_xscale("log")
axes[0, 1].set_ylabel(r"L$_{XUV,quiescent}$ ($L_\odot$)")
axes[0, 1].set_title("b", loc="left")
axes[0, 1].set_xlabel("  ")

## center right: Total XUV luminosity (flares + quiescent) ##
axes[1, 1].plot(time02s, slumtot[0], color=cmap[0], alpha=a, lw=c)
axes[1, 1].plot(time03s, slumtot[1], color=cmap[1], alpha=a, lw=c)
axes[1, 1].plot(time04s, slumtot[2], color=cmap[2], alpha=a, lw=c)
axes[1, 1].plot(time05s, slumtot[3], color=cmap[3], alpha=a, lw=c)
axes[1, 1].plot(time06s, slumtot[4], color=cmap[4], alpha=a, lw=c)

axes[1, 1].plot(time02f, flumtot[0], "--", color=cmap[0], alpha=d, lw=b)
axes[1, 1].plot(time03f, flumtot[1], "--", color=cmap[1], alpha=d, lw=b)
axes[1, 1].plot(time04f, flumtot[2], "--", color=cmap[2], alpha=d, lw=b)
axes[1, 1].plot(time05f, flumtot[3], "--", color=cmap[3], alpha=d, lw=b)
axes[1, 1].plot(time06f, flumtot[4], "--", color=cmap[4], alpha=d, lw=b)

axes[1, 1].set_yscale("log")
axes[1, 1].set_ylabel(r"XUV Luminosity  ($L_\odot$)")
axes[1, 1].set_title("d", loc="left")


## Down left:LXUVTot ##
axes[2, 1].plot(time02s, flumflare[0] / flumxuv[0], color=cmap[0], alpha=d, lw=b)
axes[2, 1].plot(time03s, flumflare[1] / flumxuv[1], color=cmap[1], alpha=d, lw=b)
axes[2, 1].plot(time04s, flumflare[2] / flumxuv[2], color=cmap[2], alpha=d, lw=b)
axes[2, 1].plot(time05s, flumflare[3] / flumxuv[3], color=cmap[3], alpha=d, lw=b)
axes[2, 1].plot(time06s, flumflare[4] / flumxuv[4], color=cmap[4], alpha=d, lw=b)

axes[2, 1].set_yscale("log")
axes[2, 1].set_xscale("log")
axes[2, 1].set_ylabel(r"L$_{XUV,flare}$/L$_{XUV,quiescent}$ ($L_\odot$)")
axes[2, 1].set_title("f", loc="left")
axes[2, 1].set_xlabel("  ")
axes[2, 1].set_xlabel("Stellar Age (year)")


# Legend
legend_elements = [
    Line2D([0], [0], ls="-", color="black", lw=1, label=r"L$_{XUV,quiescent}$"),
    Line2D(
        [0],
        [0],
        ls="--",
        color="black",
        lw=1,
        label=r"L$_{XUV,flare}$ + L$_{XUV,quiescent}$",
    ),
]

axes[1, 1].legend(
    handles=legend_elements,
    bbox_to_anchor=(0, 0, 0, 0),
    loc="lower left",
    ncol=1,
    fontsize=6.5,
)

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time02s[1], time02s.max())
    ax.set_xscale("log")
    # Set rasterization
    ax.set_rasterization_zorder(0)

# Saving figure
if sys.argv[1] == "pdf":
    fig.savefig("MDwarfLuminosity.pdf", bbox_inches="tight", dpi=300)
if sys.argv[1] == "png":
    fig.savefig("MDwarfLuminosity.png", bbox_inches="tight", dpi=300)
