"""

Code to generate the atmosphere escape + ocean + thermal interior potential
tidal evolution histories for Proxima Centauri b, Figure 25 from
Barnes et al. 2016

@author: David P. Fleming
@email: dflemin3 (at) uw (dot) edu

Date: Oct. 21st, 2018

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

# Run vplanet
prox = vplanet.run(path / "vpl.in", units=False)

# Time grid same for all simulations
time = prox.ProxCenELim.Time / 1e6

# Define color preferences
elim_color = vplot.colors.dark_blue
rrlim_color = vplot.colors.pale_blue
auto_color = vplot.colors.orange

# Define linestyles
prox_ls = "-"
lc17_ls = ":"

# Define Labels
prox_label = "ProxCenb"
lc17_label = "LC17"
auto_label = "AtmEscAuto"

# Plot!
mpl.rcParams.update({"font.size": 10})
fig, axes = plt.subplots(nrows=3, ncols=2, figsize=(6.5, 7), sharex=True)
axes = axes.flatten()
for ax in axes:
    ax.set_xlim(0, 200)

### Top Left Plot: L_XUV/L_tot vs Time ###
axes[0].plot(time, prox.star.LXUVTot / prox.star.Luminosity, color="k")
axes[0].set_ylabel(r"L$_{XUV}$/L$_{tot}$")

### Top Right Plot: HZ Limits ###
fbk = {"lw": 0.0, "edgecolor": None}
axes[1].plot(time, prox.ProxCenELim.HZLimRunaway, color="g")
axes[1].plot(time, prox.ProxCenELim.HZLimMaxGreenhouse, color="g")
axes[1].fill_between(
    time,
    prox.ProxCenELim.HZLimRunaway,
    prox.ProxCenELim.HZLimMaxGreenhouse,
    color="g",
    **fbk,
)
axes[1].set_ylabel("Semi-Major Axis (AU)")
axes[1].set_ylim(0, 0.3)
# Plot horiz line indicating best fit semi-major axis for Prox Cen b
axes[1].axhline(y=0.0485, xmin=0.0, xmax=7.0e9, ls="--", color="black")
# Annotate best fit
axes[1].annotate(
    "b's orbit",
    xy=(0.25, 0.07),
    xycoords="axes fraction",
    fontsize=10,
    horizontalalignment="right",
    verticalalignment="bottom",
)
# Annotate HZ
axes[1].annotate(
    "HZ",
    xy=(0.43, 0.21),
    xycoords="axes fraction",
    fontsize=15,
    horizontalalignment="left",
    verticalalignment="bottom",
    color="w",
)

### Middle Left: Envelope Mass vs Time ###
axes[2].plot(
    time,
    prox.ProxCenELim.EnvelopeMass,
    color=elim_color,
    linestyle=prox_ls,
    label="E-limited",
)
axes[2].plot(
    time,
    prox.ProxCenRRLim.EnvelopeMass,
    color=rrlim_color,
    linestyle=prox_ls,
    label="RR-limited",
)
axes[2].plot(
    time,
    prox.ProxCenAuto.EnvelopeMass,
    color=auto_color,
    linestyle=prox_ls,
    label="AtmEscAuto",
)
axes[2].plot(
    time,
    prox.LC17ELim.EnvelopeMass,
    color=auto_color,
    linestyle=lc17_ls,
    label="",
)
axes[2].plot(
    time,
    prox.LC17RRLim.EnvelopeMass,
    color=rrlim_color,
    linestyle=lc17_ls,
    label="",
)
axes[2].plot(
    time,
    prox.LC17Auto.EnvelopeMass,
    color=auto_color,
    linestyle=lc17_ls,
    label="",
)
axes[2].set_ylabel(r"Envelope Mass (M$_\oplus$)")
axes[2].legend(loc="upper right", title="Escape Model")
# leg.get_frame().set_alpha(0.0)

### Middle Right: Planet radius vs Time ###
axes[3].plot(
    time,
    prox.ProxCenELim.PlanetRadius,
    color="k",
    linestyle=prox_ls,
    label="ProxCenB",
)
axes[3].plot(
    time,
    prox.ProxCenELim.PlanetRadius,
    color=elim_color,
    linestyle=prox_ls,
    label="",
)
axes[3].plot(
    time,
    prox.ProxCenRRLim.PlanetRadius,
    color=rrlim_color,
    linestyle=prox_ls,
    label="",
)
axes[3].plot(
    time,
    prox.ProxCenAuto.PlanetRadius,
    color=auto_color,
    linestyle=prox_ls,
    label="",
)
axes[3].plot(
    time,
    prox.LC17ELim.PlanetRadius,
    color="k",
    linestyle=lc17_ls,
    label="Isothermal",
)
axes[3].plot(
    time,
    prox.LC17ELim.PlanetRadius,
    color=elim_color,
    linestyle=lc17_ls,
    label="",
)
axes[3].plot(
    time,
    prox.LC17RRLim.PlanetRadius,
    color=rrlim_color,
    linestyle=lc17_ls,
    label="",
)
axes[3].plot(
    time,
    prox.LC17Auto.PlanetRadius,
    color=auto_color,
    linestyle=lc17_ls,
    label="",
)
axes[3].set_ylabel(r"Radius (R$_\oplus$)")
axes[3].legend(loc="upper right", title="Radius Model")
axes[3].set_ylim(1, 2)

### Bottom left: Water Mass vs Time ###
axes[4].plot(time, prox.ProxCenELim.SurfWaterMass, color=elim_color, linestyle=prox_ls)
axes[4].plot(
    time, prox.ProxCenRRLim.SurfWaterMass, color=rrlim_color, linestyle=prox_ls
)
axes[4].plot(time, prox.ProxCenAuto.SurfWaterMass, color=auto_color, linestyle=prox_ls)
axes[4].plot(time, prox.LC17ELim.SurfWaterMass, color=elim_color, linestyle=lc17_ls)
axes[4].plot(time, prox.LC17RRLim.SurfWaterMass, color=rrlim_color, linestyle=lc17_ls)
axes[4].plot(time, prox.LC17Auto.SurfWaterMass, color=auto_color, linestyle=lc17_ls)
axes[4].set_ylabel("Water Mass (TO)")
axes[4].set_xlabel("Time (Myr)")
# axes[4].set_ylim(-0.0001,0.001)

### Botton Right: Oxygen Mass vs Time ###
axes[5].plot(time, prox.ProxCenELim.OxygenMass, color=elim_color, linestyle=prox_ls)
axes[5].plot(time, prox.ProxCenRRLim.OxygenMass, color=rrlim_color, linestyle=prox_ls)
axes[5].plot(time, prox.ProxCenAuto.OxygenMass, color=auto_color, linestyle=prox_ls)
axes[5].plot(time, prox.LC17ELim.OxygenMass, color=elim_color, linestyle=lc17_ls)
axes[5].plot(time, prox.LC17RRLim.OxygenMass, color=rrlim_color, linestyle=lc17_ls)
axes[5].plot(time, prox.LC17Auto.OxygenMass, color=auto_color, linestyle=lc17_ls)
axes[5].set_ylabel("Oxygen Pressure (bars)")
axes[5].set_xlabel("Time (Myr)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"HEC.{ext}", bbox_inches="tight", dpi=600)
