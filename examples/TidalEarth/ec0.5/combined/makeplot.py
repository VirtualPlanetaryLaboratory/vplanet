import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import vplot
import scipy.signal as sig
#plt.rcParams["text.usetex"]=True
#plt.rcParams["text.latex.unicode"]=True
plt.rcParams.update({'font.size':15,'legend.fontsize':15})
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

out = vplot.GetOutput()
# Print final state
print('Final: t=%.3f TUMan=%.1f TMan=%.1f TCMB=%.1f HflowUMan=%.1f HflowCMB=%.1f RadPowerMan=%.1f RadPowerCore=%.1f MagMom=%.1f RIC=%.1f PowerEqtide=%.2e K2=%.2f ImK2=%.2e a(AU)=%.2e ecc=%.2e'%(out.tidalearth1.Time[-1],out.tidalearth1.TUMan[-1],out.tidalearth1.TMan[-1],out.tidalearth1.TCMB[-1],out.tidalearth1.HflowUMan[-1],out.tidalearth1.HflowCMB[-1],out.tidalearth1.RadPowerMan[-1],out.tidalearth1.RadPowerCore[-1],out.tidalearth1.MagMom[-1],out.tidalearth1.RIC[-1],out.tidalearth1.PowerEqtide[-1],out.tidalearth1.K2[-1],out.tidalearth1.ImK2[-1],out.tidalearth1.SemiMajorAxis[-1],out.tidalearth1.Eccentricity[-1]))
# Print initial,final
print('initial final')
print('t %.3e %.3e'%(out.tidalearth1.Time[0],out.tidalearth1.Time[-1]))
print('a %.3e %.3e'%(out.tidalearth1.SemiMajorAxis[0],out.tidalearth1.SemiMajorAxis[-1]))
print('e %.3e %.3e'%(out.tidalearth1.Eccentricity[0],out.tidalearth1.Eccentricity[-1]))
print('TMan %.3e %.3e'%(out.tidalearth1.TMan[0],out.tidalearth1.TMan[-1]))
print('TCore %.3e %.3e'%(out.tidalearth1.TCore[0],out.tidalearth1.TCore[-1]))
print('HflowUMan %.3e %.3e'%(out.tidalearth1.HflowUMan[0],out.tidalearth1.HflowUMan[-1]))
#print('PowerEqtide %.3e %.3e'%(out.tidalearth.PowerEqtide[0],out.tidalearth.PowerEqtide[-1]))
print('PowerEqtide %.3e %.3e'%(out.tidalearth1.PowerEqtide[0],out.tidalearth1.PowerEqtide[-1]))
print('ImK2 %.3e %.3e'%(out.tidalearth1.ImK2[0],out.tidalearth1.ImK2[-1]))
print('K2 %.3e %.3e'%(out.tidalearth1.K2[0],out.tidalearth1.K2[-1]))
print('RadPowerMan %.3e %.3e'%(out.tidalearth1.RadPowerMan[0],out.tidalearth1.RadPowerMan[-1]))
print('RadPowerCore %.3e %.3e'%(out.tidalearth1.RadPowerCore[0],out.tidalearth1.RadPowerCore[-1]))
print('HflowCMB %.3e %.3e'%(out.tidalearth1.HflowCMB[0],out.tidalearth1.HflowCMB[-1]))
print('ViscUMan %.3e %.3e'%(out.tidalearth1.ViscUMan[0],out.tidalearth1.ViscUMan[-1]))
print('ShmodUMan %.3e %.3e'%(out.tidalearth1.ShmodUMan[0],out.tidalearth1.ShmodUMan[-1]))

# Plots
rows=3
cols=2
# Mantle Figure
nfig=1
fig = plt.figure(nfig, figsize=(10,15))
panel=1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.TMan,linestyle='-',label=r'$T_{M}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.TUMan,linestyle='-',label=r'$T_{UM}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.TLMan,linestyle='-',label=r'$T_{LM}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.TCMB,linestyle='-',label=r'$T_{CMB}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.TCore,'k-',label=r'$T_{C}$')
plt.legend(loc='best',ncol=2,frameon=False,columnspacing=1)
plt.ylabel('Temperature (K)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,10000)
plt.xscale('log')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.HflowUMan,linestyle='-',label=r'$Q_{UMan}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.HflowMeltMan,linestyle='-',label=r'$Q_{Melt,Man}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.HflowCMB,linestyle='-',label=r'$Q_{CMB}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.RadPowerMan,linestyle='-',label=r'$Q_{Rad,Man}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.RadPowerCore,'-',label=r'$Q_{Rad,Core}$')
plt.plot(out.tidalearth1.Time,out.tidalearth1.PowerEqtide,'-',label=r'PowerEqtide')
plt.yscale('log'); plt.xscale('log')
plt.legend(loc='best',frameon=False)
plt.ylabel('Heat Flow (TW)')
plt.xlabel('Time (Gyr)')
ymax=np.max([out.tidalearth1.PowerEqtide[50:].max(),out.tidalearth1.HflowUMan[50:].max()])
ymax=1e6
plt.ylim(1,ymax)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.SemiMajorAxis,label='a(AU)')
plt.ylabel(r'Semi Major Axis (AU)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,0.10)
plt.xscale('log')
panel += 1
plt.subplot(rows,cols,panel)
plt.loglog(out.tidalearth1.Time,out.tidalearth1.Eccentricity,label='ecc')
plt.ylabel(r'Eccentricity')
plt.xlabel('Time (Gyr)')
plt.ylim(1e-5,1)
plt.yscale('log'); plt.xscale('log')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.MagMom,label='MagMom')
plt.ylim(0,2)
plt.ylabel('Magnetic Moment (E. Units)')
plt.xlabel('Time (Gyr)')
plt.xscale('log')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.MeltMassFluxMan*s_yr)
plt.ylabel(r'Melt Mass Flux Mantle (kg$/$yr)')
plt.xlabel('Time (Gyr)')
plt.ylim(1e12,1e18)
plt.yscale('log'); plt.xscale('log')

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)

# Core Plots
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.RIC,label='RIC')
#plt.ylim(0,1500)
plt.ylabel(r'Inner Core Radius (km)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.CoreBuoyTherm*1e13,label='Thermal')
plt.plot(out.tidalearth1.Time,out.tidalearth1.CoreBuoyCompo*1e13,label='Compositional')
plt.plot(out.tidalearth1.Time,out.tidalearth1.CoreBuoyTotal*1e13,label='Total')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Core Buoyancy Flux ($\times10^{-13}$ m$^2/$s$^3$)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.MagMom,label='MagMom')
plt.ylim(0,2)
plt.ylabel('Magnetic Moment (E. Units)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.MagPauseRad)
plt.ylabel(r'Magnetopause Radius (E. Units)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.ChiOC,label='ChiOC')
plt.plot(out.tidalearth1.Time,out.tidalearth1.ChiIC,label='ChiIC')
plt.ylim(0,0.2)
plt.ylabel(r'Core Light Element Concentration')
plt.xlabel('Time (Gyr)')
plt.legend(loc='best',frameon=False)

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)

# Orbital Evolution Plots
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.K2,label='K2')
#plt.ylim(0,1)
plt.ylabel(r'K2')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.ImK2,label='ImK2')
plt.ylabel(r'ImK2')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.Time,out.tidalearth1.SemiMajorAxis,label='a(AU)')
plt.ylabel(r'Semi Major Axis (AU)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.loglog(out.tidalearth1.Time,out.tidalearth1.Eccentricity,label='ecc')
plt.ylabel(r'Eccentricity')
plt.xlabel('Time (Gyr)')
plt.ylim(1e-5,1)
plt.yscale('log'); plt.xscale('log')

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)


# Temperature-dep Tidal Properties
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.TUMan,out.tidalearth1.FMeltUMan,label='FMeltUMan')
plt.ylabel(r'FMeltUMan')
plt.xlabel('TUMan')
plt.ylim(0,1)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.TUMan,out.tidalearth1.BLUMan,label='BLUMan')
plt.ylabel(r'BLUMan (km)')
plt.xlabel('TUMan')
plt.ylim(0)
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.tidalearth1.TUMan,out.tidalearth1.ViscUMan,label='ViscUMan')
plt.semilogy(out.tidalearth1.TUMan,out.tidalearth1.ShmodUMan,label='ShmodUMan')
plt.ylabel(r'ViscUMan, ShmodUMan')
plt.xlabel('TUMan')
plt.legend()
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.tidalearth1.TUMan,out.tidalearth1.K2,label='K2')
plt.ylim(0,1.5)
plt.ylabel(r'K2')
plt.xlabel('TUMan')
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.tidalearth1.TUMan,np.abs(out.tidalearth1.ImK2),label='ImK2')
plt.ylabel(r'ImK2')
plt.xlabel('TUMan')
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.tidalearth1.TUMan,out.tidalearth1.PowerEqtide,label='PowerEqtide')
plt.ylabel(r'PowerEqtide [TW]')
plt.xlabel('TUMan')
vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)


# Temperature-dep Orbital evo
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
planet=out.tidalearth1
plt.semilogy(planet.TUMan,planet.PowerEqtide,label=planet.name)
planet=out.tidalearth2
plt.semilogy(planet.TUMan,planet.PowerEqtide,label=planet.name)
planet=out.tidalearth3
plt.semilogy(planet.TUMan,planet.PowerEqtide,label=planet.name)
plt.ylabel(r'PowerEqtide [TW]')
plt.xlabel('TUMan')
plt.ylim(1e-8,1e4)
plt.legend()
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.tidalearth1.TUMan,out.tidalearth1.Eccentricity,label='Ecc')
plt.ylabel(r'Eccentricity')
plt.xlabel('TUMan')
plt.ylim(1e-8,1e0)
vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig(filepref+'%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig(filepref+'%d.png'%nfig)
    
plt.close()
