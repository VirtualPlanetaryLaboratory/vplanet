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
time = output.star.Time / 1.0e6  # Scale to Myr
ecc1 = output.earth.Eccentricity
ecc2 = output.outer.Eccentricity
inc1 = output.earth.SpiNBodyInc
inc2 = output.outer.SpiNBodyInc
varpi1 = output.earth.LongP
varpi2 = output.outer.LongP
a1 = output.earth.SemiMajorAxis
a2 = output.outer.SemiMajorAxis
# instell = output.earth.Instellation
# mean1 = output.earth.MeanL
mean2 = output.outer.MeanLongitude
etot = (output.star.TotOrbEnergy / output.star.TotOrbEnergy[0] - 1) * 1e9
jtot = (output.star.TotAngMom / output.star.TotAngMom[0] - 1) * 1e11

# Plot
fig, axes = plt.subplots(nrows=3, ncols=2)
color = "k"

## Upper left: a1 ##
axes[0, 0].plot(time * 1000, a1, color="k")

# Format
axes[0, 0].set_xlim(0, 10)
# axes[0,0].legend(loc="best")
axes[0, 0].set_ylim(0.995, 1.005)
axes[0, 0].set_xlabel("Time [kyr]")
axes[0, 0].set_ylabel("Semi-major Axis [AU]")
axes[0, 0].set_xticks([0, 2.5, 5, 7.5, 10])
axes[0, 0].set_xticklabels(["0", "2.5", "5", "7.5", "10"])

## Upper right: eccentricities ##
axes[0, 1].plot(time, ecc1, "k")
axes[0, 1].plot(time, ecc2, color=vplot.colors.pale_blue)

# Format
axes[0, 1].set_xlim(time.min(), time.max())
axes[0, 1].set_ylim(0.0, 1)
axes[0, 1].set_xlabel("Time [Myr]")
axes[0, 1].set_ylabel("Eccentricity")
axes[0, 1].set_xticks([0, 0.25, 0.5, 0.75, 1])
axes[0, 1].set_xticklabels(["0", "0.25", "0.5", "0.75", "1"])
axes[0, 1].set_yticks([0, 0.5, 1])
axes[0, 1].set_yticklabels(["0", "0.5", "1"])

## Middle left: inclination ##
axes[1, 0].plot(time, inc1, "k", label="inner")
axes[1, 0].plot(time, inc2, color=vplot.colors.pale_blue, label="outer")

# Format
axes[1, 0].set_xlim(time.min(), time.max())
axes[1, 0].set_ylim(0, 180)
axes[1, 0].set_xlabel("Time [Myr]")
axes[1, 0].set_ylabel(r"Inclination [$^\circ$]")
axes[1, 0].legend(loc=(0.15, 0.075), fontsize=12)
axes[1, 0].set_xticks([0, 0.25, 0.5, 0.75, 1])
axes[1, 0].set_xticklabels(["0", "0.25", "0.5", "0.75", "1"])
axes[1, 0].set_yticks([0, 45, 90, 135, 180])
axes[1, 0].set_yticklabels(["0", "45", "90", "135", "180"])

## Middle right: Resonant Argument ##
arg1 = np.fabs(
    np.fmod(
        3 * output.outer.MeanLongitude
        - output.earth.MeanLongitude
        - 2 * output.earth.LongP,
        360.0,
    )
)
arg2 = np.fabs(
    np.fmod(
        3 * output.outer.MeanLongitude
        - output.earth.MeanLongitude
        - 2 * output.outer.LongP,
        360.0,
    )
)

axes[1, 1].plot(time, arg1, "o", markersize=1, color=vplot.colors.red)
axes[1, 1].plot(time, arg2, "o", markersize=1, color=vplot.colors.dark_blue)

# Format
axes[1, 1].set_xlim(time.min(), time.max())
axes[1, 1].set_ylim(0, 360)
axes[1, 1].set_xlabel("Time [Myr]")
axes[1, 1].set_ylabel(r"Res. Arg. [$^{\circ}$]")
axes[1, 1].set_xticks([0, 0.25, 0.5, 0.75, 1])
axes[1, 1].set_xticklabels(["0", "0.25", "0.5", "0.75", "1"])
axes[1, 1].set_yticks([0, 90, 180, 270, 360])
axes[1, 1].set_yticklabels(["0", "90", "180", "270", "360"])

## Bottom Left: Instellation ##
axes[2, 0].plot(time, output.earth.Instellation / 1000, "k")

# Format
axes[2, 0].set_xlim(time.min(), time.max())
axes[2, 0].set_ylim(0, 40)
# axes[2,0].set_yscale('log')
axes[2, 0].set_xlabel("Time [Myr]")
axes[2, 0].set_ylabel(r"Instellation [kW/m$^2$]")
axes[2, 0].set_xticks([0, 0.25, 0.5, 0.75, 1])
axes[2, 0].set_xticklabels(["0", "0.25", "0.5", "0.75", "1"])
axes[2, 0].set_yticks([0, 10, 20, 30, 40])
axes[2, 0].set_yticklabels(["0", "10", "20", "30", "40"])

## Bottom Right: E,J Conservation ##
axes[2, 1].plot(time, etot, color=vplot.colors.orange, label=(r"Energy $\times 10^9$"))
axes[2, 1].plot(
    time, jtot, color=vplot.colors.purple, label=(r"Ang. Mom. $\times 10^{11}$")
)

# Format
axes[2, 1].set_xlim(time.min(), time.max())
axes[2, 1].set_ylim(-1.1, 1.11)
axes[2, 1].legend(loc="best", fontsize=12)
axes[2, 1].set_xlabel("Time [Myr]")
axes[2, 1].set_ylabel(r"$\Delta$E,  $\Delta$J")
axes[2, 1].set_xticks([0, 0.25, 0.5, 0.75, 1])
axes[2, 1].set_xticklabels(["0", "0.25", "0.5", "0.75", "1"])

# Save
ext = get_args().ext
fig.savefig(path / f"ChaoticResonances.{ext}", bbox_inches="tight", dpi=600)
print(f"Wrote ChaoticResonances.{ext}.")
