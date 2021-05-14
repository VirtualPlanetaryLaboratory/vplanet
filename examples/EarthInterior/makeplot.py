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
plt.rcParams.update({"font.size": 16, "legend.fontsize": 15})

# Run vplanet
out = vplanet.run(path / "vpl.in", units=False)


### Uncertainty ranges
TUMan_ra = np.array([1280.0, 1475.0]) + 273.0  # [K] Jaupart (2015) Table 4.
TCMB_ra = np.array([3800, 4200.0])  # [K] Hirose (2013) Table 2.
HflowUMan_ra = np.array([35, 41.0])  # [TW] Jaupart (2015) Table 12.
HflowUMan_ra = np.array([35, 41.0])  # [TW] Jaupart (2015) Table 12.
HflowCMB_ra = np.array([5, 17])  # [TW] Jaupart (2015) Table 12.
ViscUMan_ra = np.array([1.5e19, 1.5e22]) / 3300.0  # [m^2/s] Paulson (2005) Fig 3.
ViscLMan_ra = np.array([3e19, 1.5e22]) / 5200.0  # [m^2/s] Paulson (2005) Fig 3.
MeltMassFlux_ra = np.array(
    [0.52, 4 * 0.52]
)  # [1e6 kg/s] Cogne (2004) 5-15 km^3/yr.  Li (2015) ~20 km^3/yr
FMeltUMan_ra = np.array([0.07, 0.15])  # refs?

### Hi/lo
TUMan_lo = np.abs(TUMan_ra[0] - out.earth.TUMan[-1])
TUMan_hi = np.abs(TUMan_ra[1] - out.earth.TUMan[-1])
TCMB_lo = np.abs(TCMB_ra[0] - out.earth.TCMB[-1])
TCMB_hi = np.abs(TCMB_ra[1] - out.earth.TCMB[-1])
HflowUMan_lo = np.abs(HflowUMan_ra[0] - out.earth.HflowUMan[-1])
HflowUMan_hi = np.abs(HflowUMan_ra[1] - out.earth.HflowUMan[-1])
HflowCMB_lo = np.abs(HflowCMB_ra[0] - out.earth.HflowCMB[-1])
HflowCMB_hi = np.abs(HflowCMB_ra[1] - out.earth.HflowCMB[-1])
ViscUMan_lo = np.abs(ViscUMan_ra[0] - out.earth.ViscUMan[-1])
ViscUMan_hi = np.abs(ViscUMan_ra[1] - out.earth.ViscUMan[-1])
ViscLMan_lo = np.abs(ViscLMan_ra[0] - out.earth.ViscLMan[-1])
ViscLMan_hi = np.abs(ViscLMan_ra[1] - out.earth.ViscLMan[-1])
MeltMassFlux_lo = np.abs(MeltMassFlux_ra[0] - out.earth.MeltMassFluxMan[-1] * 1e-6)
MeltMassFlux_hi = np.abs(MeltMassFlux_ra[1] - out.earth.MeltMassFluxMan[-1] * 1e-6)
FMeltUMan_lo = np.abs(FMeltUMan_ra[0] - out.earth.FMeltUMan[-1])
FMeltUMan_hi = np.abs(FMeltUMan_ra[1] - out.earth.FMeltUMan[-1])

# Plots
rows = 3
cols = 2

# Mantle Figure
nfig = 1
fig = plt.figure(nfig, figsize=(10, 15))
panel = 1
plt.subplot(rows, cols, panel)
plt.plot(
    out.earth.Time,
    out.earth.TMan,
    color=vplot.colors.red,
    linestyle="-",
    label=r"$T_{M}$",
)
plt.plot(
    out.earth.Time,
    out.earth.TUMan,
    color=vplot.colors.orange,
    linestyle="-",
    label=r"$T_{UM}$",
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.TUMan[-1],
    yerr=[[TUMan_lo], [TUMan_hi]],
    color=vplot.colors.orange,
    fmt="o",
)
plt.plot(
    out.earth.Time,
    out.earth.TLMan,
    color=vplot.colors.pale_blue,
    linestyle="-",
    label=r"$T_{LM}$",
)
plt.plot(
    out.earth.Time,
    out.earth.TCMB,
    color=vplot.colors.purple,
    linestyle="-",
    label=r"$T_{CMB}$",
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.TCMB[-1],
    yerr=[[TCMB_lo], [TCMB_hi]],
    color=vplot.colors.purple,
    fmt="-o",
)
plt.plot(out.earth.Time, out.earth.TCore, "k-", label=r"$T_{C}$")
plt.legend(loc="best", ncol=2, frameon=True, columnspacing=1)
plt.ylabel("Temperature (K)")
plt.xlabel("Time (Gyr)")
plt.ylim(0, 10000)
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(
    out.earth.Time,
    out.earth.HflowUMan,
    color=vplot.colors.red,
    linestyle="-",
    label=r"$Q_{UMan}$",
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.HflowUMan[-1],
    yerr=[[HflowUMan_lo], [HflowUMan_hi]],
    color=vplot.colors.red,
    fmt="o",
)
plt.plot(
    out.earth.Time,
    out.earth.HflowCMB,
    color=vplot.colors.orange,
    linestyle="-",
    label=r"$Q_{CMB}$",
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.HflowCMB[-1],
    yerr=[[HflowCMB_lo], [HflowCMB_hi]],
    color=vplot.colors.orange,
    fmt="o",
)
plt.plot(
    out.earth.Time,
    out.earth.RadPowerMan,
    color=vplot.colors.pale_blue,
    linestyle="-",
    label=r"$Q_{Rad,Man}$",
)
plt.plot(out.earth.Time, out.earth.RadPowerCore, "k-", label=r"$Q_{Rad,Core}$")
plt.legend(loc="best", frameon=True)
plt.ylabel("Heat Flow (TW)")
plt.xlabel("Time (Gyr)")
plt.ylim(0, 150)
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(
    out.earth.Time,
    out.earth.BLUMan,
    label=r"$\delta_{UM}$",
    color=vplot.colors.dark_blue,
)
plt.plot(
    out.earth.Time, out.earth.BLLMan, label=r"$\delta_{LM}$", color=vplot.colors.orange
)
plt.legend(loc="best", frameon=True)
plt.ylabel(r"Boundary Layer Depths (km)")
plt.xlabel("Time (Gyr)")
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.semilogy(
    out.earth.Time,
    out.earth.ViscUMan,
    label=r"$\nu_{UM}$",
    color=vplot.colors.dark_blue,
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.ViscUMan[-1],
    yerr=[[ViscUMan_lo], [ViscUMan_hi]],
    color=vplot.colors.dark_blue,
    fmt="o",
)
plt.semilogy(
    out.earth.Time, out.earth.ViscLMan, label=r"$\nu_{LM}$", color=vplot.colors.orange
)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.ViscLMan[-1],
    yerr=[[ViscLMan_lo], [ViscLMan_hi]],
    color=vplot.colors.orange,
    fmt="o",
)
plt.legend(loc="best", frameon=True)
plt.ylabel(r"Mantle Viscosity ($m^2s^{-1}$)")
plt.xlabel("Time (Gyr)")
plt.ylim(1e12, 1e19)
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.FMeltUMan, color=vplot.colors.dark_blue)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.FMeltUMan[-1] * 1e-6,
    yerr=[[FMeltUMan_lo], [FMeltUMan_hi]],
    color=vplot.colors.dark_blue,
    fmt="o",
)
plt.ylabel(r"Melt Fraction Upper Mantle (n.d.)")
plt.xlabel("Time (Gyr)")
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.MeltMassFluxMan * 1e-6, color=vplot.colors.dark_blue)
plt.errorbar(
    out.earth.Time[-1],
    out.earth.MeltMassFluxMan[-1] * 1e-6,
    yerr=[[MeltMassFlux_lo], [MeltMassFlux_hi]],
    color=vplot.colors.dark_blue,
    fmt="o",
)
plt.ylabel(r"Melt Mass Flux Mantle ($\times 10^6$ kg$/$s)")
plt.xlabel("Time (Gyr)")
plt.ylim(0, 100)
plt.xlim(0, 4.6)
plt.xticks([0, 1, 2, 3, 4])

# Save the figure
ext = get_args().ext
fig.savefig(path / f"EarthInterior{nfig}.{ext}", bbox_inches="tight", dpi=600)

# Core Plots
rows = 2
nfig += 1
fig = plt.figure(nfig, figsize=(10, 10))
panel = 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.RIC, label="RIC")
plt.ylim(0, 1500)
plt.ylabel(r"Inner Core Radius (km)")
plt.xlabel("Time (Gyr)")
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.CoreBuoyTherm * 1e13, label="Thermal")
plt.plot(out.earth.Time, out.earth.CoreBuoyCompo * 1e13, label="Compositional")
plt.plot(out.earth.Time, out.earth.CoreBuoyTotal * 1e13, label="Total")
plt.legend(loc="best", frameon=True)
plt.ylabel(r"Core Buoyancy Flux ($\times10^{-13}$ m$^2/$s$^3$)")
plt.xlabel("Time (Gyr)")
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.MagMom, label="MagMom")
plt.ylim(0, 2)
plt.ylabel("Magnetic Moment (E. Units)")
plt.xlabel("Time (Gyr)")
plt.xticks([0, 1, 2, 3, 4])
panel += 1
plt.subplot(rows, cols, panel)
plt.plot(out.earth.Time, out.earth.MagPauseRad)
plt.ylabel(r"Magnetopause Radius (E. Units)")
plt.xlabel("Time (Gyr)")
plt.xticks([0, 1, 2, 3, 4])

# Save the figure
fig.savefig(path / f"EarthInterior.{ext}", bbox_inches="tight", dpi=600)
