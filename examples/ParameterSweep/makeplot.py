#!/usr/bin/env python
import pathlib
import subprocess
import sys

import bigplanet as bp
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot as vpl

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Run vspace
if not (path / "ParameterSweep").exists():
    print("Running VSPACE...")
    subprocess.check_output(["vspace", "vspace.in"], cwd=path)
else:
    print("VPSACE already run")

# Run multi-planet
if not (path / ".ParameterSweep").exists():
    print("Running MultiPlanet...")
    subprocess.check_output(["multiplanet", "vspace.in"], cwd=path)
else:
    print("Multiplanet already run")

# Run bigplanet
if not (path / "ParameterSweep.bpf").exists():
    print("Building BigPlanet File")
    subprocess.check_output(["bigplanet", "bpl.in"], cwd=path)
else:
    print("BigPlanet File already built")

print("Creating figure...")
data = bp.BPLFile(path / "ParameterSweep.bpf")

mpl.rcParams["figure.figsize"] = (6.5, 6.5)
fig = plt.figure()

RIC = bp.ExtractColumn(data, "earth:RIC:final")
RIC_units = bp.ExtractUnits(data, "earth:RIC:final")

TCore_uniq = bp.ExtractUniqueValues(data, "earth:TCore:initial")
TCore_units = bp.ExtractUnits(data, "earth:TCore:initial")

K40_uniq = bp.ExtractUniqueValues(data, "earth:40KPowerCore:final")
K40_units = bp.ExtractUnits(data, "earth:40KPowerCore:final")

RIC_Matrix = np.reshape(RIC, (len(TCore_uniq), len(K40_uniq)))

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
