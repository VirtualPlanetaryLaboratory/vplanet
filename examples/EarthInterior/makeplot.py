import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')
import scipy.signal as sig
import subprocess
#plt.rcParams["text.usetex"]=True
#plt.rcParams["text.latex.unicode"]=True
plt.rcParams.update({'font.size':16,'legend.fontsize':15})

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

#Runs VPLANET
subprocess.call(['vplanet', 'vpl.in'])

out = vplot.GetOutput()
# Print final state
#print('Final: t=%.3f TUMan=%f TMan=%f TCMB=%f TCore=%f HflowUMan=%.1f HflowCMB=%.1f RadPowerTotal=%f RadPowerMan=%.1f RadPowerCore=%.1f MagMom=%f RIC=%f'%(out.earth.Time[-1],out.earth.TUMan[-1],out.earth.TMan[-1],out.earth.TCMB[-1],out.earth.TCore[-1],out.earth.HflowUMan[-1],out.earth.HflowCMB[-1],out.earth.RadPowerTotal[-1],out.earth.RadPowerMan[-1],out.earth.RadPowerCore[-1],out.earth.MagMom[-1],out.earth.RIC[-1]))

### Uncertainty ranges
TUMan_ra = np.array([1280.,1475.])+273.        #[K] Jaupart (2015) Table 4.
TCMB_ra = np.array([3800,4200.])               #[K] Hirose (2013) Table 2.
HflowUMan_ra = np.array([35,41.])              #[TW] Jaupart (2015) Table 12.
HflowUMan_ra = np.array([35,41.])               #[TW] Jaupart (2015) Table 12.
HflowCMB_ra = np.array([5,17])                  #[TW] Jaupart (2015) Table 12.
ViscUMan_ra = np.array([1.5e19,1.5e22])/3300.  #[m^2/s] Paulson (2005) Fig 3.
ViscLMan_ra = np.array([3e19,1.5e22])/5200.    #[m^2/s] Paulson (2005) Fig 3.
MeltMassFlux_ra = np.array([0.52,4*.52])        #[1e6 kg/s] Cogne (2004) 5-15 km^3/yr.  Li (2015) ~20 km^3/yr
FMeltUMan_ra = np.array([0.07,0.15])            # refs?

### Hi/lo
TUMan_lo = np.abs(TUMan_ra[0]-out.earth.TUMan[-1])
TUMan_hi = np.abs(TUMan_ra[1]-out.earth.TUMan[-1])
TCMB_lo = np.abs(TCMB_ra[0]-out.earth.TCMB[-1])
TCMB_hi = np.abs(TCMB_ra[1]-out.earth.TCMB[-1])
HflowUMan_lo = np.abs(HflowUMan_ra[0]-out.earth.HflowUMan[-1])
HflowUMan_hi = np.abs(HflowUMan_ra[1]-out.earth.HflowUMan[-1])
HflowCMB_lo = np.abs(HflowCMB_ra[0]-out.earth.HflowCMB[-1])
HflowCMB_hi = np.abs(HflowCMB_ra[1]-out.earth.HflowCMB[-1])
ViscUMan_lo = np.abs(ViscUMan_ra[0]-out.earth.ViscUMan[-1])
ViscUMan_hi = np.abs(ViscUMan_ra[1]-out.earth.ViscUMan[-1])
ViscLMan_lo = np.abs(ViscLMan_ra[0]-out.earth.ViscLMan[-1])
ViscLMan_hi = np.abs(ViscLMan_ra[1]-out.earth.ViscLMan[-1])
MeltMassFlux_lo = np.abs(MeltMassFlux_ra[0]-out.earth.MeltMassFluxMan[-1]*1e-6)
MeltMassFlux_hi = np.abs(MeltMassFlux_ra[1]-out.earth.MeltMassFluxMan[-1]*1e-6)
FMeltUMan_lo = np.abs(FMeltUMan_ra[0]-out.earth.FMeltUMan[-1])
FMeltUMan_hi = np.abs(FMeltUMan_ra[1]-out.earth.FMeltUMan[-1])

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
plt.errorbar(out.earth.Time[-1],out.earth.TUMan[-1],yerr=[[TUMan_lo],[TUMan_hi]],color=vplot.colors.orange,fmt='o')
plt.plot(out.earth.Time,out.earth.TLMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$T_{LM}$')
plt.plot(out.earth.Time,out.earth.TCMB,color=vplot.colors.purple,linestyle='-',label=r'$T_{CMB}$')
plt.errorbar(out.earth.Time[-1],out.earth.TCMB[-1],yerr=[[TCMB_lo],[TCMB_hi]],color=vplot.colors.purple,fmt='-o')
plt.plot(out.earth.Time,out.earth.TCore,'k-',label=r'$T_{C}$')
plt.legend(loc='best',ncol=2,frameon=True,columnspacing=1)
plt.ylabel('Temperature (K)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,10000)
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.HflowUMan,color=vplot.colors.red,linestyle='-',label=r'$Q_{UMan}$')
plt.errorbar(out.earth.Time[-1],out.earth.HflowUMan[-1],yerr=[[HflowUMan_lo],[HflowUMan_hi]],color=vplot.colors.red,fmt='o')
plt.plot(out.earth.Time,out.earth.HflowCMB,color=vplot.colors.orange,linestyle='-',label=r'$Q_{CMB}$')
plt.errorbar(out.earth.Time[-1],out.earth.HflowCMB[-1],yerr=[[HflowCMB_lo],[HflowCMB_hi]],color=vplot.colors.orange,fmt='o')
plt.plot(out.earth.Time,out.earth.RadPowerMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$Q_{Rad,Man}$')
plt.plot(out.earth.Time,out.earth.RadPowerCore,'k-',label=r'$Q_{Rad,Core}$')
plt.legend(loc='best',frameon=True)
plt.ylabel('Heat Flow (TW)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,150)
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.BLUMan,label=r'$\delta_{UM}$',color=vplot.colors.dark_blue)
plt.plot(out.earth.Time,out.earth.BLLMan,label=r'$\delta_{LM}$',color=vplot.colors.orange)
plt.legend(loc='best',frameon=True)
plt.ylabel(r'Boundary Layer Depths (km)')
plt.xlabel('Time (Gyr)')
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.earth.Time,out.earth.ViscUMan,label=r'$\nu_{UM}$',color=vplot.colors.dark_blue)
plt.errorbar(out.earth.Time[-1],out.earth.ViscUMan[-1],yerr=[[ViscUMan_lo],[ViscUMan_hi]],color=vplot.colors.dark_blue,fmt='o')
plt.semilogy(out.earth.Time,out.earth.ViscLMan,label=r'$\nu_{LM}$',color=vplot.colors.orange)
plt.errorbar(out.earth.Time[-1],out.earth.ViscLMan[-1],yerr=[[ViscLMan_lo],[ViscLMan_hi]],color=vplot.colors.orange,fmt='o')
plt.legend(loc='best',frameon=True)
plt.ylabel(r'Mantle Viscosity ($m^2s^{-1}$)')
plt.xlabel('Time (Gyr)')
plt.ylim(1e12,1e19)
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.FMeltUMan,color=vplot.colors.dark_blue)
plt.errorbar(out.earth.Time[-1],out.earth.FMeltUMan[-1]*1e-6,yerr=[[FMeltUMan_lo],[FMeltUMan_hi]],color=vplot.colors.dark_blue,fmt='o')
plt.ylabel(r'Melt Fraction Upper Mantle (n.d.)')
plt.xlabel('Time (Gyr)')
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MeltMassFluxMan*1e-6,color=vplot.colors.dark_blue)
plt.errorbar(out.earth.Time[-1],out.earth.MeltMassFluxMan[-1]*1e-6,yerr=[[MeltMassFlux_lo],[MeltMassFlux_hi]],color=vplot.colors.dark_blue,fmt='o')
plt.ylabel(r'Melt Mass Flux Mantle ($\times 10^6$ kg$/$s)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,100)
plt.xlim(0,4.6)
plt.xticks([0,1,2,3,4])

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('EarthInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('EarthInterior%d.png'%nfig)

# Core Plots
rows=2
nfig += 1
fig = plt.figure(nfig, figsize=(10,10))
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
plt.legend(loc='best',frameon=True)
plt.ylabel(r'Core Buoyancy Flux ($\times10^{-13}$ m$^2/$s$^3$)')
plt.xlabel('Time (Gyr)')
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MagMom,label='MagMom')
plt.ylim(0,2)
plt.ylabel('Magnetic Moment (E. Units)')
plt.xlabel('Time (Gyr)')
plt.xticks([0,1,2,3,4])
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.earth.Time,out.earth.MagPauseRad)
plt.ylabel(r'Magnetopause Radius (E. Units)')
plt.xlabel('Time (Gyr)')
plt.xticks([0,1,2,3,4])
#panel += 1
#plt.subplot(rows,cols,panel)
#plt.plot(out.earth.Time,out.earth.ChiOC,label='ChiOC')
#plt.plot(out.earth.Time,out.earth.ChiIC,label='ChiIC')
#plt.ylim(0,0.2)
#plt.ylabel(r'Core Light Element Concentration')
#plt.xlabel('Time (Gyr)')
#plt.legend(loc='best',frameon=False)

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('EarthInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('EarthInterior%d.png'%nfig)
plt.close()
