#!/usr/bin/env python3
"""Generate the three example figures for MagLimitedEscape:

- Earth.png          Magnetic-moment sweep for modern Earth (Gunell18).
- TRAPPIST1e.png     Magnetic-moment sweep for Trappist-1e (Gunell18).
- EarthEvolution.png Four-panel time evolution comparing the constant-B
                     Earth run against the thermint-coupled Earth run.

Run from inside the MagLimitedEscape directory: ``python makeplot.py``.
The two sweeps run vplanet 60 times each at logarithmically spaced
magnetic moments and cache the results to ``.npz`` files; delete the
caches to force a re-sweep."""

import os
import shutil
import subprocess
import sys
import tempfile

import matplotlib.pyplot as plt
import numpy as np
import vplot
from vplot import colors as vp


PROTON_MASS_KG = 1.67e-27
EARTH_ATM_KG = 5.972e18
GYR_SECONDS = 3.156e16
MAG_NUM_WELL_MIXED_SPECIES = 5

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
VPLANET_BINARY = os.path.join(THIS_DIR, "..", "..", "bin", "vplanet")
DA_M_OVER_EARTH = np.logspace(-4, 2, 60)

EARTH_CONST_FORWARD = os.path.join(
      THIS_DIR, "earth_const", "earth_const.earth.forward")
EARTH_THERMINT_FORWARD = os.path.join(
      THIS_DIR, "earth_thermint", "earth_thermint.earth.forward")

H_MECHANISMS = ["Pickup", "CrossField", "PolarCap", "Cusp"]
ALL_MECHANISMS = H_MECHANISMS + ["Driscoll"]
MECHANISM_COLORS = {"Pickup": vp.orange, "CrossField": vp.dark_blue,
                    "PolarCap": vp.purple, "Cusp": vp.red,
                    "Driscoll": vp.pale_blue}
MECHANISM_LINESTYLES = {"Pickup": "--", "CrossField": "-", "PolarCap": "-",
                        "Cusp": "--", "Driscoll": "-"}

# Column index per mechanism in a forward file produced by an
# AtmEsc-only body (T1e, earth_const).
COLUMNS = {"Pickup": 3, "CrossField": 4, "PolarCap": 5, "Cusp": 6,
           "Driscoll": 7, "Total": 8}

# Earth_const has no MagMom column; earth_thermint inserts MagMom
# at index 1 and adds TMan/TCore/RIC at the tail.
COLS_CONST = {"Time": 0, "MagField": 1, "Pickup": 3, "CrossField": 4,
              "PolarCap": 5, "Cusp": 6, "Driscoll": 7, "Total": 8}
COLS_THERMINT = {"Time": 0, "MagField": 2, "Pickup": 4, "CrossField": 5,
                 "PolarCap": 6, "Cusp": 7, "Driscoll": 8, "Total": 9,
                 "TMan": 15, "TCore": 16, "RIC": 17}

PANEL_MECHANISMS = ["PolarCap", "CrossField", "Cusp"]

RC_OVERRIDES = {
      "axes.labelsize": 17,
      "xtick.labelsize": 14,
      "ytick.labelsize": 14,
      "legend.fontsize": 13,
      "lines.linewidth": 2.0,
}

SWEEPS = [
      {"sFilename": "TRAPPIST1e.png",
       "sShortName": "t1e",
       "sSourceDir": os.path.join(THIS_DIR, "trappist1e"),
       "sBodyFile": "e.in",
       "sForwardFile": "trappist1e.e.forward",
       "sCachePath": os.path.join(THIS_DIR, ".t1e_sweep_cache.npz")},
      {"sFilename": "Earth.png",
       "sShortName": "earth",
       "sSourceDir": os.path.join(THIS_DIR, "earth_const"),
       "sBodyFile": "earth.in",
       "sForwardFile": "earth_const.earth.forward",
       "sCachePath": os.path.join(THIS_DIR, ".earth_sweep_cache.npz")},
]


# ---------------------- vplanet sweep helpers ----------------------

def fnSubstituteMagField(sBodyPath, dFieldEarthUnits):
    """Replace the dMagField line in body.in with -<value>."""
    with open(sBodyPath, "r") as fHandle:
        listLines = fHandle.readlines()
    sNewValue = f"{-dFieldEarthUnits:.6e}"
    for iIndex, sLine in enumerate(listLines):
        if sLine.lstrip().startswith("dMagField"):
            listLines[iIndex] = f"dMagField               {sNewValue}\n"
            break
    with open(sBodyPath, "w") as fHandle:
        fHandle.writelines(listLines)


def fnShortenStopTime(sVplPath):
    """Shrink dStopTime in vpl.in to 1e7 yr; we only read t=0."""
    with open(sVplPath, "r") as fHandle:
        listLines = fHandle.readlines()
    for iIndex, sLine in enumerate(listLines):
        if sLine.lstrip().startswith("dStopTime"):
            listLines[iIndex] = "dStopTime       1e7\n"
        elif sLine.lstrip().startswith("dOutputTime"):
            listLines[iIndex] = "dOutputTime     1e7\n"
    with open(sVplPath, "w") as fHandle:
        fHandle.writelines(listLines)


def fnRunVplanetAtField(dictConfig, dFieldEarthUnits, sScratchDir):
    """Copy the source dir, swap in dMagField, run vplanet."""
    for sFileName in os.listdir(dictConfig["sSourceDir"]):
        if sFileName.endswith(".in"):
            shutil.copy(os.path.join(dictConfig["sSourceDir"], sFileName),
                        sScratchDir)
    fnSubstituteMagField(
          os.path.join(sScratchDir, dictConfig["sBodyFile"]),
          dFieldEarthUnits)
    fnShortenStopTime(os.path.join(sScratchDir, "vpl.in"))
    subprocess.run([VPLANET_BINARY, "vpl.in"], cwd=sScratchDir,
                   check=True, capture_output=True)


def fdaReadInitialOutputs(dictConfig, sScratchDir):
    """Return the t=0 row of the planet forward file."""
    sForwardPath = os.path.join(sScratchDir, dictConfig["sForwardFile"])
    return np.loadtxt(sForwardPath)[0]


def fdaSweepRates(dictConfig, daFieldsEarthUnits):
    """Run vplanet at each magnetic moment; return rates matrix."""
    daRates = np.zeros((len(daFieldsEarthUnits), len(COLUMNS)))
    sLabel = dictConfig["sShortName"]
    for iIndex, dField in enumerate(daFieldsEarthUnits):
        with tempfile.TemporaryDirectory(prefix=f"{sLabel}_sweep_") as sDir:
            fnRunVplanetAtField(dictConfig, dField, sDir)
            daInitialRow = fdaReadInitialOutputs(dictConfig, sDir)
        for iCol, sMechanism in enumerate(COLUMNS):
            daRates[iIndex, iCol] = daInitialRow[COLUMNS[sMechanism]]
        if iIndex % 10 == 0:
            print(f"  swept M = {dField:.3e} Earth dipoles "
                  f"({iIndex + 1}/{len(daFieldsEarthUnits)})")
    return daRates


def fnLoadOrSweepCache(dictConfig):
    """Return (daFields, daRates) from cache if present, else run sweep."""
    sCachePath = dictConfig["sCachePath"]
    if os.path.exists(sCachePath):
        print(f"Loading cached sweep from {sCachePath}")
        dictCache = np.load(sCachePath)
        return dictCache["fields"], dictCache["rates"]
    print(f"Sweeping {len(DA_M_OVER_EARTH)} magnetic moments for "
          f"{dictConfig['sShortName']}...")
    daFields = DA_M_OVER_EARTH
    daRates = fdaSweepRates(dictConfig, daFields)
    np.savez(sCachePath, fields=daFields, rates=daRates)
    print(f"Cached results to {sCachePath}")
    return daFields, daRates


# ---------------------- conversions ----------------------

def fdaConvertToAtmPerGyr(daRatesParticlesPerSec):
    """Convert particles/s into Earth atmospheres / Gyr (notebook units)."""
    daKgPerSec = daRatesParticlesPerSec * PROTON_MASS_KG
    return daKgPerSec * GYR_SECONDS / EARTH_ATM_KG


def fdaSumPositiveHRates(daRates):
    """Sum the positive-valued H mechanisms; mask -1 sentinels and
    Driscoll sign flips."""
    daSum = np.zeros(daRates.shape[0])
    for sMechanism in H_MECHANISMS:
        daRaw = daRates[:, list(COLUMNS.keys()).index(sMechanism)]
        daSum = daSum + np.where(daRaw > 0, daRaw, 0)
    return daSum


# ---------------------- magnetic-moment sweep figure ----------------------

def fnPlotPerMechanism(ax, daFields, daRates):
    """One log-log line per mechanism plus the H total."""
    for sMechanism in ALL_MECHANISMS:
        iIndex = list(COLUMNS.keys()).index(sMechanism)
        daSigned = np.where(daRates[:, iIndex] > 0,
                            daRates[:, iIndex], np.nan)
        daYAtmGyr = fdaConvertToAtmPerGyr(daSigned)
        ax.loglog(daFields, daYAtmGyr,
                  color=MECHANISM_COLORS[sMechanism],
                  linestyle=MECHANISM_LINESTYLES[sMechanism],
                  label=f"{sMechanism} H escape")
    daTotalAtmGyr = fdaConvertToAtmPerGyr(fdaSumPositiveHRates(daRates))
    ax.loglog(daFields, daTotalAtmGyr, color="black", linewidth=3.0,
              label="Total H escape")


def fnSaveSweepFigure(daFields, daRates, sOutputPath):
    """Build and write the notebook-style M-vs-escape-rate figure."""
    with plt.rc_context(RC_OVERRIDES):
        fig, ax = plt.subplots(figsize=(10, 7))
        fnPlotPerMechanism(ax, daFields, daRates)
        ax.set_xlabel("Magnetic dipole moment (Earth units)")
        ax.set_ylabel("Escape rate (Earth atm Gyr$^{-1}$)")
        ax.set_xlim(1e-4, 1e2)
        ax.set_ylim(1e-35, 1e5)
        ax.legend(loc="lower left", framealpha=0.95)
        ax.grid(alpha=0.3, which="both")
        fig.tight_layout()
        fig.savefig(sOutputPath, dpi=150)
    print(f"Wrote {sOutputPath}")


# ---------------------- four-panel time evolution ----------------------

def fdaPositiveOrNan(daValues):
    """Mask non-positive values for log plots."""
    return np.where(daValues > 0, daValues, np.nan)


def fnPlotMassLossPanel(ax, daConstRows, daThermRows):
    """Panel (a): bulk mass loss rate by mechanism plus total, for both
    constant-B and thermint-coupled Earth runs."""
    daTimeConstGyr = daConstRows[:, COLS_CONST["Time"]] / 1e9
    daTimeThermGyr = daThermRows[:, COLS_THERMINT["Time"]] / 1e9
    dBulkFactor = MAG_NUM_WELL_MIXED_SPECIES * PROTON_MASS_KG
    for sMechanism in PANEL_MECHANISMS:
        sColor = MECHANISM_COLORS[sMechanism]
        daConstRaw = daConstRows[:, COLS_CONST[sMechanism]]
        daThermRaw = daThermRows[:, COLS_THERMINT[sMechanism]]
        daConstBulk = np.where(daConstRaw == -1, np.nan,
                               np.abs(daConstRaw) * dBulkFactor)
        daThermBulk = np.where(daThermRaw == -1, np.nan,
                               np.abs(daThermRaw) * dBulkFactor)
        ax.plot(daTimeConstGyr, daConstBulk, color=sColor, linestyle="-",
                label=f"{sMechanism} (const B)")
        ax.plot(daTimeThermGyr, daThermBulk, color=sColor, linestyle="--",
                label=f"{sMechanism} (thermint B)")
    ax.plot(daTimeConstGyr, daConstRows[:, COLS_CONST["Total"]],
            color="black", linestyle="-", label="Total (const B)")
    ax.plot(daTimeThermGyr, daThermRows[:, COLS_THERMINT["Total"]],
            color="black", linestyle="--", label="Total (thermint B)")
    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Atmospheric mass loss rate (kg s$^{-1}$)")
    ax.grid(alpha=0.3)
    ax.legend(loc="center right")


def fnPlotMagFieldPanel(ax, daConstRows, daThermRows):
    """Panel (b): magnetic dipole strength vs time."""
    daTimeConstGyr = daConstRows[:, COLS_CONST["Time"]] / 1e9
    daTimeThermGyr = daThermRows[:, COLS_THERMINT["Time"]] / 1e9
    daConstField = daConstRows[:, COLS_CONST["MagField"]]
    daThermField = daThermRows[:, COLS_THERMINT["MagField"]]
    ax.plot(daTimeConstGyr, daConstField, color=vp.dark_blue, linestyle="-",
            label="Constant B")
    ax.plot(daTimeThermGyr, daThermField, color=vp.orange, linestyle="--",
            label="Thermint dynamo")
    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Dipole moment (Earth dipoles)")
    ax.grid(alpha=0.3)
    ax.legend()


def fnPlotCoreTemperaturePanel(ax, daThermRows):
    """Panel (c): core temperature (thermint run only)."""
    daTimeGyr = daThermRows[:, COLS_THERMINT["Time"]] / 1e9
    daTCore = daThermRows[:, COLS_THERMINT["TCore"]]
    ax.plot(daTimeGyr, daTCore, color=vp.red)
    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Core temperature (K)")
    ax.grid(alpha=0.3)


def fnPlotInnerCoreRadiusPanel(ax, daThermRows):
    """Panel (d): inner core radius (thermint run only)."""
    daTimeGyr = daThermRows[:, COLS_THERMINT["Time"]] / 1e9
    daRIC = daThermRows[:, COLS_THERMINT["RIC"]]
    ax.plot(daTimeGyr, daRIC, color=vp.purple)
    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Inner core radius (km)")
    ax.grid(alpha=0.3)


def fnSaveEarthEvolutionFigure(sOutputPath):
    """Build and write the four-panel Earth comparison figure."""
    daConstRows = np.loadtxt(EARTH_CONST_FORWARD)
    daThermRows = np.loadtxt(EARTH_THERMINT_FORWARD)
    with plt.rc_context(RC_OVERRIDES):
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fnPlotMassLossPanel(axes[0, 0], daConstRows, daThermRows)
        fnPlotMagFieldPanel(axes[0, 1], daConstRows, daThermRows)
        fnPlotCoreTemperaturePanel(axes[1, 0], daThermRows)
        fnPlotInnerCoreRadiusPanel(axes[1, 1], daThermRows)
        fig.tight_layout()
        fig.savefig(sOutputPath, dpi=150)
    print(f"Wrote {sOutputPath}")


# ---------------------- main ----------------------

if __name__ == "__main__":
    for dictSweep in SWEEPS:
        daFields, daRates = fnLoadOrSweepCache(dictSweep)
        fnSaveSweepFigure(daFields, daRates,
                          os.path.join(THIS_DIR, dictSweep["sFilename"]))
    fnSaveEarthEvolutionFigure(os.path.join(THIS_DIR, "EarthEvolution.png"))
