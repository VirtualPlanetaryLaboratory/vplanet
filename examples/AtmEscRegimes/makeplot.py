"""
This script produces two figures examining the mass loss of a Earth-mass core with
an extended H envelope under various atmospheric escape regimes, e.g. energy
and radiation/recombination-limited escape.

David P. Fleming, University of Washington, 2019
"""
import vplanet
import vplot
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import sys
import pathlib

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Typical plot parameters that make for pretty plot
mpl.rcParams["figure.figsize"] = (10, 8)
mpl.rcParams["font.size"] = 15.0

# Run vplanet
regimes = vplanet.run(path / "Regimes" / "vpl.in", quiet=True)
bondi = vplanet.run(path / "Bondi" / "vpl.in", quiet=True)

### First figure: Comparative atmospheric escape ###

# Plot
fig, axes = plt.subplots(nrows=2, ncols=3, sharex=True, figsize=(16, 9))

time = regimes.auto.Time

## Upper left: Envelope mass fraction ##
axes[0, 0].plot(time, regimes.auto.EnvelopeMass / regimes.auto.Mass, color="C0")
axes[0, 0].plot(time, regimes.el.EnvelopeMass / regimes.el.Mass, color="C1")
axes[0, 0].plot(time, regimes.rr.EnvelopeMass / regimes.rr.Mass, color="C2")
axes[0, 0].plot(time, regimes.bondi.EnvelopeMass / regimes.bondi.Mass, color="C3")

# Initial value
axes[0, 0].axhline(
    regimes.auto.EnvelopeMass[0] / regimes.auto.Mass[0], color="k", ls="--"
)

# Format
axes[0, 0].set_ylim(-0.02, 0.52)
axes[0, 0].set_ylabel("Envelope Mass Fraction")

## Upper middle: Envelope mass time derivative ##
axes[0, 1].plot(time, regimes.auto.DEnvMassDt, color="C0", label="AtmEscAuto")
axes[0, 1].plot(time, regimes.el.DEnvMassDt, color="C1", label="Energy-Limited")
axes[0, 1].plot(
    time, regimes.rr.DEnvMassDt, color="C2", label="Radiation/Recombination-\nLimited"
)
axes[0, 1].plot(time, regimes.bondi.DEnvMassDt, color="C3", label="Bondi-Limited")

# Format
axes[0, 1].set_ylim(-1e12, -1.0e5)
axes[0, 1].set_yscale("symlog", linthresh=0.1)
axes[0, 1].set_ylabel(r"$\dot{M}_{envelope}$ [kg/s]")
axes[0, 1].legend(loc="upper left", framealpha=0, fontsize=12, ncol=1)

## Upper left: planetary mass ##
axes[0, 2].plot(time, regimes.auto.Mass, color="C0")
axes[0, 2].plot(time, regimes.el.Mass, color="C1")
axes[0, 2].plot(time, regimes.rr.Mass, color="C2")
axes[0, 2].plot(time, regimes.bondi.Mass, color="C3")

# Initial value
axes[0, 2].axhline(regimes.auto.Mass[0], color="k", ls="--")

# Format
axes[0, 2].set_ylabel(r"Mass [M$_{\oplus}$]")

## Lower left: Planet Radius ##
axes[1, 0].plot(time, regimes.auto.PlanetRadius, color="C0", label="")
axes[1, 0].plot(time, regimes.el.PlanetRadius, color="C1", label="")
axes[1, 0].plot(time, regimes.rr.PlanetRadius, color="C2", label="")
axes[1, 0].plot(time, regimes.bondi.PlanetRadius, color="C3", label="")
axes[1, 0].plot(time, regimes.rad.PlanetRadius, color="k", label="No Escape")

# Initial value
axes[1, 0].axhline(regimes.auto.PlanetRadius[0], color="k", ls="--", label="")

# Format
axes[1, 0].set_ylim(0, 35)
axes[1, 0].set_ylabel(r"Radius [R$_{\oplus}$]")
axes[1, 0].set_xlabel("Time [yr]")
axes[1, 0].legend(bbox_to_anchor=[0.98, 0.92], fontsize=13, framealpha=0)

## Lower middle: Critical FXUV ##
axes[1, 1].plot(time, regimes.auto.RRCriticalFlux, color="C0", label="")
axes[1, 1].plot(time, regimes.el.RRCriticalFlux, color="C1", label="")
axes[1, 1].plot(time, regimes.rr.RRCriticalFlux, color="C2", label="")
axes[1, 1].plot(time, regimes.bondi.RRCriticalFlux, color="C3", label="")
axes[1, 1].plot(time, regimes.auto.FXUV, color="k", label="Incident\nXUV Flux")

# Format
axes[1, 1].set_xlabel("Time [yr]")
axes[1, 1].set_ylim(1.0e-2, 200)
axes[1, 1].set_yscale("log")
axes[1, 1].set_ylabel(r"Critical XUV Flux [W/m$^2$]")
axes[1, 1].legend(bbox_to_anchor=[0.4, 0.8], framealpha=0, fontsize=13)

## Lower right: Escape regime ##
axes[1, 2].plot(time, regimes.auto.HEscapeRegime, lw=2, color="C0", label="")
axes[1, 2].plot(time, regimes.el.HEscapeRegime, lw=2, color="C1", label="")
axes[1, 2].plot(time, regimes.rr.HEscapeRegime, lw=2, color="C2", label="")
axes[1, 2].plot(time, regimes.bondi.HEscapeRegime, lw=2, color="C3", label="")

# Annotate with the regime
axes[1, 2].text(
    0.03,
    0.8,
    "Envelope Lost",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)
axes[1, 2].text(
    0.03,
    0.58,
    "RR-Limited",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)
axes[1, 2].text(
    0.03,
    0.25,
    "Energy-Limited",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)

# Format
axes[1, 2].set_xlabel("Time [yr]")
axes[1, 2].set_ylabel("Escape Regime")
axes[1, 2].set_yticks([])
axes[1, 2].set_ylim(1, 10)

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time[1], time.max())
    ax.set_xscale("log")

    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
fig.subplots_adjust(wspace=0.25)
fig.subplots_adjust(hspace=0.05)

# Save
ext = get_args().ext
fig.savefig(path / f"AtmEscRegimes.{ext}", bbox_inches="tight", dpi=200)

### Second figure: Zoom-in on Bondi-limited escape ###

# Plot
fig, axes = plt.subplots(nrows=2, ncols=3, sharex=True, figsize=(16, 9))

time = bondi.auto.Time

## Upper left: Envelope mass fraction ##
axes[0, 0].plot(time, bondi.auto.EnvelopeMass / bondi.auto.Mass, color="C0")
axes[0, 0].plot(time, bondi.bondi.EnvelopeMass / bondi.bondi.Mass, color="C3")

# Initial value
axes[0, 0].axhline(bondi.auto.EnvelopeMass[0] / bondi.auto.Mass[0], color="k", ls="--")

# Format
axes[0, 0].set_ylim(-0.02, 0.52)
axes[0, 0].set_ylabel("Envelope Mass Fraction")

## Upper middle: Envelope mass time derivative ##
axes[0, 1].plot(time, bondi.auto.DEnvMassDt, color="C0", label="AtmEscAuto")
axes[0, 1].plot(time, bondi.bondi.DEnvMassDt, color="C3", label="Bondi-Limited")

# Format
axes[0, 1].set_ylim(-1.0e15, -1.0e9)
axes[0, 1].set_yscale("symlog", linthresh=0.1)
axes[0, 1].set_ylabel(r"$\dot{M}_{envelope}$ [kg/s]")
axes[0, 1].legend(loc="upper left", framealpha=0, fontsize=12, ncol=1)

## Upper left: planetary mass ##
axes[0, 2].plot(time, bondi.auto.Mass, color="C0")
axes[0, 2].plot(time, bondi.bondi.Mass, color="C3")

# Initial value
axes[0, 2].axhline(bondi.auto.Mass[0], color="k", ls="--")

# Format
axes[0, 2].set_ylabel(r"Mass [M$_{\oplus}$]")

## Lower left: Planet Radius ##
axes[1, 0].plot(time, bondi.auto.PlanetRadius, color="C0", label="")
axes[1, 0].plot(time, bondi.bondi.PlanetRadius, color="C3", label="")

# Initial value
axes[1, 0].axhline(bondi.auto.PlanetRadius[0], color="k", ls="--", label="")

# Format
axes[1, 0].set_ylim(0, 35)
axes[1, 0].set_ylabel(r"Radius [R$_{\oplus}$]")
axes[1, 0].set_xlabel("Time [yr]")

## Lower middle: Critical FXUV ##
axes[1, 1].plot(time, bondi.auto.RRCriticalFlux, color="C0", label="")
axes[1, 1].plot(time, bondi.bondi.RRCriticalFlux, color="C3", label="")
axes[1, 1].plot(time, bondi.auto.FXUV, color="k", label="Incident\nXUV Flux")

# Format
axes[1, 1].set_xlabel("Time [yr]")
axes[1, 1].set_ylim(1.0e-2, 200)
axes[1, 1].set_yscale("log")
axes[1, 1].set_ylabel(r"Critical XUV Flux [W/m$^2$]")
axes[1, 1].legend(bbox_to_anchor=[0.4, 0.8], framealpha=0, fontsize=13)

## Lower right: Escape regime ##
axes[1, 2].plot(time, bondi.auto.HEscapeRegime, lw=2, color="C0", label="")
axes[1, 2].plot(time, bondi.bondi.HEscapeRegime, lw=2, color="C3", label="")

# Annotate with the regime
axes[1, 2].text(
    0.7,
    0.8,
    "Envelope Lost",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)
axes[1, 2].text(
    0.7,
    0.58,
    "RR-Limited",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)
axes[1, 2].text(
    0.03,
    0.48,
    "Bondi-Limited",
    horizontalalignment="left",
    verticalalignment="center",
    transform=axes[1, 2].transAxes,
    fontsize=14,
)

# Format
axes[1, 2].set_xlabel("Time [yr]")
axes[1, 2].set_ylabel("Escape Regime")
axes[1, 2].set_yticks([])
axes[1, 2].set_ylim(1, 10)

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time[1], time.max())
    ax.set_xscale("log")

    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
fig.subplots_adjust(wspace=0.25)
fig.subplots_adjust(hspace=0.05)

# Save
fig.savefig(path / f"AtmEscRegimesZoom.{ext}", bbox_inches="tight", dpi=200)
