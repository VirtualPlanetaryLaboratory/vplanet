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

# Run simulations
os.chdir('Lopez12CPL/auto')
print('Running Lopez12CPL-Auto.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../Bondi')
print('Running Lopez12CPL-Bondi.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../ELim')
print('Running Lopez12CPL-ELim.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../RR')
print('Running Lopez12CPL-RR.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../..')

# Load data
cplauto = vpl.get_output("./Lopez12CPL/Auto")
cplbondi = vpl.get_output("./Lopez12CPL/Bondi")
cplelim = vpl.get_output("./Lopez12CPL/ELim")
cplrr = vpl.get_output("./Lopez12CPL/RR")

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
if (sys.argv[1] == 'pdf'):
    plt.savefig('Lopez12CPL.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('Lopez12CPL.png', bbox_inches="tight", dpi=200)
plt.close()
