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

# Typical plot parameters that make for pretty plot
mpl.rcParams["figure.figsize"] = (6.5, 6)
mpl.rcParams["font.size"] = 10.0

# Run vplanet
cplauto = vplanet.run(path / "Lopez12CPL" / "auto" / "vpl.in", units=False)
cplbondi = vplanet.run(path / "Lopez12CPL" / "Bondi" / "vpl.in", units=False)
cplelim = vplanet.run(path / "Lopez12CPL" / "ELim" / "vpl.in", units=False)
cplrr = vplanet.run(path / "Lopez12CPL" / "RR" / "vpl.in", units=False)

# Plot
fig, axes = plt.subplots(nrows=3, ncols=2)

timeauto = cplauto.auto.Time / 1e6
timebondi = cplbondi.bondi.Time / 1e6
timeelim = cplelim.el.Time / 1e6
timerr = cplrr.rr.Time / 1e6

## Upper left: Envelope Mass ##
axes[0, 0].plot(timeauto, cplauto.auto.EnvelopeMass, color="k")
axes[0, 0].plot(timebondi, cplbondi.bondi.EnvelopeMass, color=vplot.colors.red)
axes[0, 0].plot(timeelim, cplelim.el.EnvelopeMass, color=vplot.colors.dark_blue)
axes[0, 0].plot(timerr, cplrr.rr.EnvelopeMass, color=vplot.colors.pale_blue)
# Format
axes[0, 0].set_ylim(-0.02, 1.1)
axes[0, 0].set_ylabel(r"Envelope Mass (M$_\oplus$)")
axes[0, 0].set_xlabel("Time (Myr)")
axes[0, 0].set_xlim(0, 1)

## Upper right: Radius  ##
axes[0, 1].plot(timeauto, cplauto.auto.PlanetRadius, color="k")
axes[0, 1].plot(timebondi, cplbondi.bondi.PlanetRadius, color=vplot.colors.red)
axes[0, 1].plot(timeelim, cplelim.el.PlanetRadius, color=vplot.colors.dark_blue)
axes[0, 1].plot(timerr, cplrr.rr.PlanetRadius, color=vplot.colors.pale_blue)
# Format
axes[0, 1].set_ylim(0, 35)
axes[0, 1].set_ylabel(r"Radius (R$_\oplus$)")
axes[0, 1].set_xlabel("Time (Myr)")
axes[0, 1].set_xlim(0, 1)

## Middle left: semi-major axis ##
axes[1, 0].plot(timeauto, cplauto.auto.SemiMajorAxis, color="k")
axes[1, 0].plot(timebondi, cplbondi.bondi.SemiMajorAxis, color=vplot.colors.red)
axes[1, 0].plot(timeelim, cplelim.el.SemiMajorAxis, color=vplot.colors.dark_blue)
axes[1, 0].plot(timerr, cplrr.rr.SemiMajorAxis, color=vplot.colors.pale_blue)
# Format
axes[1, 0].set_ylim(0.0949, 0.101)
axes[1, 0].set_ylabel("Semi-Major Axis (AU)")
axes[1, 0].set_xlabel("Time (Myr)")
axes[1, 0].set_xlim(0, 1)

## Middle Right: Eccentricity ##
axes[1, 1].plot(timeauto, cplauto.auto.Eccentricity, color="k")
axes[1, 1].plot(timebondi, cplbondi.bondi.Eccentricity, color=vplot.colors.red)
axes[1, 1].plot(timeelim, cplelim.el.Eccentricity, color=vplot.colors.dark_blue)
axes[1, 1].plot(timerr, cplrr.rr.Eccentricity, color=vplot.colors.pale_blue)
# Format
axes[1, 1].set_ylim(0.05, 0.21)
axes[1, 1].set_ylabel("Eccentricity")
axes[1, 1].set_xlabel("Time (Myr)")
axes[1, 1].set_xlim(0, 1)

## Lower left: Rotation Period ##
axes[2, 0].plot(timeauto, cplauto.auto.RotPer, color="k")
axes[2, 0].plot(timebondi, cplbondi.bondi.RotPer, color=vplot.colors.red)
axes[2, 0].plot(timeelim, cplelim.el.RotPer, color=vplot.colors.dark_blue)
axes[2, 0].plot(timerr, cplrr.rr.RotPer, color=vplot.colors.pale_blue)
# Format
axes[2, 0].set_xlabel("Time (yr)")
axes[2, 0].set_ylim(0, 12)
axes[2, 0].set_ylabel("Rotation Period (days)")
axes[2, 0].set_xlabel("Time (Myr)")
axes[2, 0].set_xlim(0, 1)

## Lower right: Obliquity ##
axes[2, 1].plot(timeauto, cplauto.auto.Obliquity, color="k", label="Auto")
axes[2, 1].plot(
    timebondi, cplbondi.bondi.Obliquity, color=vplot.colors.red, label="Bondi"
)
axes[2, 1].plot(
    timeelim, cplelim.el.Obliquity, color=vplot.colors.dark_blue, label="E-Lim"
)
axes[2, 1].plot(
    timerr, cplrr.rr.Obliquity, color=vplot.colors.pale_blue, label="RR-Lim"
)
# Format
axes[2, 1].set_xlabel("Time (yr)")
axes[2, 1].set_ylim(-1, 100)
axes[2, 1].set_ylabel("Obliquity (degrees)")
axes[2, 1].set_xlabel("Time (Myr)")
axes[2, 1].set_xlim(0, 1)
axes[2, 1].legend(loc="upper right", fontsize=8, ncol=1)

for ax in axes.flatten():
    # Set rasterization
    ax.set_rasterization_zorder(0)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"Lopez12CPL.{ext}", bbox_inches="tight", dpi=200)
