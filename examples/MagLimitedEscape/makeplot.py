#!/usr/bin/env python3
"""Plot Gunell+2018 magnetic-limited mass loss rates for the three
MagLimitedEscape simulations and overlay the present-Earth Driscoll
reference value used in the source notebook."""

import os
import sys

import matplotlib.pyplot as plt
import numpy as np

try:
    import vplot
    plt.style.use("vplot")
except ImportError:
    pass

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

SIMULATIONS = [
    ("trappist1e", "trappist1e.e.forward", "Trappist-1e"),
    ("earth_const", "earth_const.earth.forward", "Earth, B = 1 Earth (const)"),
    ("earth_thermint", "earth_thermint.earth.forward",
     "Earth, B from thermint"),
]


def fdaLoadForward(sPath, iColumns):
    """Read a vplanet forward file and return a numpy array of selected
    column indices."""
    if not os.path.exists(sPath):
        raise FileNotFoundError(sPath)
    daRows = np.loadtxt(sPath)
    return daRows[:, iColumns]


def fnPlotComparison(sOutputPath):
    """Plot total mass loss rate vs time for all three simulations."""
    fig, ax = plt.subplots(figsize=(8, 5))

    iTimeColumn = 0
    for sDirName, sForwardName, sLabel in SIMULATIONS:
        sFullPath = os.path.join(THIS_DIR, sDirName, sForwardName)
        try:
            daData = np.loadtxt(sFullPath)
        except (FileNotFoundError, OSError):
            print(f"Skipping {sDirName}: forward file not found", file=sys.stderr)
            continue
        daTime = daData[:, iTimeColumn]
        # MagTotalLossRate is the 9th output for trappist1e/earth_const
        # (column index 8) and the 10th for earth_thermint (because MagMom
        # is also present); use the column whose values are O(1) kg/s.
        iLossCol = 8 if sDirName != "earth_thermint" else 9
        daLoss = daData[:, iLossCol]
        ax.plot(daTime / 1e9, daLoss, label=sLabel, linewidth=2)

    ax.set_xlabel("Time (Gyr)")
    ax.set_ylabel("Total bulk atmospheric mass loss (kg/s)")
    ax.set_title("Gunell+2018 magnetic-limited escape")
    ax.legend()
    ax.grid(alpha=0.3)
    fig.tight_layout()
    fig.savefig(sOutputPath, dpi=150)
    print(f"Wrote {sOutputPath}")


if __name__ == "__main__":
    sOutput = sys.argv[1] if len(sys.argv) > 1 else "MagLimitedEscape.png"
    fnPlotComparison(sOutput)
