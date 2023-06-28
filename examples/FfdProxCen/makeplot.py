"""
This script produces a version of Figure 10 from Davenport+2019, the
flare frequency distribution for Proxima Centauri, using VPLANET's STELLAR and FLARE modules.

Laura N.  R. do Amaral, Universidad Nacional Autónoma de México, 2021
date: Sep. 2021
"""
import pathlib
import sys
from math import exp, expm1, log, log10

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.lines import Line2D

import vplanet

try:
    import vplot as vpl
except:
    print("Cannot import vplot. Please install vplot.")

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

run = [
    "./davenport/vpl.in",
    "./lacy/vpl.in",
]


# Run the simulations
for i in range(0, 2):
    vplanet.run(path / run[i], units=False)

# Loading the data
data_daven = np.loadtxt(path / "./davenport/davenport.star.forward")
data_lacy = np.loadtxt(path / "./lacy/lacy.star.forward")

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
fig = plt.figure(figsize=(6.5, 6.5))

a = 2

fp = [1, 1, 1, 1]
np.interp(lacy[5:9], lacy[1:5], fp)
plt.plot(lacy[5:9], lacy[1:5], "-", color="k", linewidth=a)

np.interp(Mdata[5:9], Mdata[1:5], fp)
plt.plot(Mdata[5:9], Mdata[1:5], "-", color=vpl.colors.red, linewidth=a)

np.interp(Ndata[5:9], Ndata[1:5], fp)
plt.plot(Ndata[5:9], Ndata[1:5], "-", color=vpl.colors.orange, linewidth=a)

np.interp(Odata[5:9], Odata[1:5], fp)
plt.plot(Odata[5:9], Odata[1:5], "-", color=vpl.colors.dark_blue, linewidth=a)

np.interp(Pdata[5:9], Pdata[1:5], fp)
plt.plot(Pdata[5:9], Pdata[1:5], "-", color=vpl.colors.pale_blue, linewidth=a)


for i in range(1, 5):
    plt.plot(lacy[i + 4], lacy[i], label="t")
    plt.plot(Mdata[i + 4], Mdata[i], label="t")
    plt.plot(Ndata[i + 4], Ndata[i], label="t")
    plt.plot(Odata[i + 4], Odata[i], label="t")
    plt.plot(Pdata[i + 4], Pdata[i], label="t")


# Limits and scale
plt.yscale("log")
plt.xscale("log")
plt.xlim(1e34, 1e36)


# Legend
legend_elements = [
    Line2D(
        [0],
        [0],
        ls="-",
        color=vpl.colors.red,
        lw=a,
        label="Davenport et al. (2019), 1 Myr",
    ),
    Line2D(
        [0],
        [0],
        ls="-",
        color=vpl.colors.orange,
        lw=a,
        label="Davenport et al. (2019), 10 Myr",
    ),
    Line2D(
        [0],
        [0],
        ls="-",
        color=vpl.colors.dark_blue,
        lw=a,
        label="Davenport et al. (2019), 100 Myr",
    ),
    Line2D(
        [0],
        [0],
        ls="-",
        color=vpl.colors.pale_blue,
        lw=a,
        label="Davenport et al. (2019), 1 Gyr",
    ),
    Line2D([0], [0], color="k", lw=a, label="Lacy et al. (1976)"),
]

plt.legend(handles=legend_elements, ncol=1, loc="lower left", fontsize=12)


# Label
plt.title("Flare Frequency Distribution", fontsize=15)
plt.xlabel("log(Flare Energy/ergs)", fontsize=15)
plt.ylabel("Cumulative Flare Freq (#/day)", fontsize=15)
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)

# Saving figure
ext = get_args().ext
fig.savefig(path / f"FfdProxCen.{ext}", bbox_inches="tight", dpi=200)
