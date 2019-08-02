import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import vplot
import scipy.signal as sig
import os
import subprocess
#plt.rcParams["text.usetex"]=True
#plt.rcParams["text.latex.unicode"]=True
plt.rcParams.update({'font.size':16,'legend.fontsize':16})
import sys

s_yr = 3600.*24*365

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)
filepref = 'TidalEarth'

dir_path = os.path.dirname(os.path.realpath(__file__))
dirs = ["au0.01","au0.02","au0.05"]
# Run the simulations
for dir in dirs:
    print("\nRunning simulation in %s directory..." % dir)
    os.chdir(os.path.join(dir_path,dir))
    subprocess.call(['vplanet', 'vpl.in'])
# load data
outputs = [vplot.GetOutput(str(os.path.join(dir_path,dir))) for dir in dirs]
#out0 = vplot.GetOutput(dirs[0])
out0 = outputs[0]
out1 = outputs[1]
out2 = outputs[2]

# Print final state
out = out0

def fig2x3(out,nfig,color='k',legendon=False):
    fig = plt.figure(nfig, figsize=(10,15))
    panel=1
    plt.subplot(rows,cols,panel)
    plt.plot(out.tidalearth.Time,out.tidalearth.TMan,linestyle='-',color=color,label=r'$T_{M}$')
    plt.plot(out.tidalearth.Time,out.tidalearth.TCore,'--',color=color,label=r'$T_{C}$')
    if legendon: plt.legend(loc='best',ncol=2,frameon=True)
    plt.ylabel('Temperature (K)')
    plt.xlabel('Time (Gyr)')
    plt.ylim(0,6e3)
    plt.xscale('log')
    panel += 1
    plt.subplot(rows,cols,panel)
    plt.plot(out.tidalearth.Time,out.tidalearth.HflowUMan,linestyle='-',color=color,label=r'$Q_{UMan}$')
    plt.plot(out.tidalearth.Time,out.tidalearth.HflowMeltMan,linestyle=(0, (3, 5, 1, 5, 1, 5)),color=color,label=r'$Q_{Melt,Man}$')
    plt.plot(out.tidalearth.Time,out.tidalearth.HflowCMB,linestyle='--',color=color,label=r'$Q_{CMB}$')
    plt.plot(out.tidalearth.Time,out.tidalearth.RadPowerMan,linestyle=(0, (1, 5)),color=color,label=r'$Q_{Rad,Man}$')
    plt.plot(out.tidalearth.Time,out.tidalearth.PowerEqtide,'-.',color=color,label=r'$Q_{Tide}$')
    plt.yscale('log'); plt.xscale('log')
    if legendon: plt.legend(loc='upper right',frameon=True,ncol=2,columnspacing=1)
    plt.ylabel('Power (TW)')
    plt.xlabel('Time (Gyr)')
    ymax=np.max([out.tidalearth.PowerEqtide[50:].max(),out.tidalearth.HflowUMan[50:].max()])
    ymax=1e10
    plt.ylim(1e-4,ymax)
    panel += 1
    plt.subplot(rows,cols,panel)
    plt.plot(out.tidalearth.Time,out.tidalearth.SemiMajorAxis,color=color,label='$a_0$=%.2f'%out.tidalearth.SemiMajorAxis[0])
    plt.ylabel(r'Semi-Major Axis (AU)')
    plt.xlabel('Time (Gyr)')
    plt.ylim(0,0.10)
    plt.xscale('log')
    plt.legend(frameon=True,loc='upper left')
    panel += 1
    plt.subplot(rows,cols,panel)
    plt.loglog(out.tidalearth.Time,out.tidalearth.Eccentricity,color=color,label='ecc')
    plt.ylabel(r'Eccentricity')
    plt.xlabel('Time (Gyr)')
    plt.ylim(1e-5,1)
    plt.yscale('log'); plt.xscale('log')
    panel += 1
    plt.subplot(rows,cols,panel)
    plt.plot(out.tidalearth.Time,out.tidalearth.MagMom,color=color,label='MagMom')
    plt.plot(out.tidalearth.Time,out.tidalearth.RIC/3481,linestyle='--')
    plt.ylim(0,1.5)
    plt.ylabel('Mag. Mom., R$_{ic}$ ($\oplus$ Units)')
    plt.xlabel('Time (Gyr)')
    plt.xscale('log')
    panel += 1
    plt.subplot(rows,cols,panel)
    plt.plot(out.tidalearth.Time,out.tidalearth.MeltMassFluxMan*s_yr,color=color)
    plt.ylabel(r'Melt Mass Flux Mantle (kg$/$yr)')
    plt.xlabel('Time (Gyr)')
    plt.ylim(1e12,1e18)
    plt.yscale('log'); plt.xscale('log')

    vplot.make_pretty(fig)
    if (sys.argv[1] == 'pdf'):
        plt.savefig(filepref+'%d.pdf'%nfig)
    if (sys.argv[1] == 'png'):
        plt.savefig(filepref+'%d.png'%nfig)

# Plots
rows=3
cols=2
# Mantle Figure
nfig=1
colors=['red','black','blue']
fig=fig2x3(out1,nfig,colors[1],legendon=True)
fig=fig2x3(out0,nfig,colors[0],legendon=False)
fig=fig2x3(out2,nfig,colors[2],legendon=False)

# Temperature-dep Orbital evo
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
planet=out0.tidalearth
plt.semilogy(planet.TUMan,planet.PowerEqtide,color=colors[0],label='$a_0$=%.2f'%planet.SemiMajorAxis[0])
planet=out1.tidalearth
plt.semilogy(planet.TUMan,planet.PowerEqtide,color=colors[1],label='$a_0$=%.2f'%planet.SemiMajorAxis[0])
planet=out2.tidalearth
plt.semilogy(planet.TUMan,planet.PowerEqtide,color=colors[2],label='$a_0$=%.2f'%planet.SemiMajorAxis[0])
plt.ylabel(r'Tidal Power [TW]')
plt.xlabel('Upper Mantle Temp. [K]')
plt.ylim(1e-8,1e4)
plt.xlim(1600,2400)
plt.legend()
panel += 1
plt.subplot(rows,cols,panel)
planet=out0.tidalearth
plt.semilogy(planet.TUMan,planet.Eccentricity,color=colors[0],label='Ecc')
planet=out1.tidalearth
plt.semilogy(planet.TUMan,planet.Eccentricity,color=colors[1],label='Ecc')
planet=out2.tidalearth
plt.semilogy(planet.TUMan,planet.Eccentricity,color=colors[2],label='Ecc')
plt.ylabel(r'Eccentricity')
plt.xlabel('Upper Mantle Temp. [K]')
plt.ylim(1e-8,1e0)
plt.xlim(1600,2400)
vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)

plt.close()
