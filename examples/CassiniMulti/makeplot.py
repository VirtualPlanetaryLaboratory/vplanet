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
rd4 = vplanet.run(path / "RD4" / "vpl.in")
ll2 = vplanet.run(path / "LL2" / "vpl.in")
time = rd4.TGstar.Time / 1e3

#################### 4th order DistOrb ####################

fig = plt.figure(figsize=(6.5, 8))
plt.subplot(3, 2, 1)
plt.plot(time, rd4.TGb.Obliquity, color=vplot.colors.dark_blue)
plt.plot(time, rd4.TGc.Obliquity, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel(r"Obliquity ($^\circ$)")
plt.legend('', frameon=False) # Why do we need these!??

plt.subplot(3, 2, 2)
plt.plot(time, rd4.TGb.Eccentricity, color=vplot.colors.dark_blue)
plt.plot(time, rd4.TGc.Eccentricity, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("Eccentricity")
plt.legend('', frameon=False)

plt.subplot(3, 2, 3)
plt.plot(time, rd4.TGb.RotPer, color=vplot.colors.dark_blue)
plt.plot(time, rd4.TGc.RotPer, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("Rotation Period (days)")
plt.legend('', frameon=False)

plt.subplot(3, 2, 4)
plt.plot(time, rd4.TGb.DynEllip, color=vplot.colors.dark_blue, label="b")
plt.plot(time, rd4.TGc.DynEllip, color=vplot.colors.red, label="c")
plt.xlabel("Time (kyr)")
plt.ylabel("Dynamical Ellipticity")
plt.legend(loc="upper right", fontsize=12, ncol=1)

plt.subplot(3, 2, 5)
plt.plot(time, rd4.TGb.CassiniOne, color=vplot.colors.dark_blue)
plt.plot(time, rd4.TGc.CassiniOne, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("$\sin{\Psi}$")
plt.legend('', frameon=False)

plt.subplot(3, 2, 6)
plt.plot(time, rd4.TGb.CassiniTwo, color=vplot.colors.dark_blue)
plt.plot(time, rd4.TGc.CassiniTwo, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("$\cos{\Psi}$")
plt.legend('', frameon=False)

# Save the figure
ext = get_args().ext
fig.tight_layout()
fig.savefig(path / f"CassiniMulti4.{ext}")

#################### 2nd order DistOrb ####################

fig = plt.figure(figsize=(6.5, 8))
plt.subplot(3, 2, 1)
plt.plot(time, ll2.TGb.Obliquity, color=vplot.colors.dark_blue)
plt.plot(time, ll2.TGc.Obliquity, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel(r"Obliquity ($^\circ$)")
plt.legend('', frameon=False)

plt.subplot(3, 2, 2)
plt.plot(time, ll2.TGb.Eccentricity, color=vplot.colors.dark_blue)
plt.plot(time, ll2.TGc.Eccentricity, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("Eccentricity")
plt.legend('', frameon=False)

plt.subplot(3, 2, 3)
plt.plot(time, ll2.TGb.RotPer, color=vplot.colors.dark_blue)
plt.plot(time, ll2.TGc.RotPer, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("Rotation Period (days)")
plt.legend('', frameon=False)

plt.subplot(3, 2, 4)
plt.plot(time, ll2.TGb.DynEllip, color=vplot.colors.dark_blue, label="b")
plt.plot(time, ll2.TGc.DynEllip, color=vplot.colors.red, label="c")
plt.xlabel("Time (kyr)")
plt.ylabel("Dynamical Ellipticity")
plt.legend(loc="upper right", fontsize=12, ncol=1)

plt.subplot(3, 2, 5)
plt.plot(time, ll2.TGb.CassiniOne, color=vplot.colors.dark_blue)
plt.plot(time, ll2.TGc.CassiniOne, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("$\sin{\Psi}$")
plt.legend('', frameon=False)

plt.subplot(3, 2, 6)
plt.plot(time, ll2.TGb.CassiniTwo, color=vplot.colors.dark_blue)
plt.plot(time, ll2.TGc.CassiniTwo, color=vplot.colors.red)
plt.xlabel("Time (kyr)")
plt.ylabel("$\cos{\Psi}$")
plt.legend('', frameon=False)

# Save the figure
ext = get_args().ext
fig.tight_layout()
fig.savefig(path / f"CassiniMulti2.{ext}")
