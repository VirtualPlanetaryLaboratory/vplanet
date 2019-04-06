"""Stellar evolution validation figure."""
import numpy as np
import subprocess
import vplot as vpl
import matplotlib.pyplot as pl
cmap = pl.get_cmap('inferno')
import sys
import os

star = """#
sName	                  s%02d
saModules	              stellar
dMass                     %.5f
dAge                      5e6
sStellarModel             baraffe
dSatXUVFrac               1.e-3
dSatXUVTime               -0.1
saOutputOrder Time -Luminosity -Radius Temperature -RotPer -LXUVTot RadGyra
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
dEta                      0.01
dStopTime                 5e9
dOutputTime               1.0e6
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
    time = output.bodies[0].Time
    radius = [output.bodies[n].Radius for n in range(len(masses))]
    temp = [output.bodies[n].Temperature for n in range(len(masses))]
    lum = [output.bodies[n].Luminosity for n in range(len(masses))]
    lxuv = [output.bodies[n].LXUVTot for n in range(len(masses))]
    prot = [output.bodies[n].RotPer for n in range(len(masses))]
    rg = [output.bodies[n].RadGyra for n in range(len(masses))]
    return time, radius, lum, lxuv, temp, prot, rg

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Run, load in 1.3 M sun data (load separately because it leaves main sequence)
os.chdir("M1.3") # cd into dead star dir
subprocess.call(['vplanet', 'vpl.in'])
os.chdir("..") # cd back!

dead = np.genfromtxt("M1.3/system.a.forward", delimiter=" ")

# Create the figure
fig, ax = pl.subplots(nrows=3, ncols=2, figsize=(10, 6))
fig.subplots_adjust(right=0.825, wspace=0.30)

masses = np.array([0.08, 0.25, 0.5, 0.75, 1.0])
time, radius, lum, lxuv, temp, prot, rg = run(masses)

# Plot stars that survived
for n, m in enumerate(masses):
    # Top row: radius, legend
    ax[0, 0].plot(time, radius[n], label="%.2f" % m, color=cmap(0.7*m))
    # Dummy data for legend
    ax[0, 1].plot([101], [100], label="%.2f" % m, color=cmap(0.7*m))

    # Middle row: rg, Teff
    ax[1, 0].plot(time, rg[n], label="%.2f" % m, color=cmap(0.7*m))
    ax[1, 1].plot(time, temp[n], label="%.2f" % m, color=cmap(0.7*m))

    # Bottom row: L, Lxuv
    ax[2, 0].plot(time, lum[n], label="%.2f" % m, color=cmap(0.7*m))
    ax[2, 1].plot(time, lxuv[n], label="%.2f" % m, color=cmap(0.7*m))

# Add in dead star
# Top row: radius, legend
m = 1.3
time = dead[:,0]
ax[0, 0].plot(time, dead[:,4], label="%.2f" % m, color=cmap(0.7*m))
# Dummy data for legend
ax[0, 1].plot([101], [100], label="%.2f" % m, color=cmap(0.7*m))

# Middle row: rg, Teff
ax[1, 0].plot(time, dead[:,8], label="%.2f" % m, color=cmap(0.7*m))
ax[1, 1].plot(time, dead[:,5], label="%.2f" % m, color=cmap(0.7*m))

# Bottom row: L, Lxuv
ax[2, 0].plot(time, dead[:,3], label="%.2f" % m, color=cmap(0.7*m))
ax[2, 1].plot(time, dead[:,7], label="%.2f" % m, color=cmap(0.7*m))

for axis in ax.flatten():
    axis.set_xscale('log')
ax[0, 1].set_xlim(0, 1)
ax[0, 1].set_xlim(0, 1)
ax[0, 1].set_axis_off()
ax[2, 0].set_yscale('log')
ax[2, 1].set_yscale('log')
ax[2, 0].set_xlabel('Time (yr)', fontsize=14)
ax[2, 1].set_xlabel('Time (yr)', fontsize=14)
ax[0, 0].set_ylabel(r'Radius ($\mathrm{R}_\odot$)', fontsize=14)
ax[1, 0].set_ylabel(r'Radius of Gyration', fontsize=14)
ax[2, 0].set_ylabel(r'Luminosity ($\mathrm{L}_\odot$)', fontsize=14)
ax[1, 1].set_ylabel(r'Temperature ($\mathrm{K}$)', fontsize=14)
ax[2, 1].set_ylabel(r'L$_{\mathrm{XUV}}$ ($\mathrm{L}_\odot$)', fontsize=14)
leg = ax[0, 1].legend(loc=(-0.2, 0.15), title=r'Mass ($\mathrm{M}_\odot$)',
                      ncol=3)
leg.get_title().set_fontweight('bold')

if (sys.argv[1] == 'pdf'):
    fig.savefig('MainSequence.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('MainSequence.png', bbox_inches="tight", dpi=600)
