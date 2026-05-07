#!/usr/bin/env python3
"""Trappist-1e magnetic-moment sweep using the Driscoll13 model. Companion
to notebook_comparison.py (which sweeps under Gunell18). Runs vplanet at
each magnetic moment over the same logarithmic range so the two plots
can be inspected side by side."""

import os
import sys

import magnetic_sweep

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG = {
    "sShortName": "t1e_driscoll",
    "sSourceDir": os.path.join(THIS_DIR, "trappist1e_driscoll"),
    "sBodyFile": "e.in",
    "sForwardFile": "trappist1e_driscoll.e.forward",
    "sCachePath": os.path.join(THIS_DIR, "t1e_driscoll_sweep_cache.npz"),
    "sPlotPath": os.path.join(THIS_DIR, "T1e_DriscollComparison.png"),
}


if __name__ == "__main__":
    if len(sys.argv) > 1:
        CONFIG["sPlotPath"] = sys.argv[1]
    magnetic_sweep.fnRunSweep(CONFIG)
