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


run = [
    "./davenport/vpl.in",
    "./lacy/vpl.in",
]

# Run the simulations
for i in range(0, 2):
    vplanet.run(path / run[i], units=False)

# Loading the data

data_daven = np.loadtxt("./davenport/davenport.star.forward")
data_lacy = np.loadtxt("./lacy/lacy.star.forward")

age_daven = data_daven[:, 0]

lacy = []

for i in range(0, 9):
    lacy.append(data_lacy[0, i])

Mdata = []
Ndata = []
Odata = []
Pdata = []

for idx, val in enumerate(age_daven):
    if val == (1e6):
        Mdata = data_daven[idx, :]
    if val == (1e7):
        Ndata = data_daven[idx, :]
    if val == (1e8):
        Odata = data_daven[idx, :]
    if val == (1e9):
        Pdata = data_daven[idx, :]


# Plot
fig = plt.figure(figsize=(4, 3))

a = 0.5

fp = [1, 1, 1, 1]
np.interp(lacy[5:9], lacy[1:5], fp)
plt.plot(lacy[5:9], lacy[1:5], "-", color="Orange", linewidth=a)

np.interp(Mdata[5:9], Mdata[1:5], fp)
plt.plot(Mdata[5:9], Mdata[1:5], "-", color="cornflowerblue", linewidth=a)

np.interp(Ndata[5:9], Ndata[1:5], fp)
plt.plot(Ndata[5:9], Ndata[1:5], "-", color="darkviolet", linewidth=a)

np.interp(Odata[5:9], Odata[1:5], fp)
plt.plot(Odata[5:9], Odata[1:5], "-", color="darkred", linewidth=a)

np.interp(Pdata[5:9], Pdata[1:5], fp)
plt.plot(Pdata[5:9], Pdata[1:5], "-", color="red", linewidth=a)


for i in range(1, 5):
    plt.plot(lacy[i + 4], lacy[i], label="t")
    plt.plot(Mdata[i + 4], Mdata[i], label="t")
    plt.plot(Ndata[i + 4], Ndata[i], label="t")
    plt.plot(Odata[i + 4], Odata[i], label="t")
    plt.plot(Pdata[i + 4], Pdata[i], label="t")


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
    Line2D([0], [0], color="orange", lw=1, label="Lacy mode"),
]

plt.legend(handles=legend_elements, ncol=1, loc="lower left", fontsize=7)


# Label
plt.title("Flare Frequency Distribution")
plt.xlabel("log Flare Energy (erg)")
plt.ylabel("Cumulative Flare Freq (#/day)")

# Saving figure
if sys.argv[1] == "pdf":
    fig.savefig("FfdProxCen.pdf", bbox_inches="tight", dpi=600)
if sys.argv[1] == "png":
    fig.savefig("FfdProxCen.png", bbox_inches="tight", dpi=600)
