"""
This script produces a version of Figure 10 from Davenport+2019, the
flare frequency distribution for Proxima Centauri, using VPLANET's STELLAR and FLARE modules.

Laura N.  R. do Amaral, Universidad Nacional Autónoma de México, 2021
date: Sep. 2021
"""
import os
import pathlib
import subprocess
import sys
from math import exp, expm1, log, log10

import matplotlib as mpl
import matplotlib.colors as colors
import matplotlib.pyplot as plt
import matplotlib.style
import numpy as np
import seaborn as sns
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
from scipy.interpolate import interp1d

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

# mpl.style.use('classic')

run = [
    "./1Myr/vpl.in",
    "./10Myr/vpl.in",
    "./100Myr/vpl.in",
    "./1000Myr/vpl.in",
    "./lacy/vpl.in",
]

# Run the simulations
for i in range(0, 5):
    vplanet.run(path / run[i], units=False)

# Loading the data
M = np.loadtxt("./lacy/FfdLacy.lacy.forward")
MD = np.loadtxt("./1Myr/FfdDaven1.daven1Myr.forward")
ND = np.loadtxt("./10Myr/FfdDaven10.daven10Myr.forward")
OD = np.loadtxt("./100Myr/FfdDaven100.daven100Myr.forward")
PD = np.loadtxt("./1000Myr/FfdDaven1000.daven1000Myr.forward")


# Organizing the data
Mdata = []
MdataD = []
NdataD = []
OdataD = []
PdataD = []


for i in range(0, 9):
    Mdata.append(M[0, i])
    MdataD.append(MD[0, i])
    NdataD.append(ND[0, i])
    OdataD.append(OD[0, i])
    PdataD.append(PD[0, i])

# Plot
fig = plt.figure(figsize=(4, 3))

cmap = sns.color_palette("plasma", n_colors=4)

a = 0.5

fp = [1, 1, 1, 1]
np.interp(Mdata[5:9], Mdata[1:5], fp)
plt.plot(Mdata[5:9], Mdata[1:5], "-", color="Orange", linewidth=a)

np.interp(MdataD[5:9], MdataD[1:5], fp)
plt.plot(MdataD[5:9], MdataD[1:5], "-", color="cornflowerblue", linewidth=a)

np.interp(NdataD[5:9], NdataD[1:5], fp)
plt.plot(NdataD[5:9], NdataD[1:5], "-", color="darkviolet", linewidth=a)

np.interp(OdataD[5:9], OdataD[1:5], fp)
plt.plot(OdataD[5:9], OdataD[1:5], "-", color="darkred", linewidth=a)

np.interp(PdataD[5:9], PdataD[1:5], fp)
plt.plot(PdataD[5:9], PdataD[1:5], "-", color="red", linewidth=a)


for i in range(1, 5):
    plt.plot(Mdata[i + 4], Mdata[i], label="t")
    plt.plot(MdataD[i + 4], MdataD[i], label="t")
    plt.plot(NdataD[i + 4], NdataD[i], label="t")
    plt.plot(OdataD[i + 4], OdataD[i], label="t")
    plt.plot(PdataD[i + 4], PdataD[i], label="t")


# Limits and scale
plt.yscale("log")
plt.xscale("log")
plt.xlim(1e33, 1e36)


# Legend
legend_elements = [
    Line2D(
        [0], [0], ls="-", color="cornflowerblue", lw=1, label="Davenport mode, 1 Myr"
    ),
    Line2D([0], [0], ls="-", color="darkviolet", lw=1, label="Davenport mode, 10 Myr"),
    Line2D([0], [0], ls="-", color="darkred", lw=1, label="Davenport mode, 100 Myr"),
    Line2D([0], [0], ls="-", color="red", lw=1, label="Davenport mode, 1 Gyr"),
    Line2D([0], [0], color=cmap[3], lw=1, label="Lacy mode"),
]

plt.legend(handles=legend_elements, ncol=1, loc="lower left", fontsize=7)


# Label
plt.title("Flare Frequency Distribution")  # , fontsize=37)
plt.xlabel("log Flare Energy (erg)")  # ,fontsize=40)
plt.ylabel("Cumulative Flare Freq (#/day)")  # ,fontsize=40)
# plt.xticks(fontsize=40)
# plt.yticks(fontsize=40)


# Saving figure
if sys.argv[1] == "pdf":
    fig.savefig("FfdReproduced.pdf", bbox_inches="tight", dpi=600)
if sys.argv[1] == "png":
    fig.savefig("FfdReproduced.png", bbox_inches="tight", dpi=600)
