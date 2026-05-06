#!/usr/bin/env python3
"""Sweep dMagField for modern Earth and produce a notebook-style plot of
escape rate vs magnetic dipole moment. The setup uses the same Earth
parameters as earth_const/ (modern quiet-Sun stellar wind at 1 AU,
1000 K exobase). Useful as a companion to T1e_Reproduction so that the
mass-loss-rate behaviour can be compared between an M-dwarf planet and
a Sun-like-star planet."""

import os
import sys

import magnetic_sweep

THIS_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG = {
    "sShortName": "earth",
    "sSourceDir": os.path.join(THIS_DIR, "earth_const"),
    "sBodyFile": "earth.in",
    "sForwardFile": "earth_const.earth.forward",
    "sCachePath": os.path.join(THIS_DIR, "earth_sweep_cache.npz"),
    "sPlotPath": os.path.join(THIS_DIR, "Earth_Reproduction.png"),
}


if __name__ == "__main__":
    if len(sys.argv) > 1:
        CONFIG["sPlotPath"] = sys.argv[1]
    magnetic_sweep.fnRunSweep(CONFIG)
