import numpy as np
import matplotlib.pyplot  as plt
import seaborn as sns

sns.set_style("whitegrid")
plt.close('all')

cmap=plt.get_cmap('nipy_spectral')
A = 0
# 1: plot individual graphs, 0: plot all graphs together
individual = 0
# 1: plot time axis log scale, 0: plot time axis linear
log_plot = 1
Initial_water = 1
# read data
data = np.loadtxt("solarsystem.venus.forward")
TO        = 1.39e21         # mass of 1 Terr. Ocean [kg]

time        = data[:,0]  # time (yr)
M_water_atm = data[:,1] * TO  # water mass in atmopshere (TO)
RG_limit    = data[:,2]  # ??
M_O_mantle  = data[:,3]  # oxygen mass in mantle (kg)
P_O_atm     = data[:,4]  # oxygen pressure in atmosphere (bar)
N_H_Space   = data[:,5] *1e-3 # number of H atoms in space
N_O_Space   = data[:,6] *1e-3 # number of H atoms in space

n_time = len(time)
i_end  = n_time-1

N_H_atm = np.zeros(n_time) # number of H atoms in atmosphere
N_O_atm = np.zeros(n_time) # number of O atoms in atmosphere

N_H_tot = np.zeros(n_time) # number of O atoms in atmosphere
N_O_tot = np.zeros(n_time) # number of O atoms in atmosphere

Press_H2O = np.zeros(n_time)
M_O_atm = np.zeros(n_time)

round = 1e45

ATOMMASS  = 1.660538921e-27 # kg

REARTH = 6.3781e6        # m
MEARTH = 5.972186e24     # kg
BIGG   = 6.67428e-11     # m**3/kg/s**2
r_p    = 0.9499*REARTH
m_p    = 0.815*MEARTH
g      = (BIGG * m_p) / (r_p ** 2)

for i in range(n_time):
    Press_H2O[i] = M_water_atm[i] * g *1e-5 / (4 * np.pi * r_p**2)
    M_O_atm[i] = P_O_atm[i]*1e5 * 4 * np.pi * r_p**2 / g

    N_H_atm[i] = 2 * M_water_atm[i] *1e-3/ (0.018 * ATOMMASS * round)
    N_H_tot[i] = N_H_atm[i] + N_H_Space[i]

    N_O_atm[i] = M_water_atm[i] *1e-3/ (0.018 * ATOMMASS * round) \
                 + M_O_atm[i] *1e-3/ (0.016 * ATOMMASS * round)
    N_O_tot[i] = N_O_atm[i] + N_O_Space[i]

print('H: ',N_H_tot[0],N_H_tot[i],N_H_atm[i],N_H_Space[i])
print('O: ',N_O_tot[0],N_O_tot[i],N_O_atm[i],N_O_Space[i])

### Plot ###

fig = plt.figure()
fig.suptitle('Initial water content 100 terrestrial oceans', fontsize=16, fontweight='bold')

ax4 = fig.add_subplot(221)
ax4.plot(time*10**-6, Press_H2O, label='partial pressure H2O')
ax4.plot(time*10**-6, P_O_atm, label='partial pressure O')
ax4.legend(loc='best', frameon=True)
ax4.set_ylabel('Atmospheric pressure (bar)')
ax4.set_yscale('log')
ax4.set_xscale('log')


ax7 = fig.add_subplot(222, sharex=ax4)
ax7.plot(time*10**-6, N_H_Space, label='space')
ax7.plot(time*10**-6, N_H_atm, label='atmosphere')
ax7.plot(time*10**-6, N_H_tot, label='total', linestyle=':')
ax7.legend(loc='best', frameon=True)
# ax7.set_xlabel('Time (Myrs)')
ax7.set_ylabel('Number of H atoms ($10^{45}$)')
ax7.set_yscale('log')

ax8 = fig.add_subplot(223, sharex=ax4)
ax8.plot(time*10**-6, N_O_Space, label='space')
ax8.plot(time*10**-6, N_O_atm, label='atmosphere')
ax8.plot(time*10**-6, N_O_tot, label='total', linestyle=':')
ax8.legend(loc='best', frameon=True)
ax8.set_xlabel('Time (Myrs)')
ax8.set_ylabel('Number of O atoms ($10^{45}$)')
ax8.set_yscale('log')

ax9 = fig.add_subplot(224, sharex=ax4)
ax9.plot(time*10**-6, N_H_tot/N_H_tot[0]-1, label='H')
ax9.plot(time*10**-6, N_O_tot/N_O_tot[0]-1, label='O', linestyle='--')
ax9.plot(time*10**-6, (N_O_tot/N_O_tot[0]-1)-0.5*(N_H_tot/N_H_tot[0]-1), label='O-H/2', linestyle=':')
ax9.legend(loc='best', frameon=True)
ax9.set_xlabel('Time (Myrs)')
ax9.set_ylabel('Ratio of atoms gained')
# ax8.set_yscale('log')
#
# ax9 = fig.add_subplot(339, sharex=ax1)
# ax9.plot(time*10**-6, N_H_tot-N_H_tot[0], label='H')
# ax9.plot(time*10**-6, N_O_tot-N_O_tot[0], label='O')
# ax9.plot(time*10**-6, (N_O_tot-N_O_tot[0])-(N_H_tot-N_H_tot[0])/2., label='O-H/2', linestyle='--')
# ax9.legend(loc='best', frameon=True)
# ax9.set_xlabel('Time (Myrs)')
# ax9.set_ylabel('Number of atoms lost ($10^{45}$)')

plt.show()


# r_core = np.zeros(n_time)+r_c
#
# if individual == 1:
#     ## plot individual figures
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0)
#     # plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0)
#     plt.plot(time*10**-6, Tpot, label='T_p', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Tsurf, label='T_surf', linestyle='--', linewidth=3.0, color=cmap(0))
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     # ax1.set_ylim([0,4100])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Temperature (K)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, r_sol/r_p, label='$r_s$', color=cmap(100), linewidth=3.0)
#     # plt.plot(time*10**-6, r_core/r_p, label='$r_c$', color=cmap(220), linestyle='--', linewidth=3.0)
#     plt.plot(time*10**-6, r_sol, label='$r_s$', color=cmap(0), linewidth=3.0)
#     # plt.plot(time*10**-6, r_core, label='$r_c$', color=cmap(0), linestyle='--', linewidth=3.0)
#     plt.ylim([0.5,1])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Solidification radius ($r_p$)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, q_m, label='Mantle heat flux', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_OLR, label='Outgoing longwave radiation', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_BOL, label='Absorbed stellar radiation', linewidth=3.0)
#     # plt.plot(time*10**-6, Flux_XUV, label='Absorbed stellar XUV flux', linewidth=3.0)
#     plt.plot(time*10**-6, q_m, label='Mantle heat flux', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, OLR, label='Net Flux Atmosphere', linewidth=3.0, linestyle='--', color=cmap(0))
#     # plt.plot(time*10**-6, ASR, label='Absorbed stellar radiation', linewidth=3.0, linestyle='-.', color=cmap(0))
#     plt.plot(time*10**-6, XUV, label='Absorbed stellar XUV flux', linewidth=3.0, linestyle=':', color=cmap(0))
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Flux ($W/m^2$)', fontsize=18, fontweight='bold')
#     plt.yscale('log')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     # plt.plot(time*10**-6, M_water_mo/M_water_mo[0], label='magma ocean + atm', linewidth=3.0)
#     # plt.plot(time*10**-6, M_water_atm/M_water_mo[0], label='atmosphere', linewidth=3.0)
#     # plt.plot(time*10**-6, M_water_sol/M_water_mo[0], label='solid', linewidth=3.0)
#     plt.plot(time*10**-6, M_water_mo/M_water_mo[0], label='magma ocean + atm', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, M_water_atm/M_water_mo[0], label='atmosphere', linewidth=3.0, linestyle='--', color=cmap(0))
#     plt.plot(time*10**-6, M_water_sol/M_water_mo[0], label='solid', linewidth=3.0, linestyle=':', color=cmap(0))
#     plt.ylim([1e-3,1.05])
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Water Mass Fraction', fontsize=18, fontweight='bold')
#     plt.yscale('log')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.tick_params(labelsize=14)
#     plt.show()
#
#     plt.figure()
#     plt.title('GJ1132b: $M_{water}^{ini} = $'+str(Initial_water)+' TO', fontsize=18, fontweight='bold')
#     plt.plot(time*10**-6, Press_atm, label='total pressure', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Press_H2O, label='partial pressure H2O', linestyle='--', linewidth=3.0, color=cmap(0))
#     plt.plot(time*10**-6, Press_O, label='partial pressure O', linestyle=':', linewidth=3.0, color=cmap(0))
#     plt.legend(loc='best', frameon=True, fontsize=16)
#     plt.xlabel('Time (Myrs)', fontsize=18, fontweight='bold')
#     plt.ylabel('Atmospheric pressure (bar)', fontsize=18, fontweight='bold')
#     if log_plot == 1:
#         plt.xscale('log')
#     plt.xlim([1e-6,time[i_end+1]*1e-6])
#     plt.yscale('log')
#     plt.tick_params(labelsize=14)
#     plt.show()
#
# else:
#     ## plot multiple figures
