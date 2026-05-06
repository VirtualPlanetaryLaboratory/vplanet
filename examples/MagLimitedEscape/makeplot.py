#!/usr/bin/env python3
"""Plot the Gunell+2018 magnetic-limited escape diagnostics for all three
MagLimitedEscape simulations.

Left panel: per-mechanism mass-loss rate at the simulation start for each
of Trappist-1e, Earth at constant 1 Earth dipole, and Earth coupled to
thermint. Mechanisms are converted from particles/s to kg/s of hydrogen
(proton mass) for consistency.

Right panel: time evolution of the magnetic moment (in Earth dipoles) and
total bulk atmospheric loss rate for the thermint-coupled Earth, showing
the geodynamo decay from ~1.7 -> ~1.0 Earth units across 4.5 Gyr."""

import os
import sys

import matplotlib.pyplot as plt
import numpy as np

try:
    import vplot  # noqa: F401  - imported for matplotlib style registration
    if "vplot" in plt.style.available:
        plt.style.use("vplot")
except ImportError:
    pass

PROTON_MASS = 1.67e-27
THIS_DIR = os.path.dirname(os.path.abspath(__file__))

COLUMNS_NO_THERMINT = {
    "MagField": 1,
    "MagPauseRadAtmEsc": 2,
    "Pickup": 3,
    "CrossField": 4,
    "PolarCap": 5,
    "Cusp": 6,
    "Driscoll": 7,
    "TotalLoss": 8,
}
COLUMNS_THERMINT = {
    "MagMom": 1,
    "MagField": 2,
    "MagPauseRadAtmEsc": 3,
    "Pickup": 4,
    "CrossField": 5,
    "PolarCap": 6,
    "Cusp": 7,
    "Driscoll": 8,
    "TotalLoss": 9,
}

SIMULATIONS = [
    {
        "label": "Trappist-1e (B = 1 Earth)",
        "forward": "trappist1e/trappist1e.e.forward",
        "columns": COLUMNS_NO_THERMINT,
    },
    {
        "label": "Earth (B = 1 Earth, const)",
        "forward": "earth_const/earth_const.earth.forward",
        "columns": COLUMNS_NO_THERMINT,
    },
    {
        "label": "Earth (B from thermint)",
        "forward": "earth_thermint/earth_thermint.earth.forward",
        "columns": COLUMNS_THERMINT,
    },
]

MECHANISMS = ["Pickup", "CrossField", "PolarCap", "Cusp", "Driscoll"]


def fdaLoadForward(sRelativePath):
    """Read a vplanet forward file as a 2-D numpy array."""
    sFullPath = os.path.join(THIS_DIR, sRelativePath)
    return np.loadtxt(sFullPath)


def fnPlotMechanismBars(ax, listSimulations):
    """Bar chart: per-mechanism H mass-loss rate at t=0 for each simulation."""
    iMechanismCount = len(MECHANISMS)
    iSimulationCount = len(listSimulations)
    daBarWidth = 0.8 / iSimulationCount
    daXBase = np.arange(iMechanismCount)

    for iSim, dictSim in enumerate(listSimulations):
        daRows = fdaLoadForward(dictSim["forward"])
        daInitial = daRows[0]
        daRates = np.array([daInitial[dictSim["columns"][m]] for m in MECHANISMS])
        daKgPerSec = np.abs(daRates) * PROTON_MASS
        ax.bar(daXBase + iSim * daBarWidth, daKgPerSec, daBarWidth,
               label=dictSim["label"])

    ax.set_yscale("log")
    ax.set_xticks(daXBase + daBarWidth * (iSimulationCount - 1) / 2)
    ax.set_xticklabels(MECHANISMS, rotation=20)
    ax.set_ylabel("Hydrogen mass loss rate (kg/s)")
    ax.set_title("Per-mechanism rate at t = 0")
    ax.legend(fontsize=8)
    ax.grid(alpha=0.3, axis="y")


def fnPlotThermintEvolution(ax, sForwardPath, dictColumns):
    """Twin-axis plot: magnetic moment and total bulk loss rate vs time."""
    daRows = fdaLoadForward(sForwardPath)
    daTimeGyr = daRows[:, 0] / 1e9
    daMagMom = daRows[:, dictColumns["MagField"]]
    daTotalLoss = daRows[:, dictColumns["TotalLoss"]]

    ax.plot(daTimeGyr, daMagMom, color="C0", linewidth=2,
            label="Magnetic moment (Earth dipoles)")
    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Magnetic moment (Earth dipoles)", color="C0")
    ax.tick_params(axis="y", labelcolor="C0")
    ax.grid(alpha=0.3)

    axTwin = ax.twinx()
    axTwin.plot(daTimeGyr, daTotalLoss, color="C3", linewidth=2,
                linestyle="--", label="Total bulk loss (kg/s)")
    axTwin.set_ylabel("Total bulk loss rate (kg/s)", color="C3")
    axTwin.tick_params(axis="y", labelcolor="C3")
    ax.set_title("Earth + thermint: dynamo evolution")


def fnSavePlot(sOutputPath):
    """Build the figure and write it to disk."""
    fig, (axLeft, axRight) = plt.subplots(1, 2, figsize=(13, 5))
    fnPlotMechanismBars(axLeft, SIMULATIONS)
    dictThermintSim = SIMULATIONS[-1]
    fnPlotThermintEvolution(axRight, dictThermintSim["forward"],
                            dictThermintSim["columns"])
    fig.suptitle("Gunell+2018 magnetic-limited escape", fontsize=13)
    fig.tight_layout(rect=(0, 0, 1, 0.96))
    fig.savefig(sOutputPath, dpi=150)
    print(f"Wrote {sOutputPath}")


if __name__ == "__main__":
    sOutput = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        THIS_DIR, "MagLimitedEscape.png")
    fnSavePlot(sOutput)
