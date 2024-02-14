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
out = vplanet.run(path / "vpl.in", units=False)

fig = plt.figure(figsize=(6.5, 8))
plt.subplot(2, 2, 1)
plt.plot(
    out.earth.Time,
    out.earth._40KPowerCore,
    color=vplot.colors.red,
    linestyle="-",
    label="$^{40}$K",
)
plt.plot(
    out.earth.Time,
    out.earth._232ThPowerCore,
    color=vplot.colors.orange,
    linestyle="-",
    label="$^{232}$Th",
)
plt.plot(
    out.earth.Time,
    out.earth._235UPowerCore,
    color=vplot.colors.pale_blue,
    linestyle="-",
    label="$^{235}$U",
)
plt.plot(
    out.earth.Time,
    out.earth._238UPowerCore,
    color=vplot.colors.purple,
    linestyle="-",
    label="$^{238}$U",
)
plt.plot(out.earth.Time, out.earth.RadPowerCore, "k-", label="Total")
plt.legend(loc="best")
plt.ylabel("Core Power (TW)")
plt.xlabel("Time (Gyr)")

plt.subplot(2, 2, 2)
plt.plot(out.earth.Time, out.earth._40KPowerMan, color=vplot.colors.red, linestyle="-")
plt.plot(
    out.earth.Time, out.earth._232ThPowerMan, color=vplot.colors.orange, linestyle="-"
)
plt.plot(
    out.earth.Time, out.earth._235UPowerMan, color=vplot.colors.pale_blue, linestyle="-"
)
plt.plot(
    out.earth.Time, out.earth._238UPowerMan, color=vplot.colors.purple, linestyle="-"
)
plt.plot(out.earth.Time, out.earth.RadPowerMan, "k-")
plt.ylabel("Mantle Power (TW)")
plt.xlabel("Time (Gyr)")

plt.subplot(2, 2, 3)
plt.plot(
    out.earth.Time, out.earth._40KPowerCrust, color=vplot.colors.red, linestyle="-"
)
plt.plot(
    out.earth.Time, out.earth._232ThPowerCrust, color=vplot.colors.orange, linestyle="-"
)
plt.plot(
    out.earth.Time,
    out.earth._235UPowerCrust,
    color=vplot.colors.pale_blue,
    linestyle="-",
)
plt.plot(
    out.earth.Time, out.earth._238UPowerCrust, color=vplot.colors.purple, linestyle="-"
)
plt.plot(out.earth.Time, out.earth.RadPowerCrust, "k-")
plt.ylabel("Crust Power (TW)")
plt.xlabel("Time (Gyr)")

plt.subplot(2, 2, 4)
plt.plot(
    out.earth.Time, out.earth._40KPowerTotal, color=vplot.colors.red, linestyle="-"
)
plt.plot(
    out.earth.Time, out.earth._232ThPowerTotal, color=vplot.colors.orange, linestyle="-"
)
plt.plot(
    out.earth.Time,
    out.earth._235UPowerTotal,
    color=vplot.colors.pale_blue,
    linestyle="-",
)
plt.plot(
    out.earth.Time, out.earth._238UPowerTotal, color=vplot.colors.purple, linestyle="-"
)
plt.plot(out.earth.Time, out.earth.RadPowerTotal, "k-")
plt.ylabel("Isotope Power (TW)")
plt.xlabel("Time (Gyr)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"RadHeat.{ext}", bbox_inches="tight", dpi=600)
