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

# Run vplanet
output = vplanet.run(path / "vpl.in")

# Plot
fig, axes = plt.subplots(nrows=2, ncols=2)

time = output.A.Time / 1e3

## Upper left: Envelope Mass ##
axes[0, 0].plot(time, output.A.EnvelopeMass, color="k")
# Format
axes[0, 0].set_ylim(-0.002, 1)
axes[0, 0].set_ylabel(r"Envelope Mass (M$_\oplus$)")
axes[0, 0].set_xlabel("Time (kyr)")
axes[0, 0].set_xlim(0, 100)

## Upper right: Radius  ##
axes[0, 1].plot(time, output.A.PlanetRadius, color="k", label="A")
axes[0, 1].plot(time, output.B.PlanetRadius * 1e6, color=vplot.colors.red, label="B")
axes[0, 1].plot(
    time, output.C.PlanetRadius * 1e6, color=vplot.colors.pale_blue, label="C"
)
# Format
axes[0, 1].set_ylim(0.1, 100)
axes[0, 1].set_ylabel(r"Radius (R$_\oplus$)")
axes[0, 1].set_xlabel("Time (kyr)")
axes[0, 1].set_xlim(0, 100)
axes[0, 1].set_yscale("log")
axes[0, 1].legend(loc="lower left")

## Lower left: Eccentricity  ##
axes[1, 0].plot(time, output.A.Eccentricity, color="k")
axes[1, 0].plot(time, output.B.Eccentricity, color=vplot.colors.red)
axes[1, 0].plot(time, output.C.Eccentricity, color=vplot.colors.pale_blue)
axes[1, 0].set_xlim(0, 100)
axes[1, 0].set_xlabel("Time (kyrs)")
axes[1, 0].set_ylabel("Eccentricity")

## Lower right: Inclination  ##
axes[1, 1].plot(time, output.A.SpiNBodyInc, "k")
axes[1, 1].plot(time, output.B.SpiNBodyInc, color=vplot.colors.red)
axes[1, 1].plot(time, output.C.SpiNBodyInc, color=vplot.colors.pale_blue)
axes[1, 1].set_xlim(0, 100)
axes[1, 1].set_xlabel("Time (kyrs)")
axes[1, 1].set_ylabel("Inclination ($^\circ$)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"NBodyAtmEsc.{ext}")
