#!/usr/bin/env python3
"""Modern-Earth magnetic-moment sweep using the Driscoll13 model.
Companion to earth_comparison.py (Gunell18 sweep). Useful for verifying
that the two models agree at the no-field limit and diverge as the
dipole grows past the magnetopause cutoff."""

import os
import sys

import magnetic_sweep

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG = {
    "sShortName": "earth_driscoll",
    "sSourceDir": os.path.join(THIS_DIR, "earth_const_driscoll"),
    "sBodyFile": "earth.in",
    "sForwardFile": "earth_const_driscoll.earth.forward",
    "sCachePath": os.path.join(THIS_DIR, "earth_driscoll_sweep_cache.npz"),
    "sPlotPath": os.path.join(THIS_DIR, "Earth_DriscollReproduction.png"),
}


if __name__ == "__main__":
    if len(sys.argv) > 1:
        CONFIG["sPlotPath"] = sys.argv[1]
    magnetic_sweep.fnRunSweep(CONFIG)
