"""
This script produces a reproduction of Figures 2 and 3 from Rodriguez+2011, the
dynamics of CoRoT-7b and c, using VPLANET's STELLAR, EQTIDE, and DISTORB modules.

David P. Fleming, University of Washington, 2018
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
mpl.rcParams["figure.figsize"] = (10, 8)
mpl.rcParams["font.size"] = 16.0

# Run vplanet
output = vplanet.run(path / "vpl.in", units=False)

# Extract data
time = output.b.Time / 1.0e6  # Scale to Myr
ecc1 = output.b.Eccentricity
ecc2 = output.c.Eccentricity
varpi1 = output.b.LongP
varpi2 = output.c.LongP
a1 = output.b.SemiMajorAxis
a2 = output.c.SemiMajorAxis

# Plot
fig, axes = plt.subplots(nrows=2, ncols=2, sharex=True)
color = "k"

## Upper left: a1 ##
axes[0, 0].plot(time, a1, color="C3", zorder=-1, label="CoRoT-7b")

# Format
axes[0, 0].set_xlim(time.min(), time.max())
axes[0, 0].legend(loc="best")
axes[0, 0].set_ylim(0.0165, 0.019)
axes[0, 0].set_ylabel("Semi-major Axis [AU]")

## Upper right: eccentricities ##
axes[0, 1].plot(time, ecc1, color="C3", zorder=-1)
axes[0, 1].plot(time, ecc2, color="C0", zorder=-1)

# Format
axes[0, 1].set_xlim(time.min(), time.max())
axes[0, 1].set_ylim(0.0, 0.2)
axes[0, 1].set_ylabel("Eccentricity")

## Lower left: a2 ##
axes[1, 0].plot(time, a2, color="C0", zorder=-1, label="CoRoT-7c")

# Format
axes[1, 0].set_xlim(time.min(), time.max())
axes[1, 0].legend(loc="best")
axes[1, 0].set_ylim(0.0459, 0.0462)
axes[1, 0].set_xlabel("Time [Myr]")
axes[1, 0].set_ylabel("Semi-major Axis [AU]")

## Lower right: diff between longitude of periapses ##
varpiDiff = np.fabs(np.fmod(varpi1 - varpi2, 360.0))
axes[1, 1].scatter(time, varpiDiff, color="C3", s=10, zorder=-1)

# Format
axes[1, 1].set_xlim(time.min(), time.max())
axes[1, 1].set_ylim(0, 360)
axes[1, 1].set_xlabel("Time [Myr]")
axes[1, 1].set_ylabel(r"$\Delta \varpi$ [$^{\circ}$]")

# Final formating
#fig.tight_layout()
for ax in axes.flatten():
    # Rasterize
    ax.set_rasterization_zorder(0)

    # Set tick locations
    #ax.set_xticklabels(["0", "2", "4", "6", "8", "10"])
    ax.set_xticks([0, 2, 4, 6, 8, 10])

# Show late-term ecc damping
inset1 = fig.add_axes([0.74, 0.735, 0.2, 0.2])
inset1.plot(time, ecc1, color="C3", zorder=20)
inset1.plot(time, ecc2, color="C0", zorder=20)

inset1.set_xlabel("Time [Myr]", fontsize=12)
inset1.set_ylabel("Eccentricity", fontsize=12)
inset1.set_xlim(8, 10)
inset1.set_xticks([8, 9, 10])
inset1.set_xticklabels(["8", "9", "10"], fontsize=12)
inset1.set_yticks([1.0e-4, 1.0e-3, 1.0e-2])
inset1.set_yticklabels(["$10^{-4}$", "$10^{-3}$", "$10^{-2}$"], fontsize=12)
inset1.set_yscale("log")

# Show early apsidal locking
inset2 = fig.add_axes([0.74, 0.235, 0.2, 0.2])
inset2.scatter(time, varpiDiff, color="C3", s=10, zorder=20)

inset2.set_xlim(0.1, 3)
inset2.set_ylim(0, 360)
inset2.set_xscale("log")
inset2.set_yticks([0, 180, 360])
inset2.set_yticklabels(["0", "180", "360"], fontsize=12)
inset2.set_ylabel(r"$\Delta \varpi$ [$^{\circ}$]", fontsize=12)
inset2.set_xticks([0.1, 0.25, 0.5, 1, 2, 3])
inset2.set_xticklabels(["0.1", "0.25", "0.5", "1", "2", "3"], fontsize=12)
inset2.set_xlabel("Time [Myr]", fontsize=12)

# Save the figure
ext = get_args().ext
#fig.savefig(path / f"ApseLock.{ext}", bbox_inches="tight", dpi=600)
fig.savefig(path / f"ApseLock.{ext}", dpi=600)
