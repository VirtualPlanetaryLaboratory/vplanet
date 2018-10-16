import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import vplot
import scipy.signal as sig
#plt.rcParams["text.usetex"]=True
#plt.rcParams["text.latex.unicode"]=True
plt.rcParams.update({'font.size':15,'legend.fontsize':15})
import sys

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

out = vplot.GetOutput()
# Print final state
print('Final: t=%.3f TUMan=%.1f TMan=%.1f TCMB=%.1f HflowUMan=%.1f HflowCMB=%.1f RadPowerMan=%.1f RadPowerCore=%.1f MagMom=%.1f RIC=%.1f'%(out.earth.Time[-1],out.earth.TUMan[-1],out.earth.TMan[-1],out.earth.TCMB[-1],out.earth.HflowUMan[-1],out.earth.HflowCMB[-1],out.earth.RadPowerMan[-1],out.earth.RadPowerCore[-1],out.earth.MagMom[-1],out.earth.RIC[-1]))

# Plots
rows=3
cols=2
# Mantle Figure
nfig=1
fig = plt.figure(nfig, figsize=(10,15))
panel=1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.TMan,color=vplot.colors.red,linestyle='-',label=r'$T_{M}$')
plt.plot(out.earth.Time,out.earth.TUMan,color=vplot.colors.orange,linestyle='-',label=r'$T_{UM}$')
plt.plot(out.earth.Time,out.earth.TLMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$T_{LM}$')
plt.plot(out.earth.Time,out.earth.TCMB,color=vplot.colors.purple,linestyle='-',label=r'$T_{CMB}$')
plt.plot(out.earth.Time,out.earth.TCore,'k-',label=r'$T_{C}$')
plt.legend(loc='best',ncol=2,frameon=False,columnspacing=1)
plt.ylabel('Temperature (K)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,10000)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.HflowUMan,color=vplot.colors.red,linestyle='-',label=r'$Q_{UMan}$')
plt.plot(out.earth.Time,out.earth.HflowCMB,color=vplot.colors.orange,linestyle='-',label=r'$Q_{CMB}$')
plt.plot(out.earth.Time,out.earth.RadPowerMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$Q_{Rad,Man}$')
plt.plot(out.earth.Time,out.earth.RadPowerCore,'k-',label=r'$Q_{Rad,Core}$')
plt.legend(loc='best',frameon=False)
plt.ylabel('Heat Flow (TW)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,150)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.BLUMan,label=r'$\delta_{UM}$')
plt.plot(out.earth.Time,out.earth.BLLMan,label=r'$\delta_{LM}$')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Boundary Layer Depths (km)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.earth.Time,out.earth.ViscUMan,label=r'$\nu_{UM}$')
plt.semilogy(out.earth.Time,out.earth.ViscLMan,label=r'$\nu_{LM}$')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Mantle Viscosity ($m^2s^{-1}$)')
plt.xlabel('Time (Gyr)')
plt.ylim(1e12,1e19)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.FMeltUMan)
plt.ylabel(r'Melt Fraction Upper Mantle (n.d.)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MeltMassFluxMan*1e-6)
plt.ylabel(r'Melt Mass Flux Mantle ($\times 10^6$ kg$/$s)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,100)

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('EarthInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('EarthInterior%d.png'%nfig)

# Core Plots
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.RIC,label='RIC')
plt.ylim(0,1500)
plt.ylabel(r'Inner Core Radius (km)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.CoreBuoyTherm*1e13,label='Thermal')
plt.plot(out.earth.Time,out.earth.CoreBuoyCompo*1e13,label='Compositional')
plt.plot(out.earth.Time,out.earth.CoreBuoyTotal*1e13,label='Total')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Core Buoyancy Flux ($\times10^{-13}$ m$^2/$s$^3$)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MagMom,label='MagMom')
plt.ylim(0,2)
plt.ylabel('Magnetic Moment (E. Units)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MagPauseRad)
plt.ylabel(r'Magnetopause Radius (E. Units)')
plt.xlabel('Time (Gyr)')

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('EarthInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('EarthInterior%d.png'%nfig)
plt.close()
