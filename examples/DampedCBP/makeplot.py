"""
xxx
"""
import pathlib
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Typical plot parameters that make for pretty plot
mpl.rcParams["figure.figsize"] = (36, 27)
mpl.rcParams["font.size"] = 21.0

# Run vplanet
output = vplanet.run(path / "vpl.in", units=False)

# Extract data
time = output.cbp.Time
ecc_cbp = output.cbp.Eccentricity
a_cbp = output.cbp.CBPR  # Semi-major axis of the cbp
instell_cbp = output.cbp.Instellation  # Incident stellar radiation on the cbp
sol_const = 1361  # Solar Constant, average incident flux the Earth receives [W/m^2]
instell_cbp_Earth = instell_cbp / sol_const  # Instellation of cpb in Earth units

a_crit = output.secondary.CriticalSemiMajorAxis

# Plot
fig, axes = plt.subplots(ncols=1, nrows=3, sharey=False)
fig.set_size_inches(16, 22)

# Each line gets it own color
planet_color = vplot.colors.pale_blue
acrit_color = "k"  # black
a_critMax_color = vplot.colors.orange

##Plotting the insolation of the cbp on the top panel##
axes[0].plot(time, instell_cbp_Earth, lw=4, color=planet_color, ls="-", label="CBP")

# Format
axes[0].set_xscale("log")
axes[0].set_xlabel("Time [yr]", fontsize=35)
axes[0].set_xlim(1e6, time.max())
axes[0].set_ylim(ymin=0.45, ymax=0.9)
axes[0].set_ylabel("Instellation " + "[Earth Units]", fontsize=35)
axes[0].tick_params(axis="both", which="major", labelsize=28, width=3, length=9)
axes[0].set_rasterization_zorder(0)

##Plotting the semi-major axis of the cbp and a_crit on the middle panel##
axes[1].plot(
    time, a_cbp, lw=4, color=planet_color, ls="-", label="CBP"
)  # Semi-major axis of the cbp
axes[1].plot(
    time, a_crit, lw=4, color=acrit_color, ls="-", label=r"$a_{crit}$"
)  # a_crit
axes[1].axhline(
    max(a_crit), lw=6, color=a_critMax_color, ls="--", label=r"$a_{crit, max}$"
)  # Maximum a_crit

# Format
axes[1].set_xscale("log")
axes[1].set_xlabel("Time [yr]", fontsize=35)
axes[1].set_xlim(1e6, time.max())
axes[1].set_ylim(0.157, 0.3)
axes[1].set_ylabel("Semi-Major Axis [AU]", fontsize=35)
axes[1].set_yticks([0.02 * i + 0.16 for i in range(8)])
axes[1].tick_params(axis="both", which="major", labelsize=28, width=3, length=9)
axes[1].set_rasterization_zorder(0)
axes[1].legend(loc="lower left", framealpha=0, ncol=3, fontsize=30)

##Plotting the free eccentricity of the cbp##
axes[2].plot(time, ecc_cbp, lw=4, color=planet_color, ls="-", label=r"$e_{free}$")

# Format
axes[2].set_xscale("log")
axes[2].set_xlabel("Time [yr]", fontsize=35)
axes[2].set_xlim(1e6, time.max())
axes[2].set_ylim(0.037, 0.16)
axes[2].set_ylabel("Eccentricity", fontsize=35)
axes[2].tick_params(axis="both", which="major", labelsize=28, width=3, length=9)
axes[2].set_rasterization_zorder(0)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"HabitableCBP.{ext}", bbox_inches="tight", dpi=600)
