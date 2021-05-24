#!/usr/bin/env python
import vplanet
import vplot as vpl
import bigplanet as bp
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pathlib
import sys
import subprocess

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Run vspace
if not (path / "ParameterSweep").exists():
    subprocess.check_output(["vspace", "vspace.in"], cwd=path)

# Run multi-planet
if not (path / ".ParameterSweep").exists():
    subprocess.check_output(["multi-planet", "vspace.in"], cwd=path)

# Run bigplanet
if not (path / ".ParameterSweep_BPL").exists():
    subprocess.check_output(["bigplanet", "vspace.in"], cwd=path)

data = bp.BPLFile(path / "ParameterSweep.bpl")

mpl.rcParams["figure.figsize"] = (10, 8)
fig = plt.figure()

RIC = bp.ExtractColumn(data, "earth_RIC_final")
RIC_units = bp.ExtractUnits(data, "earth_RIC_final")

TCore_uniq = bp.ExtractUniqueValues(data, "earth_TCore_initial")
TCore_units = bp.ExtractUnits(data, "earth_TCore_initial")

K40_uniq = bp.ExtractUniqueValues(data, "earth_40KPowerCore_final")
K40_units = bp.ExtractUnits(data, "earth_40KPowerCore_final")

RIC_Matrix = bp.CreateMatrix(TCore_uniq, K40_uniq, RIC)

contours = [0, 500, 1000, 1500, 2000, 2500]
xlabel = "Initial Core Temperature (" + TCore_units + ")"
ylabel = "Current Potassium-40 Power (" + K40_units + ")"
title = "Final Inner Core Radius (" + RIC_units + ")"

plt.xlabel(xlabel, fontsize=20)
plt.ylabel(ylabel, fontsize=20)
plt.title(title, fontsize=20)

plt.tick_params(axis="both", labelsize=20)

cont = plt.contour(
    TCore_uniq, K40_uniq, RIC_Matrix, colors=vpl.colors.pale_blue, levels=contours
)
plt.clabel(cont, fmt="%.0f", fontsize=15)

cont = plt.contour(TCore_uniq, K40_uniq, RIC_Matrix, levels=[1221])
plt.clabel(cont, fmt="%.0f", fontsize=15)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"ParameterSweep.{ext}", dpi=300)
