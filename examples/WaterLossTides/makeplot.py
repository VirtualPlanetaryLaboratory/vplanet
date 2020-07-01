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
print('Running WaterCPL.')
subprocess.call(['vplanet', 'vpl.in'])

os.chdir('../WaterCTL')
print('Running WaterCTL.')
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
if (sys.argv[1] == 'pdf'):
    plt.savefig('WaterLossTides.pdf', bbox_inches="tight", dpi=200)
if (sys.argv[1] == 'png'):
    plt.savefig('WaterLossTides.png', bbox_inches="tight", dpi=200)
plt.close()
