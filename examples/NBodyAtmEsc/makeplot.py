import numpy as np
import matplotlib.pyplot as plt
import sys
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')
import os
import subprocess as subp

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

# Run Simulations
os.chdir("AtmEsc")
print("Running AtmEsc.")
#subp.call(['vplanet','vpl.in'])

os.chdir("../NBody")
print("Running NBody.")
#subp.call(['vplanet','vpl.in'])

os.chdir("..")
print("Running VPLanet.")
subp.call(['vplanet','vpl.in'])

output = vpl.GetOutput()

# Plot
fig, axes = plt.subplots(nrows=2, ncols=2)

time = output.A.Time/1e3

## Upper left: Envelope Mass ##
axes[0,0].plot(time, output.A.EnvelopeMass,color='k')
# Format
axes[0,0].set_ylim(-0.002, 1)
axes[0,0].set_ylabel(r"Envelope Mass (M$_\oplus$)")
axes[0,0].set_xlabel('Time (kyr)')
axes[0,0].set_xlim(0,100)

## Upper right: Radius  ##
axes[0,1].plot(time, output.A.PlanetRadius, color='k',label='A')
axes[0,1].plot(time, output.B.PlanetRadius*1e6, color=vpl.colors.red,label='B')
axes[0,1].plot(time, output.C.PlanetRadius*1e6, color=vpl.colors.pale_blue,label='C')
# Format
axes[0,1].set_ylim(0.1,100)
axes[0,1].set_ylabel(r'Radius (R$_\oplus$)')
axes[0,1].set_xlabel('Time (kyr)')
axes[0,1].set_xlim(0,100)
axes[0,1].set_yscale('log')
axes[0,1].legend(loc="lower left")

## Lower left: Eccentricity  ##
axes[1,0].plot(time, output.A.Eccentricity,color='k')
axes[1,0].plot(time, output.B.Eccentricity,color=vpl.colors.red)
axes[1,0].plot(time, output.C.Eccentricity,color=vpl.colors.pale_blue)
axes[1,0].set_xlabel('Time (kyrs)')
axes[1,0].set_ylabel('Eccentricity')

## Lower right: Inclination  ##
axes[1,1].plot(time, output.A.SpiNBodyInc,'k')
axes[1,1].plot(time, output.B.SpiNBodyInc,color=vpl.colors.red)
axes[1,1].plot(time, output.C.SpiNBodyInc,color=vpl.colors.pale_blue)
axes[1,1].set_xlabel('Time (kyrs)')
axes[1,1].set_ylabel('Inclination ($^\circ$)')

if (sys.argv[1] == 'pdf'):
    plt.savefig('NBodyAtmEsc.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('NBodyAtmEsc.png')

plt.close()
