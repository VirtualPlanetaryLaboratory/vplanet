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
cpl = vplanet.run(path / "CPL" / "vpl.in", units=False)
ctl = vplanet.run(path / "CTL" / "vpl.in", units=False)

fig, ax = plt.subplots(1, 2, figsize=(12, 4))

ax[0].plot(cpl.d.Time, cpl.d.RotPer, color="k", linestyle="dashed", label="CPL")
ax[0].plot(ctl.d.Time, ctl.d.RotPer, color="k", linestyle="solid", label="CTL")
ax[0].set_xscale("log")
ax[0].set_xlabel("Time (Years)")
ax[0].set_ylabel("Rotation Period (d)")
ax[0].legend(loc="upper left", fontsize=10)

ax[1].plot(cpl.d.Time, cpl.d.Obliquity, color="k", linestyle="dashed")
ax[1].plot(ctl.d.Time, ctl.d.Obliquity, color="k", linestyle="solid")
ax[1].set_xscale("log")
ax[1].set_xlabel("Time (Years)")
ax[1].set_ylabel(r"Oblqiuity ($^\circ$)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"TideLock.{ext}")
