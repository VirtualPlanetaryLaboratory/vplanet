# -*- coding: iso-8859-1 -*-
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
print("Running 4th order model.")
rd4 = vplanet.run(path / "RD4" / "vpl.in", units=False)

print("Running 2nd order model.")
ll2 = vplanet.run(path / "LL2" / "vpl.in", units=False)


fig = plt.figure(figsize=(6.5, 6))
fig.subplots_adjust(hspace=0.1)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/1.dat", unpack=True)
ax = plt.subplot(4, 2, 1)
plt.plot(
    rd4.Mercury.Time / 1e6, rd4.Mercury.Eccentricity, "k-", label="4th Order", zorder=100
)
plt.plot(
    ll2.Mercury.Time / 1e6, ll2.Mercury.Eccentricity, "k", linestyle='dotted', label="2nd Order", 
    zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.85, "Mercury", transform=ax.transAxes)
merc_de = rd4.Mercury.Eccentricity - e
merc_di = rd4.Mercury.Inc - inc

plt.subplot(4, 2, 2)
plt.plot(rd4.Mercury.Time / 1e6, rd4.Mercury.Inc, "k-", label="4th order", zorder=100)
plt.plot(ll2.Mercury.Time / 1e6, ll2.Mercury.Inc, "k", linestyle='dotted', label="2nd Order", zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)
plt.legend(loc="lower left", fontsize=8)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/2.dat", unpack=True)
ax = plt.subplot(4, 2, 3)
plt.plot(
    rd4.Venus.Time / 1e6, rd4.Venus.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.Venus.Time / 1e6, ll2.Venus.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.85, "Venus", transform=ax.transAxes)
venus_de = rd4.Venus.Eccentricity - e
venus_di = rd4.Venus.Inc - inc

plt.subplot(4, 2, 4)
plt.plot(rd4.Venus.Time / 1e6, rd4.Venus.Inc, "k-", zorder=100)
plt.plot(ll2.Venus.Time / 1e6, ll2.Venus.Inc, "k", linestyle='dotted', zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/3.dat", unpack=True)
ax = plt.subplot(4, 2, 5)
plt.plot(
    rd4.Earth.Time / 1e6, rd4.Earth.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.Earth.Time / 1e6, ll2.Earth.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.85, "Earth", transform=ax.transAxes)
earth_de = rd4.Earth.Eccentricity - e
earth_di = rd4.Earth.Inc - inc

plt.subplot(4, 2, 6)
plt.plot(rd4.Earth.Time / 1e6, rd4.Earth.Inc, "k-", label="DistOrb", zorder=100)
plt.plot(ll2.Earth.Time / 1e6, ll2.Earth.Inc, "k", linestyle='dotted', zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/4.dat", unpack=True)
ax = plt.subplot(4, 2, 7)
plt.plot(rd4.Mars.Time / 1e6, rd4.Mars.Eccentricity, "k-", zorder=100)
plt.plot(
    ll2.Mars.Time / 1e6, ll2.Mars.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xlabel("Time (Myr)")
plt.text(0.05, 0.05, "Mars", transform=ax.transAxes)
mars_de = rd4.Mars.Eccentricity - e
mars_di = rd4.Mars.Inc - inc

plt.subplot(4, 2, 8)
plt.plot(rd4.Mars.Time / 1e6, rd4.Mars.Inc, "k-", zorder=100)
plt.plot(ll2.Mars.Time / 1e6, ll2.Mars.Inc, "k", linestyle='dotted', zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xlabel("Time (Myr)")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"SSDistOrbDistRotInner.{ext}", bbox_inches="tight", dpi=300)
plt.close()

#################### Outer Planets #############################################

fig = plt.figure(figsize=(6.5, 6))
fig.subplots_adjust(hspace=0.1, wspace=0.23)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/5.dat", unpack=True)
ax = plt.subplot(4, 2, 1)
plt.plot(
    rd4.Jupiter.Time / 1e6, rd4.Jupiter.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.Jupiter.Time / 1e6, ll2.Jupiter.Eccentricity, "k", linestyle='dotted', 
    zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.05, "Jupiter", transform=ax.transAxes)
jup_de = rd4.Jupiter.Eccentricity - e
jup_di = rd4.Jupiter.Inc - inc

plt.subplot(4, 2, 2)
plt.plot(rd4.Jupiter.Time / 1e6, rd4.Jupiter.Inc, "k-", label="4th Order", zorder=100)
plt.plot(
    ll2.Jupiter.Time / 1e6, ll2.Jupiter.Inc, "k", linestyle='dotted', label="2nd Order", 
    zorder=100
)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylim(1, 2.3)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/6.dat", unpack=True)
ax = plt.subplot(4, 2, 3)
plt.plot(
    rd4.Saturn.Time / 1e6, rd4.Saturn.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.Saturn.Time / 1e6, ll2.Saturn.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.9, "Saturn", transform=ax.transAxes)
sat_de = rd4.Saturn.Eccentricity - e
sat_di = rd4.Saturn.Inc - inc

plt.subplot(4, 2, 4)
plt.plot(rd4.Saturn.Time / 1e6, rd4.Saturn.Inc, "k-", zorder=100)
plt.plot(ll2.Saturn.Time / 1e6, ll2.Saturn.Inc, "k", linestyle='dotted', zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/7.dat", unpack=True)
ax = plt.subplot(4, 2, 5)
plt.plot(
    rd4.George.Time / 1e6, rd4.George.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.George.Time / 1e6, ll2.George.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xticks(visible=False)
plt.text(0.05, 0.85, "Uranus", transform=ax.transAxes)
george_de = rd4.George.Eccentricity - e
george_di = rd4.George.Inc - inc

plt.subplot(4, 2, 6)
plt.plot(rd4.George.Time / 1e6, rd4.George.Inc, "k-", zorder=100)
plt.plot(ll2.George.Time / 1e6, ll2.George.Inc, "k", linestyle='dotted', zorder=100)
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt(path / "hnbdata/8.dat", unpack=True)
ax = plt.subplot(4, 2, 7)
plt.plot(
    rd4.Neptune.Time / 1e6, rd4.Neptune.Eccentricity, "k-", zorder=100
)
plt.plot(
    ll2.Neptune.Time / 1e6, ll2.Neptune.Eccentricity, "k", linestyle='dotted', zorder=100
)
plt.plot(t / 1e6, e, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$e$")
plt.xlabel("Time (Myr)")
plt.text(0.05, 0.85, "Neptune", transform=ax.transAxes)
nept_de = rd4.Neptune.Eccentricity - e
nept_di = rd4.Neptune.Inc - inc

plt.subplot(4, 2, 8)
plt.plot(rd4.Neptune.Time / 1e6, rd4.Neptune.Inc, "k-", label="4th Order", zorder=100)
plt.plot(
    ll2.Neptune.Time / 1e6, ll2.Neptune.Inc, "k", linestyle='dotted', label="2nd Order", 
    zorder=100
    )
plt.plot(t / 1e6, inc, "0.5", label="HNBody")
plt.xlim(0, 1)
plt.ylabel("$i$ ($^{\circ}$)")
plt.xlabel("Time (Myr)")
plt.legend(loc="lower left", fontsize=10, ncol=2)

# Save
fig.savefig(path / f"SSDistOrbDistRotOuter.{ext}", bbox_inches="tight", dpi=300)
plt.close()

##################### Differences Plot #####################

fig = plt.figure(figsize=(6.5, 4))
fig.subplots_adjust(hspace=0.1, wspace=0.23)

plt.subplot(2, 2, 1)
plt.plot(rd4.Earth.Time / 1e6, merc_de, c=vplot.colors.purple, lw=1, label="Mercury")
plt.plot(rd4.Earth.Time / 1e6, venus_de, c=vplot.colors.orange, lw=1, label="Venus")
plt.plot(rd4.Earth.Time / 1e6, earth_de, c=vplot.colors.pale_blue, lw=1, label="Earth")
plt.plot(rd4.Earth.Time / 1e6, mars_de, c=vplot.colors.red, lw=1, label="Mars")
plt.ylabel("$\Delta e$")
plt.xticks(visible=False)
plt.legend(loc="lower left", fontsize=8, ncol=2)

plt.subplot(2, 2, 3)
plt.plot(rd4.Earth.Time / 1e6, jup_de, c=vplot.colors.purple, lw=1, label="Jupiter")
plt.plot(
    rd4.Earth.Time / 1e6, sat_de, c=vplot.colors.orange, lw=1, zorder=-1, label="Saturn"
)
plt.plot(
    rd4.Earth.Time / 1e6, george_de, c=vplot.colors.pale_blue, lw=1, label="Uranus"
)
plt.plot(rd4.Earth.Time / 1e6, nept_de, c=vplot.colors.red, lw=1, label="Neptune")
plt.ylabel("$\Delta e$")
plt.xlabel("Time (Myr)")
plt.legend(loc="lower left", fontsize=8, ncol=2)


plt.subplot(2, 2, 2)
plt.plot(rd4.Earth.Time / 1e6, merc_di, c=vplot.colors.purple, lw=1)
plt.plot(rd4.Earth.Time / 1e6, venus_di, c=vplot.colors.orange, lw=1)
plt.plot(rd4.Earth.Time / 1e6, earth_di, c=vplot.colors.pale_blue, lw=1)
plt.plot(rd4.Earth.Time / 1e6, mars_di, c=vplot.colors.red, lw=1)
plt.xticks(visible=False)
plt.ylabel("$\Delta i$ ($^{\circ}$)")

plt.subplot(2, 2, 4)
plt.plot(rd4.Earth.Time / 1e6, jup_di, c=vplot.colors.purple, lw=1)
plt.plot(rd4.Earth.Time / 1e6, sat_di, c=vplot.colors.orange, lw=1, zorder=-1)
plt.plot(rd4.Earth.Time / 1e6, george_di, c=vplot.colors.pale_blue, lw=1)
plt.plot(rd4.Earth.Time / 1e6, nept_di, c=vplot.colors.red, lw=1)
plt.ylabel("$\Delta i$ ($^{\circ}$)")
plt.xlabel("Time (Myr)")

# Save
fig.savefig(path / f"SSDistOrbDistRotError.{ext}", bbox_inches="tight", dpi=300)
plt.close()

################### Obliquity #####################

laskar_time_wo = np.array([0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0])
laskar_wo = np.array([23.5, 15, 22, 31.5, 28.5, 23.0, 26.5, 28.0, 22.0, 21, 30.5])

laskar_time_w = np.array(
    [
        0,
        0.05,
        0.1,
        0.15,
        0.2,
        0.25,
        0.3,
        0.35,
        0.4,
        0.45,
        0.5,
        0.55,
        0.6,
        0.65,
        0.7,
        0.75,
        0.8,
        0.85,
        0.9,
        0.95,
        1.0,
    ]
)
laskar_w = np.array(
    [
        23.5,
        22.5,
        23.25,
        23.75,
        23.5,
        22.75,
        23.0,
        23.5,
        23.5,
        23.25,
        23.25,
        23.25,
        23.5,
        24.0,
        23.5,
        22.75,
        23,
        23.5,
        23.5,
        23.0,
        22.5,
    ]
)

# Run vplanet (mars)
vplanet.run(path / "marshnb" / "vpl.in", units=False)
vplanet.run(path / "marsvpl" / "vpl.in", units=False)
tm, oblm, pAm, precf = np.loadtxt(path / "marshnb/solarsys.Mars.backward", unpack=True)
tm1, em1, incm1, argpm1, longam1, oblm1, pAm1, precf1 = np.loadtxt(
    path / "marsvpl/solarsys.Mars.backward", unpack=True
)

# Run vplanet w/ out moon
out2 = vplanet.run(path / "womoon" / "vpl.in", units=False)

fig = plt.figure(figsize=(6.5, 4))
fig.subplots_adjust(wspace=0.25)

plt.subplot(1, 2, 1)
plt.plot(
    rd4.Earth.Time / 1e6,
    rd4.Earth.Obliquity,
    "-",
    c=vplot.colors.pale_blue,
    label="Earth with Moon",
)
plt.plot(
    out2.Earth.Time / 1e6,
    out2.Earth.Obliquity,
    "-",
    c=vplot.colors.dark_blue,
    label="Earth without Moon",
)
plt.plot(laskar_time_w, laskar_w, "o", c=vplot.colors.pale_blue, mfc="None")
plt.plot(laskar_time_wo, laskar_wo, "o", c=vplot.colors.dark_blue, mfc="None")
plt.ylabel(r"$\varepsilon$ ($^{\circ}$)")
plt.xlabel("Time (Myr)")
plt.xlim(0, 1)
plt.legend(loc="upper right", fontsize=10)

ax = plt.subplot(1, 2, 2)
plt.plot(
    tm1 / 1e6,
    oblm1,
    "-",
    color=vplot.colors.orange,
    zorder=100,
    lw=1,
    label="Mars (4th Order+DistRot)",
)
plt.plot(tm / 1e6, oblm, "-", c=vplot.colors.red, lw=1, label="Mars (HNBody+DistRot)")
plt.ylabel(r"$\varepsilon$ ($^{\circ}$)")
plt.xlabel("Time (Myr)")
plt.xlim(-10, 0)
plt.ylim(5, 55)
plt.legend(loc="upper right", fontsize=10)


# Save
fig.savefig(path / f"SSDistOrbDistRotObliq.{ext}", bbox_inches="tight", dpi=300)
plt.close()
