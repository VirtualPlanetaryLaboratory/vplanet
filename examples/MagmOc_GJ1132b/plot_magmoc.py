import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt

plt.close('all')

#########################################
#         Set style for plot            #
mpl.rcParams['lines.linewidth']  = 2
mpl.rcParams['axes.labelsize']   = 13
mpl.rcParams['xtick.labelsize']  = 12
mpl.rcParams['ytick.labelsize']  = 12
mpl.rcParams['legend.fontsize']  = 13
mpl.rcParams['axes.titleweight'] = 'bold'
#########################################

cmap=plt.get_cmap('nipy_spectral')

# read data
data = np.loadtxt("GJ1132.GJ1132b.forward")

time        = data[:,0]  # time (yr)
Tpot        = data[:,1]  # Potential temp magma ocean (K)
Tsurf       = data[:,2]  # Surface temp (K)
r_sol       = data[:,3]  # solidification radius (R_earth)
M_water_mo  = data[:,4]  # water mass in magma ocean + atmosphere (TO)
M_water_sol = data[:,5]  # water mass in solid mantle (kg)
M_O_mo      = data[:,6]  # mass of oxygen in magma ocean + atmosphere (kg)
M_O_sol     = data[:,7]  # mass of oxygen in solid mantle (kg)
Press_H2O   = data[:,8]  # partial pressure water in atmopshere (bar)
Press_O     = data[:,9]  # partial pressure oxygen in atmosphere (bar)
Frac_Fe2O3  = data[:,12] # partial pressure oxygen in atmosphere (bar)
NetFluxAtmo = data[:,13] # atmospheric net flux (W/m^2)
Frac_H2O    = data[:,14] # Water fraction in magma ocean
MeltFrac    = data[:,15] # Melt Fraction magma ocean

n_time = len(time)
i_end  = n_time-1

M_water_atm = np.zeros(n_time)
M_O_atm     = np.zeros(n_time)


REARTH = 6.3781e6                   # Earth radius [m]
MEARTH = 5.972186e24                # Earth mass [kg]
BIGG   = 6.67428e-11                # gravity constant [m**3/kg/s**2]
r_p    = 1.15*REARTH                # Radius planet [m]
m_p    = 1.62*MEARTH                # Mass planet [kg]
g      = (BIGG * m_p) / (r_p ** 2)  # surface gravity
rho_m  = 4000                       # mantle melt density
TO     = 1.39e21                    # mass of 1 Terr. Ocean [kg]

for i in range(n_time):
    M_water_atm[i] = Press_H2O[i]*1e5 * 4 * np.pi * r_p**2 / g
    M_O_atm[i]     = Press_O[i]*1e5 * 4 * np.pi * r_p**2 / g

print('###################')
print('Simulation results:')
print('Solidification Time           = ',time[n_time-1]*1e-6,  ' Myr')
print('Water mass locked in mantle   = ',M_water_sol[n_time-1], ' TO')
print('Oxygen mass locked in mantle  = ',M_O_sol[n_time-1],     ' kg')
print('Water pressure in atmosphere  = ',Press_H2O[n_time-1],  ' bar')
print('Oxygen pressure in atmosphere = ',Press_O[n_time-1],    ' bar')
print('Fe2O3 mass frac in mantle     = ',Frac_Fe2O3[n_time-1])
print('###################')

### Plot ###

fig = plt.figure(num=None, figsize=(10, 12), dpi=300, facecolor='w', edgecolor='k')
fig.suptitle('GJ1132b: Initial water content '+str(M_water_mo[0])+' TO + grey atmosphere', fontsize=16, fontweight='bold')

ax1 = fig.add_subplot(421)
ax1.plot(time*10**-6, Tpot,  label='$T_p$',      color=cmap(0))
ax1.plot(time*10**-6, Tsurf, label='$T_{surf}$', color=cmap(220), linestyle='--')
ax1.legend(loc='best', frameon=True)
ax1.set_ylabel('Temperature (K)')
ax1.set_xscale('log')
ax1.set_xlim([1e-3,time[i_end]*1e-6])

ax2 = fig.add_subplot(422, sharex=ax1)
ax2.plot(time*10**-6, r_sol/1.15, label='$r_s$', color=cmap(0))
ax2.set_ylim([0.5,1])
# ax2.legend(loc='best', frameon=True)
ax2.set_ylabel('Solidification radius ($r_p$)')

ax3 = fig.add_subplot(423, sharex=ax1)
ax3.plot(time*10**-6, M_water_atm/TO,            color=cmap(0),   label='atmosphere')
ax3.plot(time*10**-6, M_water_mo-M_water_atm/TO, color=cmap(220), label='magma ocean')
ax3.plot(time*10**-6, M_water_sol,               color=cmap(70),  label='solid')
ax3.set_ylim([0.01,100])
ax3.legend(loc='best', frameon=True)
ax3.set_ylabel('Water Mass (TO)')
ax3.set_yscale('log')

ax4 = fig.add_subplot(425, sharex=ax1)
ax4.plot(time*10**-6, Press_H2O, color=cmap(0),   label='$H_2O$')
ax4.plot(time*10**-6, Press_O,   color=cmap(220), label='$O_2$')
ax4.legend(loc='best', frameon=True)
ax4.set_ylabel('Atmospheric pressure (bar)')
ax4.set_yscale('log')

ax5 = fig.add_subplot(426, sharex=ax1)
ax5.plot(time*10**-6, Frac_H2O,   color=cmap(0),   label='$H_2O$')
ax5.plot(time*10**-6, Frac_Fe2O3, color=cmap(220), label='$Fe_2O_3$')
ax5.legend(loc='best', frameon=True)
ax5.set_ylabel('Mass frac in magma ocean')
# ax5.set_yscale('log')
# ax5.set_ylim([1e16,5e21])

ax6 = fig.add_subplot(424, sharex=ax1)
ax6.plot(time*10**-6, M_O_atm,        color=cmap(0),   label='atmosphere')
ax6.plot(time*10**-6, M_O_mo-M_O_atm, color=cmap(220), label='magma ocean')
ax6.plot(time*10**-6, M_O_sol,        color=cmap(70),  label='solid')
ax6.legend(loc='best', frameon=True)
ax6.set_ylabel('Oxygen Mass (kg)')
ax6.set_yscale('log')
# ax6.set_ylim([1e16,1e21])

ax7 = fig.add_subplot(427, sharex=ax1)
ax7.plot(time*10**-6, NetFluxAtmo, color=cmap(0))
ax7.set_ylabel('Atmospheric net flux ($W/m^2$)')
ax7.set_yscale('log')
ax7.set_xlabel('Time (Myrs)')

ax8 = fig.add_subplot(428, sharex=ax1)
ax8.plot(time*10**-6, MeltFrac, color=cmap(0))
# ax8.legend(loc='best', frameon=True)
ax8.set_ylabel('Melt Fraction Magma Ocean')
# ax8.set_yscale('log')
ax8.set_xlabel('Time (Myrs)')


plt.subplots_adjust(left=0.1, right=0.95, top=0.93, bottom=0.05, wspace=0.25)
plt.savefig('Results_GJ1132b.png')
