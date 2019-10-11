import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import sys
import os
import subprocess
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

#Typical plot parameters that make for pretty plot
mpl.rcParams['figure.figsize'] = (10,8)
mpl.rcParams['font.size'] = 15.0

# Run the simulations
dir_path = os.path.dirname(os.path.realpath(__file__))
dirs = ["LehmerCatling17", "Dynamic"]

# Run simulations
for dir in dirs:
    os.chdir(os.path.join(dir_path,dir))

    # Run simulation
    subprocess.call(['vplanet', 'vpl.in'])
# Return to top-level directory
os.chdir(dir_path)

# Load data
lc17 = vpl.GetOutput("./LehmerCatling17")
dyn = vpl.GetOutput("./Dynamic")

### First figure: Comparative atmospheric escape ###

# Plot
fig, axes = plt.subplots(nrows=4, ncols=3, sharex=True, figsize=(8.5,11))

time = lc17.star.Time

## Upper left: Envelope mass ##
axes[0,0].plot(time, lc17.planet.EnvelopeMass, color='k')
axes[0,0].plot(time, dyn.planet.EnvelopeMass, color=vpl.colors.dark_blue)

# Format
axes[0,0].set_ylim(-0.01, 0.07)
axes[0,0].set_ylabel(r"Envelope Mass ($M_\oplus$)")

## Upper middle: Total Mass ##
axes[0,1].plot(time, lc17.planet.Mass, color='k')
axes[0,1].plot(time, dyn.planet.Mass, color=vpl.colors.dark_blue)

# Format
axes[0,1].set_ylim(1.9, 2)
axes[0,1].set_ylabel(r"Total Mass ($M_\oplus$)")

## Upper right: Envelope mass time derivative ##
axes[0,2].plot(time, lc17.planet.DEnvMassDt, color='k')
axes[0,2].plot(time, dyn.planet.DEnvMassDt, color=vpl.colors.dark_blue)

axes[0,2].set_ylim(-0.01, 1e-4)
axes[0,2].set_yscale("symlog", linthreshy=0.1)
axes[0,2].set_ylabel(r"$\dot{M}_{envelope}$ [$M_\oplus/Myr$]")

## Upper Middle left: Stellar Luminosity
axes[1,0].plot(time, lc17.star.Luminosity, color='k')
axes[1,0].plot(time, dyn.star.Luminosity, color=vpl.colors.dark_blue)

# Format
axes[1,0].set_ylim(0.25, 2)
axes[1,0].set_ylabel(r"Luminosity ($L_\odot$)")

## Upper Middle middle: XUV Luminosity
axes[1,1].plot(time, lc17.star.LXUV, color='k')
axes[1,1].plot(time, dyn.star.LXUV, color=vpl.colors.dark_blue)

# Format
axes[1,1].set_ylim(1e-4, 2e-3)
axes[1,1].set_ylabel(r"$L_{XUV}$ ($L_\odot$)")

## Upper Right middle: Incident XUV flux
axes[1,2].plot(time, lc17.planet.FXUV, color='k')
axes[1,2].plot(time, dyn.planet.FXUV, color=vpl.colors.dark_blue)

# Format
axes[1,2].set_ylim(50,250)
axes[1,2].set_ylabel(r"$F_{XUV}$ ($W/m^2$)")

## Lower Middle left: Planetary radius
axes[2,0].plot(time, lc17.planet.Radius, color='k')
axes[2,0].plot(time, dyn.planet.Radius, color=vpl.colors.dark_blue)

# Format
axes[2,0].set_ylim(1,30)
axes[2,0].set_ylabel(r"Radius ($R_\oplus$)")

## Lower Middle middle: Planetary XUV radius
axes[2,1].plot(time, lc17.planet.RadXUV, color='k')
axes[2,1].plot(time, dyn.planet.RadXUV, color=vpl.colors.dark_blue)

# Format
axes[2,1].set_ylim(1,30)
axes[2,1].set_ylabel(r"XUV Radius ($R_\oplus$)")

## Lower Middle right: Roche radius
axes[2,1].plot(time, lc17.planet.RocheRadius, color='k')
axes[2,1].plot(time, dyn.planet.RocheRadius, color=vpl.colors.dark_blue)

# Format
axes[2,1].set_ylim(1,30)
axes[2,1].set_ylabel(r"Roche Lobe Radius ($R_\oplus$)")

## Lower left: Scale height
axes[3,0].plot(time, lc17.planet.ScaleHeight, color='k')
axes[3,0].plot(time, dyn.planet.ScaleHeight, color=vpl.colors.dark_blue)

# Format
axes[3,0].set_ylim(300,450)
axes[3,0].set_ylabel("Scale Height (km)")

## Lower middle: Surface Pressure
axes[3,1].plot(time, lc17.planet.PresSurf, color='k')
axes[3,1].plot(time, dyn.planet.PresSurf, color=vpl.colors.dark_blue)

# Format
axes[3,1].set_ylim(-0.01,7)
axes[3,1].set_ylabel("Scale Height (GPa)")

## Lower right: Thermal Temperature
axes[3,2].plot(time, lc17.planet.ThermTemp, color='k')
axes[3,2].plot(time, dyn.planet.ThermTemp, color=vpl.colors.dark_blue)

# Format
axes[3,2].set_ylim(900,1500)
axes[3,2].set_ylabel("Thermal Temp. (K)")

# Format
axes[3,1].set_xlabel("Time [yr]")

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time[1],time.max())
    ax.set_xscale("log")

    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
fig.subplots_adjust(wspace=0.25)
fig.subplots_adjust(hspace=0.05)

if (sys.argv[1] == 'pdf'):
    plt.savefig('MiniNeptuneEvap.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('MiniNeptuneEvap.png', bbox_inches="tight", dpi=200)

plt.close()
