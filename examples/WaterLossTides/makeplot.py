import vplanet
import vplot
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pathlib
import sys

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Tweaks
mpl.rcParams["figure.figsize"] = (6.5, 6)
mpl.rcParams["font.size"] = 10.0

# Load data
cpl = vplanet.run(path / "WaterCPL" / "vpl.in")
ctl = vplanet.run(path / "WaterCTL" / "vpl.in")

# Plot
fig, axes = plt.subplots(nrows=2, ncols=2)

time = cpl.b.Time / 1e9

## Upper left: Water Mass ##
axes[0, 0].plot(time, cpl.b.SurfWaterMass, color="k", label="CPL")
axes[0, 0].plot(time, ctl.b.SurfWaterMass, color=vplot.colors.red, label="CTL")
# Format
axes[0, 0].set_ylim(-0.02, 10)
axes[0, 0].set_ylabel("Water Mass (Earth Oceans)")
axes[0, 0].set_xlabel("Time (Gyr)")
axes[0, 0].set_xlim(0, 1)
axes[0, 0].legend(loc="upper right", fontsize=12, ncol=1)

## Upper right: Oxygen mass  ##
axes[0, 1].plot(time, cpl.b.OxygenMass, color="k")
axes[0, 1].plot(time, ctl.b.OxygenMass, color=vplot.colors.red)
# Format
axes[0, 1].set_ylim(0, 2100)
axes[0, 1].set_ylabel("Oxygen Pressure (bars)")
axes[0, 1].set_xlabel("Time (Gyr)")
axes[0, 1].set_xlim(0, 1)

## Lower left: semi-major axis ##
axes[1, 0].plot(time, cpl.b.SemiMajorAxis, color="k")
axes[1, 0].plot(time, ctl.b.SemiMajorAxis, color=vplot.colors.red)
# Format
axes[1, 0].set_ylabel("Semi-Major Axis (AU)")
axes[1, 0].set_xlabel("Time (Gyr)")
axes[1, 0].set_xlim(0, 1)

## Lower Right: Eccentricity ##
axes[1, 1].plot(time, cpl.b.Eccentricity, color="k")
axes[1, 1].plot(time, ctl.b.Eccentricity, color=vplot.colors.red)
# Format
axes[1, 1].set_ylim(0, 0.1)
axes[1, 1].set_ylabel("Eccentricity")
axes[1, 1].set_xlabel("Time (Gyr)")
axes[1, 1].set_xlim(0, 1)

for ax in axes.flatten():
    # Set rasterization
    ax.set_rasterization_zorder(0)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"WaterLossTides.{ext}", bbox_inches="tight", dpi=200)
