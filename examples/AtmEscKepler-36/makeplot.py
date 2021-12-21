"""Reproduce Figure 3 in Lopez and Fortney (2013)."""
import vplanet
import vplot
import numpy as np
from tqdm import tqdm
import matplotlib.pyplot as plt
import pathlib
import sys

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Star input file template
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

# Planet input file template
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
bUseEnergyLimited         1
dEnvelopeMass -%.5f
dMass -%.5f
saOutputOrder Time -EnvelopeMass Radius
"""

# System input file template
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
    with open(path / "vpl.in", "w") as file:
        filenames = " ".join(["p%02d.in" % n for n in range(nfiles)])
        print(system % filenames, file=file)

    # Write the star file
    with open(path / "star.in", "w") as file:
        print(star, file=file)

    # Write each planet file
    for n in range(nfiles):
        with open(path / ("p%02d.in" % n), "w") as file:
            print(planet % (n, envmass[n], mass[n]), file=file)


def run(envmass, mass):
    """Run vplanet and collect the output."""
    write_in(envmass, mass)
    output = vplanet.run(path / "vpl.in", clobber=True, quiet=True, units=False)
    envmassfinal = np.zeros(len(output.bodies) - 1)
    for i, body in enumerate(output.bodies[1:]):
        envmassfinal[i] = body.EnvelopeMass[-1]
    return (envmass - envmassfinal) / envmass


# The variables we're iterating over
envfrac = np.array([0.01, 0.03, 0.05, 0.1, 0.25, 0.33, 0.45])
colors = plt.get_cmap("plasma")(np.array([0, 0.25, 0.35, 0.5, 0.6, 0.75, 0.8]))
coremass = np.logspace(np.log10(1), np.log10(50), 20)

# Create the figure
fig, ax = plt.subplots(3, figsize=(4.5, 9))
fig.subplots_adjust(top=0.85, left=0.2, hspace=0.4)

# Add the colorbar
cax = fig.add_axes([0.4, 0.9, 0.5, 0.025])
cax.annotate(
    r"$\mathrm{f}_{\mathrm{envelope}}$",
    xy=(-0.1, 0),
    xytext=(0, 0),
    xycoords="axes fraction",
    textcoords="offset points",
    ha="right",
    va="center",
    clip_on=False,
    fontsize=20,
)
cax.imshow(
    [np.linspace(0, 1, 100)],
    cmap=plt.get_cmap("plasma"),
    vmin=0,
    vmax=1,
    aspect="auto",
    extent=(0, 1, 0, 1),
)
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
    ax[n].set_xscale("log")
    ax[n].set_xticks((0.1, 1, 10))
    ax[n].set_xticklabels((0.1, 1, 10))

ax[0].set_xlim(1, 50)
ax[1].set_xlim(0.1, 50)
ax[2].set_xlim(1, 50)
ax[0].set_xlabel(r"Core Mass ($\mathrm{M}_\oplus$)")
ax[1].set_xlabel(r"Initial Envelope Mass ($\mathrm{M}_\oplus$)")
ax[2].set_xlabel(r"Initial Total Mass ($\mathrm{M}_\oplus$)")


# Save
ext = get_args().ext
fig.savefig(path / f"AtmEscKepler-36.{ext}", bbox_inches="tight", dpi=600)
