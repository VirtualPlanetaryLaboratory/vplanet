import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import vplot as vpl
import sys
import os
import subprocess


# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

mpl.rcParams['figure.figsize'] = (6.5,8)
mpl.rcParams['font.size'] = 10.0

### First figure: water photolysis escape ###

# Run simulations
os.chdir('WaterCPL')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../WaterCTL')
subprocess.call(['vplanet', 'vpl.in'])
os.chdir('..')

# Load data
cpl = vpl.GetOutput("./WaterCPL")
ctl = vpl.GetOutput("./WaterCTL")

# Plot
fig, axes = plt.subplots(nrows=3, ncols=2)

time = cpl.b.Time/1e6

## Upper left: Water Mass ##
axes[0,0].plot(time, cpl.b.SurfWaterMass,color='k')
axes[0,0].plot(time, ctl.b.SurfWaterMass, color=vpl.colors.red)
# Format
axes[0,0].set_ylim(-0.02, 10)
axes[0,0].set_ylabel("Water Mass (Earth Oceans)")
axes[0,0].set_xlabel('Time (Gyr)')
axes[0,0].set_xlim(0,100)

## Upper right: Oxygen mass  ##
axes[0,1].plot(time, cpl.b.OxygenMass, color='k')
axes[0,1].plot(time, ctl.b.OxygenMass, color=vpl.colors.red)
# Format
axes[0,1].set_ylim(0,2000)
axes[0,1].set_ylabel('Oxygen Pressure (bars)')
axes[0,1].set_xlabel('Time (Gyr)')
axes[0,1].set_xlim(0,100)
#axes[0,1].legend(loc="upper left", framealpha=0, fontsize=12, ncol=1)

## Middle left: semi-major axis ##
axes[1,0].plot(time, cpl.b.SemiMajorAxis, color='k')
axes[1,0].plot(time, ctl.b.SemiMajorAxis, color=vpl.colors.red)
# Format
axes[1,0].set_ylabel('Semi-Major Axis (AU)')
axes[1,0].set_xlabel('Time (Gyr)')
axes[1,0].set_xlim(0,100)

## Middle Right: Eccentricity ##
axes[1,1].plot(time, cpl.b.Eccentricity, color='k')
axes[1,1].plot(time, ctl.b.Eccentricity, color=vpl.colors.red)
# Format
axes[1,1].set_ylim(0, 0.1)
axes[1,1].set_ylabel('Eccentricity')
axes[1,1].set_xlabel('Time (Gyr)')
axes[1,1].set_xlim(0,100)

time = cpl.b.Time/1e6

## Lower left: Rotation Period ##
axes[2,0].plot(time, cpl.b.RotPer, color='k')
axes[2,0].plot(time, ctl.b.RotPer,color=vpl.colors.red)
# Format
axes[2,0].set_xlabel("Time (yr)")
axes[2,0].set_ylim(0.5,2)
axes[2,0].set_ylabel('Rotation Period (days)')
axes[2,0].set_xlabel('Time (kyr)')
axes[2,0].set_xlim(0,100)

## Lower right: Obliquity ##
axes[2,1].plot(time, cpl.b.Obliquity, color='k')
axes[2,1].plot(time, ctl.b.Obliquity,color=vpl.colors.red)
# Format
axes[2,1].set_xlabel("Time (yr)")
axes[2,1].set_ylim(-0.05,45)
axes[2,1].set_ylabel('Obliquity (degrees)')
axes[2,1].set_xlabel('Time (kyr)')
axes[2,1].set_xlim(0,100)


for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time[1],time.max())
    #ax.set_xscale("log")

    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
#fig.subplots_adjust(wspace=0.25)
#fig.subplots_adjust(hspace=0.05)

if (sys.argv[1] == 'pdf'):
    plt.savefig('AtmEscEqTideWater.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('AtmEscEqTideWater.png', bbox_inches="tight", dpi=200)

exit()

### Second figure: Zoom-in on Bondi-limited escape ###

# Plot
fig, axes = plt.subplots(nrows=2, ncols=3, sharex=True, figsize=(16,9))

time = bondi.auto.Time

## Upper left: Envelope mass fraction ##
axes[0,0].plot(time, bondi.auto.EnvelopeMass/bondi.auto.Mass, color="C0")
axes[0,0].plot(time, bondi.bondi.EnvelopeMass/bondi.bondi.Mass, color="C3")

# Initial value
axes[0,0].axhline(bondi.auto.EnvelopeMass[0]/bondi.auto.Mass[0], color="k",
                  ls="--")

# Format
axes[0,0].set_ylim(-0.02, 0.52)
axes[0,0].set_ylabel("Envelope Mass Fraction")

## Upper middle: Envelope mass time derivative ##
axes[0,1].plot(time, bondi.auto.DEnvMassDt, color="C0", label="AtmEscAuto")
axes[0,1].plot(time, bondi.bondi.DEnvMassDt, color="C3", label="Bondi-Limited")

# Format
axes[0,1].set_ylim(-1.0e15, -1.0e9)
axes[0,1].set_yscale("symlog", linthreshy=0.1)
axes[0,1].set_ylabel(r"$\dot{M}_{envelope}$ [kg/s]")
axes[0,1].legend(loc="upper left", framealpha=0, fontsize=12, ncol=1)

## Upper left: planetary mass ##
axes[0,2].plot(time, bondi.auto.Mass, color="C0")
axes[0,2].plot(time, bondi.bondi.Mass, color="C3")

# Initial value
axes[0,2].axhline(bondi.auto.Mass[0], color="k", ls="--")

# Format
axes[0,2].set_ylabel(r"Mass [M$_{\oplus}$]")

## Lower left: Planet Radius ##
axes[1,0].plot(time, bondi.auto.PlanetRadius, color="C0", label="")
axes[1,0].plot(time, bondi.bondi.PlanetRadius, color="C3", label="")

# Initial value
axes[1,0].axhline(bondi.auto.PlanetRadius[0], color="k", ls="--", label="")

# Format
axes[1,0].set_ylim(0, 35)
axes[1,0].set_ylabel(r"Radius [R$_{\oplus}$]")
axes[1,0].set_xlabel("Time [yr]")

## Lower middle: Critical FXUV ##
axes[1,1].plot(time, bondi.auto.RRCriticalFlux, color="C0", label="")
axes[1,1].plot(time, bondi.bondi.RRCriticalFlux, color="C3", label="")
axes[1,1].plot(time, bondi.auto.FXUV, color="k", label="Incident\nXUV Flux")

# Format
axes[1,1].set_xlabel("Time [yr]")
axes[1,1].set_ylim(1.0e-2, 200)
axes[1,1].set_yscale("log")
axes[1,1].set_ylabel(r"Critical XUV Flux [W/m$^2$]")
axes[1,1].legend(bbox_to_anchor=[0.4, 0.8], framealpha=0, fontsize=13)

## Lower right: Escape regime ##
axes[1,2].plot(time, bondi.auto.HEscapeRegime, lw=2, color="C0", label="")
axes[1,2].plot(time, bondi.bondi.HEscapeRegime, lw=2, color="C3", label="")

# Annotate with the regime
axes[1,2].text(0.7, 0.8, 'Envelope Lost', horizontalalignment='left',
               verticalalignment='center', transform=axes[1,2].transAxes, fontsize=14)
axes[1,2].text(0.7, 0.58, 'RR-Limited', horizontalalignment='left',
               verticalalignment='center', transform=axes[1,2].transAxes, fontsize=14)
axes[1,2].text(0.03, 0.48, 'Bondi-Limited', horizontalalignment='left',
               verticalalignment='center', transform=axes[1,2].transAxes, fontsize=14)

# Format
axes[1,2].set_xlabel("Time [yr]")
axes[1,2].set_ylabel("Escape Regime")
axes[1,2].set_yticks([])
axes[1,2].set_ylim(1, 10)

# Format all axes
for ax in axes.flatten():

    # Format x axis
    ax.set_xlim(time[0],time.max())
    #ax.set_xscale("log")

    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
fig.subplots_adjust(wspace=0.25)
fig.subplots_adjust(hspace=0.05)

if (sys.argv[1] == 'pdf'):
    plt.savefig('AtmEscRegimesZoom.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('AtmEscRegimesZoom.png', bbox_inches="tight", dpi=200)
