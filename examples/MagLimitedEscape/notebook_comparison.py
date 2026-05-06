#!/usr/bin/env python3
"""Sweep dMagField across the notebook's logarithmic range for Trappist-1e
and reproduce notebook page 27. Compare visually against the published
plot to validate vplanet's Gunell escape implementation."""

import os
import sys

import magnetic_sweep

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG = {
    "sShortName": "t1e",
    "sSourceDir": os.path.join(THIS_DIR, "trappist1e"),
    "sBodyFile": "e.in",
    "sForwardFile": "trappist1e.e.forward",
    "sCachePath": os.path.join(THIS_DIR, "t1e_sweep_cache.npz"),
    "sPlotPath": os.path.join(THIS_DIR, "T1e_NotebookComparison.png"),
}


if __name__ == "__main__":
    if len(sys.argv) > 1:
        CONFIG["sPlotPath"] = sys.argv[1]
    magnetic_sweep.fnRunSweep(CONFIG)
