import glob
import pathlib
import subprocess
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot
from scipy.interpolate import interp2d
from tqdm import tqdm

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Number of dimensions
necc = 30
nobl = 30
ecc = np.zeros(necc * nobl)
obl = np.zeros(necc * nobl)
heat = np.zeros(necc * nobl)

# Run vspace
if not ((path / "data").exists()):
    subprocess.check_output(["vspace", "vspace.in"], cwd=str(path))
dirs = glob.glob(str(path / "data" / "io*"))

# Run vplanet
for i, dir in tqdm(enumerate(dirs), total=len(dirs)):

    # Run vplanet
    output = vplanet.run(path / dir / "vpl.in", quiet=True, units=False)
    ecc[i] = output.log.initial.io.Eccentricity
    obl[i] = output.log.initial.io.Obliquity * 180 / np.pi
    heat[i] = output.log.initial.io.SurfEnFluxEqtide

# Transform onto a 2d grid
f = interp2d(ecc, obl, heat)
ecc = np.linspace(ecc.min(), ecc.max(), necc)
obl = np.linspace(obl.min(), obl.max(), nobl)
heat = f(ecc, obl)

fig = plt.figure(figsize=(10, 8))

# Arrays ecc,obl,heat now contain the data to make the figure
plt.xlabel("Eccentricity", fontsize=20)
plt.ylabel("Obliquity ($^\circ$)", fontsize=20)
plt.tick_params(axis="both", labelsize=20)

plt.yscale("log")
plt.xlim(1e-3, 0.01)
plt.ylim(1e-3, 1)


ContSet = plt.contour(
    ecc,
    obl,
    heat,
    5,
    colors="black",
    linestyles="solid",
    levels=[1, 5, 10],
    linewidths=3,
    origin="lower",
)
plt.clabel(ContSet, fmt="%d", inline=True, fontsize=18)

# Io's heat flux is 1.5-3 W/m^2. After some fussing, this choice of contour matches that range.
# plt.contour(ecc,obl,heat,5,colors=vplot.colors.orange,linestyles='solid',
#                      levels=[2.1],linewidths=45,origin='lower')

plt.contourf(
    ecc,
    obl,
    heat,
    5,
    colors=vplot.colors.orange,
    linestyles="solid",
    levels=[1.5, 3],
    origin="lower",
)

x = [0.0041, 0.0041]
y = [1e-3, 1]
plt.plot(x, y, linestyle="dashed", color="black", linewidth=3)

x = [1e-3, 0.3]
y = [0.0023, 0.0023]
plt.plot(x, y, linestyle="dotted", color="black", linewidth=3)

# Save the figure
ext = get_args().ext
fig.savefig(path / f"IoHeat.{ext}", bbox_inches="tight", dpi=600)
