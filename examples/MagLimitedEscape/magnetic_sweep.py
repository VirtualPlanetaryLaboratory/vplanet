"""Shared sweep + plot machinery for the MagLimitedEscape example.

A "sweep config" is a dict carrying everything that distinguishes a
planet: source directory, body file, forward-file pattern, dMagField
input line to substitute, plot title, and cache path.

Both notebook_comparison.py (Trappist-1e) and earth_comparison.py
(modern Earth) call fnRunSweep with their config; this module owns the
loop logic, the unit conversion, and the plot styling so neither
caller has to."""

import os
import shutil
import subprocess
import tempfile

import matplotlib.pyplot as plt
import numpy as np

import vplot
from vplot import colors as vp

PROTON_MASS_KG = 1.67e-27
EARTH_ATM_KG = 5.972e18
GYR_SECONDS = 3.156e16

DA_M_OVER_EARTH = np.logspace(-4, 2, 60)

VPLANET_BINARY = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "..", "..", "bin", "vplanet")

# Output column indices for any AtmEsc-only body file produced by this
# example. Both Trappist-1e and Earth use the same saOutputOrder block,
# so the column mapping is shared.
COLUMNS = {
    "Pickup": 3, "CrossField": 4, "PolarCap": 5, "Cusp": 6,
    "Driscoll": 7, "Total": 8,
}

H_MECHANISMS = ["Pickup", "CrossField", "PolarCap", "Cusp"]
ALL_MECHANISMS = H_MECHANISMS + ["Driscoll"]

MECHANISM_COLORS = {
    "Pickup": vp.orange,
    "CrossField": vp.dark_blue,
    "PolarCap": vp.purple,
    "Cusp": vp.red,
    "Driscoll": vp.pale_blue,
}
MECHANISM_LINESTYLES = {
    "Pickup": "--", "CrossField": "-", "PolarCap": "-",
    "Cusp": "--", "Driscoll": "-",
}

RC_OVERRIDES = {
    "axes.labelsize": 17,
    "xtick.labelsize": 14,
    "ytick.labelsize": 14,
    "legend.fontsize": 13,
    "lines.linewidth": 2.0,
}


def fnSubstituteMagField(sBodyPath, dFieldEarthUnits):
    """Replace 'dMagField <something>' with 'dMagField -<value>' in place."""
    with open(sBodyPath, "r") as fHandle:
        sContents = fHandle.read()
    listLines = sContents.splitlines(keepends=True)
    sNewValue = f"{-dFieldEarthUnits:.6e}"
    for iIndex, sLine in enumerate(listLines):
        if sLine.lstrip().startswith("dMagField"):
            listLines[iIndex] = f"dMagField         {sNewValue}\n"
            break
    with open(sBodyPath, "w") as fHandle:
        fHandle.writelines(listLines)


def fnShortenStopTime(sVplPath):
    """Shrink dStopTime in vpl.in to 1e7 years; the sweep only reads t=0."""
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
    """Copy the source dir into the scratch dir, swap in dMagField, run."""
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
    """Run vplanet at each magnetic moment and collect per-mechanism rates."""
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


def fdaConvertToAtmPerGyr(daRatesParticlesPerSec):
    """Convert particles/s into Earth atmospheres per Gyr.
    NaN propagation lets log plots mask sign flips."""
    daKgPerSec = daRatesParticlesPerSec * PROTON_MASS_KG
    return daKgPerSec * GYR_SECONDS / EARTH_ATM_KG


def fdaSumPositive(daRates, listMechanisms):
    """Sum only the positive-valued mechanism rates (Driscoll's negative
    excursions are excluded from the H total, matching the notebook)."""
    daSum = np.zeros(daRates.shape[0])
    for sMechanism in listMechanisms:
        daRaw = daRates[:, list(COLUMNS.keys()).index(sMechanism)]
        daSum = daSum + np.where(daRaw > 0, daRaw, 0)
    return daSum


def fnPlotPerMechanism(ax, daFields, daRates):
    """Draw one line per mechanism plus the total H curve."""
    for sMechanism in ALL_MECHANISMS:
        iIndex = list(COLUMNS.keys()).index(sMechanism)
        daSigned = np.where(daRates[:, iIndex] > 0, daRates[:, iIndex], np.nan)
        daYAtmGyr = fdaConvertToAtmPerGyr(daSigned)
        ax.loglog(daFields, daYAtmGyr,
                  color=MECHANISM_COLORS[sMechanism],
                  linestyle=MECHANISM_LINESTYLES[sMechanism],
                  label=f"{sMechanism} H escape")
    daTotalAtmGyr = fdaConvertToAtmPerGyr(fdaSumPositive(daRates, H_MECHANISMS))
    ax.loglog(daFields, daTotalAtmGyr, color="black", linewidth=3.0,
              label="Total H escape")


def fnSavePlot(daFields, daRates, sOutputPath):
    """Build and write the magnetic-moment vs escape-rate figure."""
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


def fnRunSweep(dictConfig):
    """Top-level driver: load cache or run the sweep, then plot."""
    if os.path.exists(dictConfig["sCachePath"]):
        print(f"Loading cached sweep from {dictConfig['sCachePath']}")
        dictCache = np.load(dictConfig["sCachePath"])
        daFields = dictCache["fields"]
        daRates = dictCache["rates"]
    else:
        print(f"Sweeping {len(DA_M_OVER_EARTH)} magnetic moments for "
              f"{dictConfig['sShortName']}...")
        daFields = DA_M_OVER_EARTH
        daRates = fdaSweepRates(dictConfig, daFields)
        np.savez(dictConfig["sCachePath"], fields=daFields, rates=daRates)
        print(f"Cached results to {dictConfig['sCachePath']}")
    fnSavePlot(daFields, daRates, dictConfig["sPlotPath"])
