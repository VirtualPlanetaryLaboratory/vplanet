"""Stellar evolution validation figure."""
import numpy as np
import subprocess
import vplot as vpl
from tqdm import tqdm
import matplotlib.pyplot as pl
cmap = pl.get_cmap('jet')

star = """#
sName	                  s%02d
saModules	              stellar
dMass                     %.5f
dAge                      1e7
sStellarModel             baraffe
dSatXUVFrac               1.e-3
dSatXUVTime               -0.1
saOutputOrder Luminosity Radius Temperature RotPer LXUVStellar
"""

system = """#
sSystemName               system
iVerbose                  0
bOverwrite                1
saBodyFiles               %s
sUnitMass                 solar
sUnitLength               AU
sUnitTime                 YEARS
sUnitAngle                d
bDoLog                    1
iDigits                   6
dMinValue                 1e-10
bDoForward                1
bVarDt                    1
dEta                      0.1
dStopTime                 5e9
dOutputTime               1e7
"""


def write_in(masses):
    """Write the .in files to disk."""
    nfiles = len(masses)

    # Write the vpl.in file
    with open("vpl.in", "w") as file:
        filenames = " ".join(["s%02d.in" % n for n in range(nfiles)])
        print(system % filenames, file=file)

    # Write each star file
    for n in range(nfiles):
        with open("s%02d.in" % n, "w") as file:
            print(star % (n, masses[n]), file=file)


def run(masses):
    """Run vplanet and collect the output."""
    write_in(masses)
    subprocess.call(['vplanet', 'vpl.in'])
    output = vpl.GetOutput()
    radius = [output.bodies[n].Radius for n in range(len(masses))]
    return radius


# Create the figure
fig, ax = pl.subplots(2, 2, figsize=(8, 6))
fig.subplots_adjust()

masses = np.array([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0])
radius = run(masses)

ax[0].plot(radius)

pl.show()
