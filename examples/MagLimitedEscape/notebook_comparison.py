#!/usr/bin/env python3
"""Reproduce the Trappist-1e panel of the source notebook (Combined for
Paper, page 27) by sweeping dMagField across the same logarithmic range
and running vplanet at each value. Produces a log-log figure with the
same axes and per-mechanism curves so the result can be compared visually
to the notebook plot.

The notebook reports rates in Earth atmospheres per Gyr; this script
applies the same conversion (1 Earth atm = 1e-6 Mearth = 5.972e18 kg)
to vplanet's per-mechanism particle-flux outputs, with proton mass for
the H -> kg conversion that matches the notebook."""

import os
import shutil
import subprocess
import sys
import tempfile

import matplotlib.pyplot as plt
import numpy as np

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
T1E_DIR = os.path.join(THIS_DIR, "trappist1e")
VPLANET_BINARY = os.path.join(THIS_DIR, "..", "..", "bin", "vplanet")

PROTON_MASS_KG = 1.67e-27
EARTH_ATM_KG = 5.972e18  # 1 Earth atmosphere mass = 1e-6 Mearth
GYR_SECONDS = 3.156e16

# Output column indices for the t1e.in body file (no thermint).
COLUMNS = {
    "Pickup": 3,
    "CrossField": 4,
    "PolarCap": 5,
    "Cusp": 6,
    "Driscoll": 7,
    "Total": 8,
}

# Same range and density as the notebook's magnetic_moments scan.
EARTH_DIPOLE_AM2 = 8.0e22
DA_M_OVER_EARTH = np.logspace(-4, 2, 60)


def fnRunVplanetAtField(daMagFieldEarthUnits, sScratchDir):
    """Copy the T1e example into a scratch dir, swap in dMagField, run."""
    for sFileName in os.listdir(T1E_DIR):
        if sFileName.endswith(".in"):
            shutil.copy(os.path.join(T1E_DIR, sFileName), sScratchDir)
    sBodyFile = os.path.join(sScratchDir, "e.in")
    with open(sBodyFile, "r") as fHandle:
        sContents = fHandle.read()
    daSign = -daMagFieldEarthUnits  # negative -> Earth units in vplanet
    sContents = sContents.replace("dMagField         -1",
                                  f"dMagField         {daSign:.6e}")
    with open(sBodyFile, "w") as fHandle:
        fHandle.write(sContents)
    subprocess.run([VPLANET_BINARY, "vpl.in"], cwd=sScratchDir,
                   check=True, capture_output=True)


def fdaReadInitialOutputs(sScratchDir):
    """Return the t=0 row of the planet forward file."""
    sForwardPath = os.path.join(sScratchDir, "trappist1e.e.forward")
    return np.loadtxt(sForwardPath)[0]


def fdaSweepRates(daFieldsEarthUnits):
    """Run vplanet at each magnetic moment, collect per-mechanism rates."""
    daRates = np.zeros((len(daFieldsEarthUnits), len(COLUMNS)))
    for iIndex, dField in enumerate(daFieldsEarthUnits):
        with tempfile.TemporaryDirectory(prefix="t1e_sweep_") as sScratchDir:
            fnRunVplanetAtField(dField, sScratchDir)
            daInitialRow = fdaReadInitialOutputs(sScratchDir)
        for iCol, sMechanism in enumerate(COLUMNS):
            daRates[iIndex, iCol] = daInitialRow[COLUMNS[sMechanism]]
        if iIndex % 10 == 0:
            print(f"  swept M = {dField:.3e} Earth dipoles "
                  f"({iIndex + 1}/{len(daFieldsEarthUnits)})")
    return daRates


def fdaConvertToAtmPerGyr(daRatesParticlesPerSec):
    """Convert particles/s into Earth atmospheres per Gyr (notebook units).
    The notebook always multiplies by proton mass for the H -> kg conversion."""
    daKgPerSec = np.abs(daRatesParticlesPerSec) * PROTON_MASS_KG
    return daKgPerSec * GYR_SECONDS / EARTH_ATM_KG


def fnPlotComparison(daFields, daRates, sOutputPath):
    """Reproduce the notebook's T1e log-log plot.

    Total H = sum(Pickup + CrossField + PolarCap + Cusp), matching the
    notebook's total_escape_TE definition. The Driscoll mechanism is shown
    separately and not included in the H total (also matching the notebook,
    since its sign can flip)."""
    listHMechanisms = ["Pickup", "CrossField", "PolarCap", "Cusp"]
    daTotalParticles = np.zeros(len(daFields))
    for sMechanism in listHMechanisms:
        daTotalParticles = daTotalParticles + np.abs(
            daRates[:, list(COLUMNS.keys()).index(sMechanism)])

    fig, ax = plt.subplots(figsize=(9, 6))
    dictColors = {"Pickup": "orange", "CrossField": "blue",
                  "PolarCap": "purple", "Cusp": "deeppink",
                  "Driscoll": "gold"}
    dictStyles = {"Pickup": "--", "CrossField": "-", "PolarCap": "-",
                  "Cusp": "--", "Driscoll": "-"}
    for sMechanism in ["Pickup", "CrossField", "PolarCap", "Cusp",
                       "Driscoll"]:
        iIndex = list(COLUMNS.keys()).index(sMechanism)
        daYAtmGyr = fdaConvertToAtmPerGyr(daRates[:, iIndex])
        ax.loglog(daFields, daYAtmGyr,
                  color=dictColors[sMechanism],
                  linestyle=dictStyles[sMechanism],
                  linewidth=1.5,
                  label=f"{sMechanism} H Escape")
    daTotalAtmGyr = fdaConvertToAtmPerGyr(daTotalParticles)
    ax.loglog(daFields, daTotalAtmGyr, color="black", linewidth=2.5,
              label="Total H Escape")

    ax.set_xlabel("Magnetic Dipole Moment [Earth Units]")
    ax.set_ylabel("Escape Rate [Earth atm / Gyr]")
    ax.set_title("Trappist-1e: vplanet reproduction of notebook page 27")
    ax.set_xlim(1e-4, 1e2)
    ax.set_ylim(1e-35, 1e5)
    ax.legend(loc="lower left", fontsize=10, framealpha=0.95)
    ax.grid(alpha=0.3, which="both")
    fig.tight_layout()
    fig.savefig(sOutputPath, dpi=150)
    print(f"Wrote {sOutputPath}")


if __name__ == "__main__":
    sCachePath = os.path.join(THIS_DIR, "t1e_sweep_cache.npz")
    if os.path.exists(sCachePath):
        print(f"Loading cached sweep from {sCachePath}")
        dictCache = np.load(sCachePath)
        daFields = dictCache["fields"]
        daResultRates = dictCache["rates"]
    else:
        print(f"Sweeping {len(DA_M_OVER_EARTH)} magnetic moments for T1e...")
        daFields = DA_M_OVER_EARTH
        daResultRates = fdaSweepRates(daFields)
        np.savez(sCachePath, fields=daFields, rates=daResultRates)
        print(f"Cached results to {sCachePath}")
    sOutputPath = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        THIS_DIR, "T1e_NotebookComparison.png")
    fnPlotComparison(daFields, daResultRates, sOutputPath)
