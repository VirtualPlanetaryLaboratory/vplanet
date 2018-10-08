"""Reproduce Figure 7 in Luger and Barnes (2015)."""
import numpy as np
import subprocess
import vplot as vpl
from tqdm import tqdm
import matplotlib.pyplot as pl
import matplotlib.colors as colors
import sys
cmap = pl.get_cmap('jet')


star = """#
sName	                  star
saModules	              stellar
dMass                     %.5f
dAge                      1e7
sStellarModel             baraffe
dSatXUVFrac               1.e-3
dSatXUVTime               -1
saOutputOrder HZLimRecVenus HZLimRunaway HZLimMaxGreenhouse HZLimEarlyMars
"""

planet = """#
sName p%02d
saModules                 atmesc
dXFrac                    1.0
dSurfWaterMass            -10.0
dSemi                     %.5f
dEcc                      0.
dEnvelopeMass             0.0
dMass                     -1.0
dRadius                   -1.0
sWaterLossModel           lbexact
sAtmXAbsEffH2OModel       bolmont16
bInstantO2Sink            0
saOutputOrder Time -SurfWaterMass -OxygenMass
"""

system = """#
sSystemName               system
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
dEta                      0.1
dStopTime                 5e9
dOutputTime               1e9
"""


def HZLims(M):
    """Get the four HZ lims in AU for a star of mass `M`."""
    # Write the vpl.in file
    with open("vpl.in", "w") as file:
        print(system % "", file=file)

    # Write the star file
    with open("star.in", "w") as file:
        print(star % M, file=file)

    # Run
    subprocess.call(['vplanet', 'vpl.in'])
    output = vpl.GetOutput()
    return output.bodies[0].HZLimRecVenus[-1], \
        output.bodies[0].HZLimRunaway[-1], \
        output.bodies[0].HZLimMaxGreenhouse[-1], \
        output.bodies[0].HZLimEarlyMars[-1],


def write_in(mass, semi):
    """Write the .in files to disk."""
    nfiles = len(semi)

    # Write the vpl.in file
    with open("vpl.in", "w") as file:
        filenames = " ".join(["p%02d.in" % n for n in range(nfiles)])
        print(system % filenames, file=file)

    # Write the star file
    with open("star.in", "w") as file:
        print(star % mass, file=file)

    # Write each planet file
    for n in range(nfiles):
        with open("p%02d.in" % n, "w") as file:
            print(planet % (n, semi[n]), file=file)


def run(mass, semi):
    """Run vplanet and collect the output."""
    write_in(mass, semi)
    subprocess.call(['vplanet', 'vpl.in'])
    output = vpl.GetOutput()
    water = np.zeros(len(output.bodies) - 1)
    o2 = np.zeros(len(output.bodies) - 1)
    for i, body in enumerate(output.bodies[1:]):
        water[i] = 10 - body.SurfWaterMass[-1]
        o2[i] = body.OxygenMass[-1]
    return water, o2


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
mass = np.linspace(0.08, 0.6, 60)
hzpos = np.linspace(0, 1, 60)

# The variables we're plotting
water = np.zeros((len(mass), len(hzpos)))
o2 = np.zeros((len(mass), len(hzpos)))
rv = np.zeros(len(mass))
rg = np.zeros(len(mass))
mg = np.zeros(len(mass))
em = np.zeros(len(mass))

# Create the figure
fig, ax = pl.subplots(1, 2, figsize=(12, 4))
fig.subplots_adjust(bottom=0.15)

# Loop over stellar mass
for i, m in tqdm(enumerate(mass), total=len(mass)):
    # Adjust our semi-major axis array
    rv[i], rg[i], mg[i], em[i] = HZLims(m)
    semi = rv[i] + hzpos * (em[i] - rv[i])

    # Compute
    water[i, 0:20], o2[i, 0:20] = run(m, semi[0:20])
    water[i, 20:40], o2[i, 20:40] = run(m, semi[20:40])
    water[i, 40:60], o2[i, 40:60] = run(m, semi[40:60])

# Enforce some minima for plotting
water[water < 0.1] = 0.1
o2[o2 < 1] = 1

# Plot water
im1 = ax[0].imshow(water, cmap=cmap, aspect='auto',
                   extent=(0, 1, 0.08, 0.6),
                   norm=colors.LogNorm(vmin=0.1, vmax=10))
cb1 = fig.colorbar(im1, ax=ax[0], ticks=[0.1, 1, 10])
cb1.ax.set_yticklabels(['0.1', '1', '10'])

# Plot oxygen
im2 = ax[1].imshow(o2, cmap=cmap, aspect='auto',
                   extent=(0, 1, 0.08, 0.6),
                   norm=colors.LogNorm(vmin=1, vmax=5000))
cb2 = fig.colorbar(im2, ax=ax[1], ticks=[1, 10, 100, 1000, 5000])
cb2.ax.set_yticklabels(['1', '10', '100', '1000', '5000'])

for n in [0, 1]:
    ax[n].plot((rg - rv) / (em - rv), mass, 'w--')
    ax[n].plot((mg - rv) / (em - rv), mass, 'w--')
    ax[n].set_xlabel('Position in Habitable Zone', fontsize=18)
    ax[n].set_xticks((0, 0.25, 0.5, 0.75, 1.0))
    ax[n].set_xticklabels(("RV", "25%", "50%", "75%", "EM"))
ax[0].set_ylabel(r'Stellar mass ($\mathrm{M}_\odot$)', fontsize=18)
ax[0].set_title("Water Lost (TO)", fontsize=20)
ax[1].set_title(r"$\mathrm{O}_2$ buildup (bar)", fontsize=20)

if (sys.argv[1] == 'pdf'):
    plt.savefig('O2BuildUp.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('O2BuildUp.png')
