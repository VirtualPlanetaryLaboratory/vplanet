"""Reproduce Figure 3 in Lopez and Fortney (2013)."""
import numpy as np
import subprocess
import vplot as vpl
from tqdm import tqdm
import matplotlib.pyplot as pl
import sys
cmap = pl.get_cmap('plasma')

star = """#
sName	                  star
saModules	              stellar
dMass                     1.
dRadius                   1.
dLuminosity               -1.
dAge                      1e7
sStellarModel             none
dSatXUVFrac               1.e-3
dSatXUVTime               -0.1
"""

planet = """#
sName p%02d
saModules                 atmesc
dXFrac                    1.0
dSurfWaterMass            0
bHaltEnvelopeGone         0
sPlanetRadiusModel        lopez
dAtmXAbsEffH              0.1
dSemi                     -0.1
dEcc                      0.
dEnvelopeMass -%.5f
dMass -%.5f
saOutputOrder Time -EnvelopeMass Radius
"""

system = """#
sSystemName               kepler36
iVerbose                  0
bOverwrite                1
saBodyFiles               star.in %s
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
dOutputTime               1e9
"""


def write_in(envmass, mass):
    """Write the .in files to disk."""
    nfiles = len(envmass)

    # Write the vpl.in file
    with open("vpl.in", "w") as file:
        filenames = " ".join(["p%02d.in" % n for n in range(nfiles)])
        print(system % filenames, file=file)

    # Write the star file
    with open("star.in", "w") as file:
        print(star, file=file)

    # Write each planet file
    for n in range(nfiles):
        with open("p%02d.in" % n, "w") as file:
            print(planet % (n, envmass[n], mass[n]), file=file)


def run(envmass, mass):
    """Run vplanet and collect the output."""
    write_in(envmass, mass)
    subprocess.call(['vplanet', 'vpl.in'])
    #exit()
    output = vpl.GetOutput()
    envmassfinal = np.zeros(len(output.bodies) - 1)
    for i, body in enumerate(output.bodies[1:]):
        envmassfinal[i] = body.EnvelopeMass[-1]
    return (envmass - envmassfinal) / envmass

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)


# The variables we're iterating over
envfrac = np.array([0.01, 0.03, 0.05, 0.1, 0.25, 0.33, 0.45])
colors = cmap(np.array([0, 0.25, 0.35, 0.5, 0.6, 0.75, 0.8]))
coremass = np.logspace(np.log10(1), np.log10(50), 20)

# Create the figure
fig, ax = pl.subplots(3, figsize=(4.5, 9))
fig.subplots_adjust(top=0.85, left=0.2, hspace=0.4)

# Add the colorbar
cax = fig.add_axes([0.4, 0.9, 0.5, 0.025])
cax.annotate(r"$\mathrm{f}_{\mathrm{envelope}}$", xy=(-0.1, 0),
             xytext=(0, 0), xycoords="axes fraction",
             textcoords="offset points",
             ha="right", va="center", clip_on=False, fontsize=20)
cax.imshow([np.linspace(0, 1, 100)], cmap=cmap, vmin=0, vmax=1, aspect='auto',
           extent=(0, 1, 0, 1))
cax.set_xticks((0, 0.25, 0.5, 0.75, 1.0))
cax.set_xticklabels(("1%", "3%", "10%", "33%", "100%"))
cax.set_yticks(())

# Loop over envelope fraction
for i, f in tqdm(enumerate(envfrac), total=len(envfrac)):
    # Total and envelope mass
    mass = coremass / (1 - f)
    envmass = mass * f

    # Compute percent stripped
    percentstripped = 100 * run(envmass, mass)

    # Plot
    ax[0].plot(coremass, percentstripped, color=colors[i], zorder=-i)
    ax[1].plot(envmass, percentstripped, color=colors[i], zorder=-i)
    ax[2].plot(mass, percentstripped, color=colors[i], zorder=-i)

for n in range(3):
    ax[n].set_ylabel(r"% Envelope stripped")
    ax[n].set_xscale('log')
    ax[n].set_xticks((0.1, 1, 10))
    ax[n].set_xticklabels((0.1, 1, 10))

ax[0].set_xlim(1, 50)
ax[1].set_xlim(0.1, 50)
ax[2].set_xlim(1, 50)
ax[0].set_xlabel(r"Core Mass ($\mathrm{M}_\oplus$)")
ax[1].set_xlabel(r"Initial Envelope Mass ($\mathrm{M}_\oplus$)")
ax[2].set_xlabel(r"Initial Total Mass ($\mathrm{M}_\oplus$)")

if (sys.argv[1] == 'pdf'):
    fig.savefig('AtmEscKepler-36.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    fig.savefig('AtmEscKepler-36.png', bbox_inches="tight", dpi=600)
