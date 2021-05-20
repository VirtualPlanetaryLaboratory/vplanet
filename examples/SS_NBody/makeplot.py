import vplanet
import vplot as vpl
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pathlib
import sys
from matplotlib.ticker import FormatStrFormatter

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args


class Planet:
    a = 0.0
    e = 0.0
    i = 0.0
    LongP = 0.0
    LongA = 0.0
    MeanA = 0.0
    x1 = 0.0
    x2 = 0.0
    x3 = 0.0
    v1 = 0.0
    v2 = 0.0
    v3 = 0.0
    Time = []
    Mass = 0.0


def ReadHNBody(FileName):
    planet = Planet()
    (
        planet.Time,
        planet.a,
        planet.e,
        planet.i,
        planet.LongP,
        planet.LongA,
        planet.MeanA,
        planet.x1,
        planet.x2,
        planet.x3,
        planet.v1,
        planet.v2,
        planet.v3,
    ) = np.loadtxt(FileName, skiprows=17, unpack=True)
    planet.MeanA[planet.MeanA < 0] += 360
    planet.LongP[planet.LongP < 0] += 360
    planet.LongA[planet.LongA < 0] += 360
    planet.a = planet.a
    return planet


# Run vplanet
SS = vplanet.run(path / "vpl.in", units=False)

# Get hnbody results
# TODO: Commit these?!
hnEarth = ReadHNBody(path / "plan3.dat")
hnEarth.Mass = 1.0


fig, ([ax1, ax2], [ax3, ax4], [ax5, ax6]) = plt.subplots(3, 2, figsize=[14, 16])

ax1.plot(hnEarth.Time, hnEarth.a, color=vpl.colors.red)
ax1.plot(SS.Earth.Time, SS.Earth.SemiMajorAxis, "k")
ax1.set_rasterization_zorder(0)
ax1.set_ylabel("Semi-Major Axis (AU)")
ax1.yaxis.set_major_formatter(FormatStrFormatter("%.6f"))

ax2.plot(hnEarth.Time, hnEarth.e, color=vpl.colors.red)
ax2.plot(SS.Earth.Time, SS.Earth.Eccentricity, "k")
ax2.set_ylabel("Eccentricity")

plt.subplot(3, 2, 3)
plt.plot(hnEarth.Time, hnEarth.i, color=vpl.colors.red)
plt.plot(SS.Earth.Time, SS.Earth.SpiNBodyInc, "k")
plt.legend(["HNBody", "VPLanet"], loc="upper left")
plt.ylabel("Inclination ($^\circ$)")

plt.subplot(3, 2, 4)
plt.plot(hnEarth.Time, hnEarth.LongP, color=vpl.colors.red, marker=".", linewidth=0)
plt.plot(SS.Earth.Time, SS.Earth.LongP, "k", marker=".", linewidth=0)
plt.xlabel("Time (yrs)")
plt.ylim(0, 360)
plt.ylabel("Longitude of Pericenter ($^\circ$)")

plt.subplot(3, 2, 5)
plt.plot(hnEarth.Time, hnEarth.LongA, color=vpl.colors.red, marker=".", linewidth=0)
plt.plot(SS.Earth.Time, SS.Earth.SpiNBodyLongA, color="k", marker=".", linewidth=0)
plt.ylim(0, 360)
plt.ylabel("Longitude of Ascending Node ($^\circ$)")
plt.xlabel("Time (yrs)")


etot = (SS.Sun.TotOrbEnergy / SS.Sun.TotOrbEnergy[0] - 1) * 1e9
jtot = (SS.Sun.TotAngMom / SS.Sun.TotAngMom[0] - 1) * 1e11

plt.subplot(3, 2, 6)
plt.plot(SS.Sun.Time, etot, color=vpl.colors.orange, label=(r"Energy $\times 10^9$"))
plt.plot(
    SS.Sun.Time, jtot, color=vpl.colors.purple, label=(r"Ang. Mom. $\times 10^{11}$")
)
plt.xlabel("Time (yr)", fontsize=16)
plt.ylabel(r"$(\Delta E / E - 1)/10^{9}$", fontsize=16)
plt.ylabel(r"$\Delta$E,  $\Delta$J")
plt.xlabel("Time (yrs)")
plt.legend(loc="upper left")

# Save the figure
ext = get_args().ext
fig.savefig(path / f"SS_NBody.{ext}")
