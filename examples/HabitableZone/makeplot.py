import glob
import os
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

MSUN = 1.988416e30
LSUN = 3.846e26

# If necessary, build directories
if not ((path / "data").exists()):
    sys.stdout.write("Buliding directories.")
    sys.stdout.flush()
    subprocess.run(["vspace", "vspace.in"], cwd=str(path))
else:
    print("VPSACE already run")

# Run multi-planet
if not (path / ".data").exists():
    sys.stdout.write("\nRunning trials.")
    sys.stdout.flush()
    subprocess.run(["multiplanet", "vspace.in"], cwd=str(path))
    sys.stdout.write("\n")
    sys.stdout.flush()
else:
    print("Multiplanet already run")

sys.stdout.write("Making plot.")
sys.stdout.flush()
dirs = []
for file in os.listdir(path / "data/"):
    d = os.path.join(path / "data/", file)
    if os.path.isdir(d):
        dirs.append(d)

sorted_dirs = sorted(dirs)

nmass = len(sorted_dirs)
mass = np.zeros(nmass)
recv = np.zeros(nmass)
runaway = np.zeros(nmass)
maxg = np.zeros(nmass)
earlym = np.zeros(nmass)
lum = np.zeros(nmass)

# dirs_list = enumerate(dirs.sort())
# for dir in dirs.sort():
#    print(dir)
# exit()

# Run vplanet
i = 0
for dir in sorted_dirs:
    # Run vplanet
    output = vplanet.get_output(path / dir, units=False)
    mass[i] = output.log.initial.star.Mass / MSUN
    recv[i] = output.log.initial.star.HZLimRecVenus
    runaway[i] = output.log.initial.star.HZLimRunaway
    maxg[i] = output.log.initial.star.HZLimMaxGreenhouse
    earlym[i] = output.log.initial.star.HZLimEarlyMars
    lum[i] = output.log.initial.star.Luminosity / LSUN
    # print(dir,mass[i],lum[i],recv[i],runaway[i],maxg[i],earlym[i])
    i = i + 1

fig = plt.figure(figsize=(6.5, 5))

# Arrays ecc,obl,heat now contain the data to make the figure
plt.xlabel("Semi-major Axis (au)", fontsize=20)
plt.ylabel("Stellar Mass (M$_\odot$)", fontsize=20)
plt.tick_params(axis="both", labelsize=20)

plt.xlim(0.01, 2.5)
plt.ylim(0.08, 1.1)

plt.plot(recv, mass, color=vplot.colors.orange)
plt.plot(runaway, mass, color=vplot.colors.dark_blue)
plt.plot(maxg, mass, color=vplot.colors.dark_blue)
plt.plot(earlym, mass, color=vplot.colors.pale_blue)

fbk = {"lw": 0.0, "edgecolor": None}
plt.fill_betweenx(mass, runaway, maxg, facecolor=vplot.colors.dark_blue, **fbk)
plt.fill_betweenx(mass, recv, runaway, facecolor=vplot.colors.orange, **fbk)
plt.fill_betweenx(mass, maxg, earlym, facecolor=vplot.colors.pale_blue, **fbk)

plt.fill([1.5, 1.5, 1.7, 1.7], [0.6, 0.65, 0.65, 0.6], vplot.colors.orange)
plt.annotate("Too Hot?", [1.73, 0.601], color=vplot.colors.orange, fontsize=20)

plt.fill([1.5, 1.5, 1.7, 1.7], [0.5, 0.55, 0.55, 0.5], vplot.colors.dark_blue)
plt.annotate("Hab. Zone", [1.73, 0.501], color=vplot.colors.dark_blue, fontsize=20)

plt.fill([1.5, 1.5, 1.7, 1.7], [0.4, 0.45, 0.45, 0.4], vplot.colors.pale_blue)
plt.annotate("Too Cold?", [1.73, 0.401], color=vplot.colors.pale_blue, fontsize=20)

ext = get_args().ext
fig.savefig(path / f"HabitableZone.{ext}", bbox_inches="tight", dpi=600)
print("")
