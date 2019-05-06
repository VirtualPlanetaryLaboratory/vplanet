#################################################
#   Plot results for TRAPPIST-1 from VPLanet    #
# Modules used: MagmOc, AtmEsc, RadHeat, EqTide #
#################################################

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
import seaborn as sns
import os as os
from time import time

# TRAPPIST-1 g #
# read data
data_g = np.loadtxt("Trappist1.g.forward")
R_N_Planet = 1.15
M_N_Planet = 1.14
Name_Planet = 'Trappist-1 g'
Name_Folder = 'Trappist-1_g'

# write data to arrays
time        = data[:,0]  # time (yr)
Tpot        = data[:,1]  # Potential temp magma ocean (K)
Tsurf       = data[:,2]  # Surface temp (K)
r_sol       = data[:,3]  # solidification radius (R_earth)
M_water_mo  = data[:,4] # water mass in magma ocean + atmosphere (TO)
M_water_sol = data[:,5] # water mass in solid mantle (kg)
M_O_mo      = data[:,6] # mass of oxygen in magma ocean + atmosphere (kg)
M_O_sol     = data[:,7] # mass of oxygen in solid mantle (kg)
Press_H2O   = data[:,8] # partial pressure water in atmopshere (bar)
Press_O     = data[:,9] # partial pressure oxygen in atmosphere (bar)
M_H_Space   = data[:,10] # partial pressure oxygen in atmosphere (bar)
M_O_Space   = data[:,11] # partial pressure oxygen in atmosphere (bar)
Frac_Fe2O3  = data[:,12] # partial pressure oxygen in atmosphere (bar)
NetFluxAtmo = data[:,13] # atmospheric net flux (W/m^2)
Frac_H2O    = data[:,14] # Water fraction in magma ocean
RadioHeat   = data[:,15] # Radiogenic Heating Power (TW)
TidalHeat   = data[:,16] # Tidal Heating Power (TW)
SemiMajor   = data[:,17] # Semi Major Axis (AU)
HZInnerEdge = data[:,18] # Inner Edge of the HZ (AU)

n_time = len(time)
i_end  = n_time-1

M_water_atm = np.zeros(n_time)
M_O_atm     = np.zeros(n_time)

TO        = 1.39e21      # mass of 1 Terr. Ocean [kg]

REARTH = 6.3781e6        # m
MEARTH = 5.972186e24     # kg
BIGG   = 6.67428e-11     # m**3/kg/s**2
r_p    = R_N_Planet * REARTH
m_p    = M_N_Planet * MEARTH
g      = (BIGG * m_p) / (r_p ** 2)

man_sol   = 0 # Mantle solidified?
esc_stop  = 0 # Escape stopped? (Inner edge HZ)
atm_des   = 0 # Atmosphere desiccated?
quasi_sol = 0 # Atm desiccated & T_surf below 1000K but not solid?

# find time of solidification, desiccation, and/or entry of habitable zone
for i in range(n_time):

    M_water_atm[i] = Press_H2O[i] * 1e5 * 4 * np.pi * r_p**2 / g
    M_O_atm[i]     = Press_O[i]   * 1e5 * 4 * np.pi * r_p**2 / g

    if (atm_des == 0) and (Press_H2O[i] <= 1e-2):
        atm_des  = 1
        t_desicc = i

    if (man_sol == 0) and (r_sol[i] >= R_N_Planet):
        man_sol = 1
        t_solid = i

    if (esc_stop == 0) and (SemiMajor[i] >= HZInnerEdge[i]):
        esc_stop = 1
        t_habit  = i

# write results to file
results = open('Results.dat','w')
results.write('# -----------------------'+str(Name_Planet)+'----------------------- # \n')

if (atm_des == 1) and (man_sol == 0):
    results.write('# Desiccated & Solidified at same time?\n')
    results.write(str(1)+'\n')
    results.write('# Desiccation & Solidification Time [Myr]\n')
    results.write(str(time[t_desicc]*1e-6)+'\n')
    results.write('# Water mass locked in mantle [TO] \n')
    results.write(str(M_water_sol[t_desicc])+'\n')
    results.write('# Oxygen mass locked in mantle [kg] \n')
    results.write(str(M_O_sol[t_desicc])+'\n')
    results.write('# Total Water mass left in system [TO] \n')
    results.write(str(M_water_sol[t_desicc]+M_water_atm[t_desicc]/TO)+'\n')
    results.write('# Water pressure in atmosphere [bar] \n')
    results.write(str(Press_H2O[t_desicc])+'\n')
    results.write('# Oxygen pressure in atmosphere [bar] \n')
    results.write(str(Press_O[t_desicc])+'\n')
    results.write('# Fe2O3 mass frac in mantle \n')
    results.write(str(Frac_Fe2O3[t_desicc])+'\n')
else:
    results.write('# Desiccated & Solidified at same time?\n')
    results.write(str(0)+'\n')
    results.write('# Solidification Time [Myr]\n')
    results.write(str(time[t_solid]*1e-6)+'\n')
    results.write('# Water mass locked in mantle [TO] \n')
    results.write(str(M_water_sol[t_solid])+'\n')
    results.write('# Oxygen mass locked in mantle [kg] \n')
    results.write(str(M_O_sol[t_solid])+'\n')
    results.write('# Total Water mass left in system [TO] \n')
    results.write(str(M_water_sol[t_solid]+M_water_atm[t_solid]/TO)+'\n')
    results.write('# Water pressure in atmosphere [bar] \n')
    results.write(str(Press_H2O[t_solid])+'\n')
    results.write('# Oxygen pressure in atmosphere [bar] \n')
    results.write(str(Press_O[t_solid])+'\n')
    results.write('# Fe2O3 mass frac in mantle \n')
    results.write(str(Frac_Fe2O3[t_solid])+'\n')

    results.write('# ------------------------------------------------------------------ # \n')
    if (atm_des==1):
        results.write('# Atmosphere Desiccated? \n')
        results.write(str(1)+'\n')
        results.write('# Desiccation Time [Myr] \n')
        results.write(str(time[t_desicc]*1e-6)+'\n')
        results.write('# Total Water mass left in system [TO] \n')
        results.write(str(M_water_sol[t_desicc]+M_water_atm[t_desicc]/TO)+'\n')
        results.write('# Water pressure in atmosphere [bar] \n')
        results.write(str(Press_H2O[t_desicc])+'\n')
        results.write('# Oxygen pressure in atmosphere [bar] \n')
        results.write(str(Press_O[t_desicc])+'\n')
    elif (esc_stop==1):
        results.write('# Atmosphere Desiccated? \n')
        results.write(str(0)+'\n')
        results.write('# Time of Habitable Zone Entry [Myr] \n')
        results.write(str(time[t_habit]*1e-6)+'\n')
        results.write('# Total Water mass left in system [TO] \n')
        results.write(str(M_water_sol[t_habit]+M_water_atm[t_habit]/TO)+'\n')
        results.write('# Water pressure in atmosphere [bar] \n')
        results.write(str(Press_H2O[t_habit])+'\n')
        results.write('# Oxygen pressure in atmosphere [bar] \n')
        results.write(str(Press_O[t_habit])+'/n')

    results.write('# ------------------------------------------------------------------- # \n')
results.close()
