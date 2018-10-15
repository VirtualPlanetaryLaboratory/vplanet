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
    sys.argv.append('png')  #default
#    print('ERROR: Incorrect number of arguments.')
#    print('Usage: '+sys.argv[0]+' <pdf | png>')
#    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png' and sys.argv[1] != '-n'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

out = vplot.GetOutput()
# Print final state
print('Final: t=%.3f TUMan=%.1f TMan=%.1f TCMB=%.1f HflowUMan=%.1f HflowMeltMan=%.1f HflowCMB=%.1f RadPowerMan=%.1f RadPowerCore=%.1f FMeltUMan=%.2f MeltMassFlux=%.1e ViscUMan=%.1e ViscLMan=%.1e MagMom=%.1f RIC=%.1f'%(out.venus.Time[-1],out.venus.TUMan[-1],out.venus.TMan[-1],out.venus.TCMB[-1],out.venus.HflowUMan[-1],out.venus.HflowMeltMan[-1],out.venus.HflowCMB[-1],out.venus.RadPowerMan[-1],out.venus.RadPowerCore[-1],out.venus.FMeltUMan[-1],out.venus.MeltMassFluxMan[-1],out.venus.ViscUMan[-1],out.venus.ViscLMan[-1],out.venus.MagMom[-1],out.venus.RIC[-1]))

if sys.argv[1] == '-n': exit(1)
    
# Plots
rows=3
cols=2
# Mantle Figure
nfig=1
fig = plt.figure(nfig, figsize=(10,15))
panel=1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.TMan,color=vplot.colors.red,linestyle='-',label=r'$T_{M}$')
plt.plot(out.venus.Time,out.venus.TUMan,color=vplot.colors.orange,linestyle='-',label=r'$T_{UM}$')
plt.plot(out.venus.Time,out.venus.TLMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$T_{LM}$')
plt.plot(out.venus.Time,out.venus.TCMB,color=vplot.colors.purple,linestyle='-',label=r'$T_{CMB}$')
plt.plot(out.venus.Time,out.venus.TCore,'k-',label=r'$T_{C}$')
plt.legend(loc='best',ncol=2,frameon=False,columnspacing=1)
plt.ylabel('Temperature (K)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,10000)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.HflowUMan,color='black',linestyle='-',label=r'$Q_{UMan}$')
plt.plot(out.venus.Time,out.venus.HflowMeltMan,color=vplot.colors.red,linestyle='-',label=r'$Q_{MeltMan}$')
plt.plot(out.venus.Time,out.venus.HflowCMB,color=vplot.colors.orange,linestyle='-',label=r'$Q_{CMB}$')
plt.plot(out.venus.Time,out.venus.RadPowerMan,color=vplot.colors.pale_blue,linestyle='-',label=r'$Q_{Rad,Man}$')
plt.plot(out.venus.Time,out.venus.RadPowerCore,'k-',label=r'$Q_{Rad,Core}$')
plt.legend(loc='best',frameon=False)
plt.ylabel('Heat Flow (TW)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,150)
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.BLUMan,label=r'$\delta_{UM}$')
plt.plot(out.venus.Time,out.venus.BLLMan,label=r'$\delta_{LM}$')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Boundary Layer Depths (km)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.venus.Time,out.venus.ViscUMan,label=r'$\nu_{UM}$')
plt.semilogy(out.venus.Time,out.venus.ViscLMan,label=r'$\nu_{LM}$')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Mantle Viscosity ($m^2s^{-1}$)')
plt.xlabel('Time (Gyr)')
plt.ylim(np.min([out.venus.ViscUMan,out.venus.ViscLMan]),np.max([out.venus.ViscUMan,out.venus.ViscLMan]))
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.FMeltUMan)
plt.ylabel(r'Melt Fraction Upper Mantle (n.d.)')
plt.xlabel('Time (Gyr)')
plt.ylim(0,1)
panel += 1
plt.subplot(rows,cols,panel)
plt.semilogy(out.venus.Time,out.venus.MeltMassFluxMan)
plt.ylabel(r'Melt Mass Flux Mantle (kg$/$s)')
plt.xlabel('Time (Gyr)')
#plt.ylim(1e4,1e9)


vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('VenusInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('VenusInterior%d.png'%nfig)

# Core Plots
nfig += 1
fig = plt.figure(nfig, figsize=(10,15))
panel = 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.RIC,label='RIC')
plt.ylim(0,1500)
plt.ylabel(r'Inner Core Radius (km)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.CoreBuoyTherm*1e13,label='Thermal')
plt.plot(out.venus.Time,out.venus.CoreBuoyCompo*1e13,label='Compositional')
plt.plot(out.venus.Time,out.venus.CoreBuoyTotal*1e13,label='Total')
plt.legend(loc='best',frameon=False)
plt.ylabel(r'Core Buoyancy Flux ($\times10^{-13}$ m$^2/$s$^3$)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.MagMom,label='MagMom')
plt.ylim(0,2)
plt.xlim(0,out.venus.Time[-1])
plt.ylabel('Magnetic Moment (E. Units)')
plt.xlabel('Time (Gyr)')
panel += 1
plt.subplot(rows,cols,panel)
plt.plot(out.venus.Time,out.venus.MagPauseRad)
plt.ylabel(r'Magnetopause Radius (E. Units)')
plt.xlabel('Time (Gyr)')
plt.xlim(0,out.venus.Time[-1])
plt.ylim(0,2.0)

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('VenusInterior%d.pdf'%nfig)
if (sys.argv[1] == 'png'):
    plt.savefig('VenusInterior%d.png'%nfig)
plt.close()
