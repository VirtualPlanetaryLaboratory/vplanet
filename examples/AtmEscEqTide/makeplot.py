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

mpl.rcParams['figure.figsize'] = (6.5,6)
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
fig, axes = plt.subplots(nrows=2, ncols=2)

time = cpl.b.Time/1e9

## Upper left: Water Mass ##
axes[0,0].plot(time, cpl.b.SurfWaterMass,color='k',label='CPL')
axes[0,0].plot(time, ctl.b.SurfWaterMass, color=vpl.colors.red,label='CTL')
# Format
axes[0,0].set_ylim(-0.02, 10)
axes[0,0].set_ylabel("Water Mass (Earth Oceans)")
axes[0,0].set_xlabel('Time (Gyr)')
axes[0,0].set_xlim(0,1)
axes[0,0].legend(loc="upper right", fontsize=12, ncol=1)

## Upper right: Oxygen mass  ##
axes[0,1].plot(time, cpl.b.OxygenMass, color='k')
axes[0,1].plot(time, ctl.b.OxygenMass, color=vpl.colors.red)
# Format
axes[0,1].set_ylim(0,2100)
axes[0,1].set_ylabel('Oxygen Pressure (bars)')
axes[0,1].set_xlabel('Time (Gyr)')
axes[0,1].set_xlim(0,1)

## Lower left: semi-major axis ##
axes[1,0].plot(time, cpl.b.SemiMajorAxis, color='k')
axes[1,0].plot(time, ctl.b.SemiMajorAxis, color=vpl.colors.red)
# Format
axes[1,0].set_ylabel('Semi-Major Axis (AU)')
axes[1,0].set_xlabel('Time (Gyr)')
axes[1,0].set_xlim(0,1)

## Lower Right: Eccentricity ##
axes[1,1].plot(time, cpl.b.Eccentricity, color='k')
axes[1,1].plot(time, ctl.b.Eccentricity, color=vpl.colors.red)
# Format
axes[1,1].set_ylim(0, 0.1)
axes[1,1].set_ylabel('Eccentricity')
axes[1,1].set_xlabel('Time (Gyr)')
axes[1,1].set_xlim(0,1)

for ax in axes.flatten():
    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
#fig.subplots_adjust(wspace=0.25)
#fig.subplots_adjust(hspace=0.05)

if (sys.argv[1] == 'pdf'):
    plt.savefig('AtmEscEqTideWater.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('AtmEscEqTideWater.png', bbox_inches="tight", dpi=200)

#exit()


### Second figure: Envelope loss with CPL ###

# Run simulations
os.chdir('EnvCPL/auto')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../Bondi')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../ELim')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../RR')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../..')

# Load data
cplauto = vpl.GetOutput("./EnvCPL/Auto")
cplbondi = vpl.GetOutput("./EnvCPL/Bondi")
cplelim = vpl.GetOutput("./EnvCPL/ELim")
cplrr = vpl.GetOutput("./EnvCPL/RR")

# Plot
fig, axes = plt.subplots(nrows=3, ncols=2)

timeauto = cplauto.auto.Time/1e6
timebondi = cplbondi.bondi.Time/1e6
timeelim = cplelim.el.Time/1e6
timerr = cplrr.rr.Time/1e6

## Upper left: Envelope Mass ##
axes[0,0].plot(timeauto, cplauto.auto.EnvelopeMass,color='k')
axes[0,0].plot(timebondi, cplbondi.bondi.EnvelopeMass, color=vpl.colors.red)
axes[0,0].plot(timeelim, cplelim.el.EnvelopeMass,color=vpl.colors.dark_blue)
axes[0,0].plot(timerr, cplrr.rr.EnvelopeMass, color=vpl.colors.pale_blue)
# Format
axes[0,0].set_ylim(-0.02, 1.1)
axes[0,0].set_ylabel(r"Envelope Mass (M$_\oplus$)")
axes[0,0].set_xlabel('Time (Myr)')
axes[0,0].set_xlim(0,1)

## Upper right: Radius  ##
axes[0,1].plot(timeauto, cplauto.auto.PlanetRadius, color='k')
axes[0,1].plot(timebondi, cplbondi.bondi.PlanetRadius, color=vpl.colors.red)
axes[0,1].plot(timeelim, cplelim.el.PlanetRadius, color=vpl.colors.dark_blue)
axes[0,1].plot(timerr, cplrr.rr.PlanetRadius, color=vpl.colors.pale_blue)
# Format
axes[0,1].set_ylim(0,35)
axes[0,1].set_ylabel(r'Radius (R$_\oplus$)')
axes[0,1].set_xlabel('Time (Myr)')
axes[0,1].set_xlim(0,1)

## Middle left: semi-major axis ##
axes[1,0].plot(timeauto, cplauto.auto.SemiMajorAxis, color='k')
axes[1,0].plot(timebondi, cplbondi.bondi.SemiMajorAxis, color=vpl.colors.red)
axes[1,0].plot(timeelim, cplelim.el.SemiMajorAxis, color=vpl.colors.dark_blue)
axes[1,0].plot(timerr, cplrr.rr.SemiMajorAxis, color=vpl.colors.pale_blue)
# Format
axes[1,0].set_ylim(0.0949,0.101)
axes[1,0].set_ylabel('Semi-Major Axis (AU)')
axes[1,0].set_xlabel('Time (Myr)')
axes[1,0].set_xlim(0,1)

## Middle Right: Eccentricity ##
axes[1,1].plot(timeauto, cplauto.auto.Eccentricity, color='k')
axes[1,1].plot(timebondi, cplbondi.bondi.Eccentricity, color=vpl.colors.red)
axes[1,1].plot(timeelim, cplelim.el.Eccentricity, color=vpl.colors.dark_blue)
axes[1,1].plot(timerr, cplrr.rr.Eccentricity, color=vpl.colors.pale_blue)
# Format
axes[1,1].set_ylim(0.05, 0.21)
axes[1,1].set_ylabel('Eccentricity')
axes[1,1].set_xlabel('Time (Myr)')
axes[1,1].set_xlim(0,1)

## Lower left: Rotation Period ##
axes[2,0].plot(timeauto, cplauto.auto.RotPer, color='k')
axes[2,0].plot(timebondi, cplbondi.bondi.RotPer,color=vpl.colors.red)
axes[2,0].plot(timeelim, cplelim.el.RotPer, color=vpl.colors.dark_blue)
axes[2,0].plot(timerr, cplrr.rr.RotPer,color=vpl.colors.pale_blue)
# Format
axes[2,0].set_xlabel("Time (yr)")
axes[2,0].set_ylim(0,12)
axes[2,0].set_ylabel('Rotation Period (days)')
axes[2,0].set_xlabel('Time (Myr)')
axes[2,0].set_xlim(0,1)

## Lower right: Obliquity ##
axes[2,1].plot(timeauto, cplauto.auto.Obliquity, color='k',label='Auto')
axes[2,1].plot(timebondi, cplbondi.bondi.Obliquity,color=vpl.colors.red,label='Bondi')
axes[2,1].plot(timeelim, cplelim.el.Obliquity, color=vpl.colors.dark_blue,label='E-Lim')
axes[2,1].plot(timerr, cplrr.rr.Obliquity,color=vpl.colors.pale_blue,label='RR-Lim')
# Format
axes[2,1].set_xlabel("Time (yr)")
axes[2,1].set_ylim(-1,100)
axes[2,1].set_ylabel('Obliquity (degrees)')
axes[2,1].set_xlabel('Time (Myr)')
axes[2,1].set_xlim(0,1)
axes[2,1].legend(loc="upper right", fontsize=8, ncol=1)

for ax in axes.flatten():
    # Set rasterization
    ax.set_rasterization_zorder(0)

fig.tight_layout()
#fig.subplots_adjust(wspace=0.25)
#fig.subplots_adjust(hspace=0.05)

if (sys.argv[1] == 'pdf'):
    plt.savefig('Lopez12CPL.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('Lopez12CPL.png', bbox_inches="tight", dpi=200)

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
