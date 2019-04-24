#################################################
#   Plot results for TRAPPIST-1 from VPLanet    #
# Modules used: MagmOc, AtmEsc, RadHeat, EqTide #
#################################################

import numpy as np
import matplotlib as mpl
import matplotlib.pyplot  as plt
import seaborn as sns
from time import time


# read data
for N_Planet in range(4,7):

    if N_Planet == 4:
        # TRAPPIST-1 e #
        data = np.loadtxt("Trappist1.e.forward")
        R_N_Planet = 0.913
        M_N_Planet = 0.766
        Name_Planet = 'Trappist-1 e'
        Name_Folder = 'Trappist-1_e'
    elif N_Planet == 5:
        # TRAPPIST-1 f #
        data = np.loadtxt("Trappist1.f.forward")
        R_N_Planet = 1.05
        M_N_Planet = 0.926
        Name_Planet = 'Trappist-1 f'
        Name_Folder = 'Trappist-1_f'
    elif N_Planet == 6:
        # TRAPPIST-1 g #
        data_g = np.loadtxt("Trappist1.g.forward")
        R_N_Planet = 1.15
        M_N_Planet = 1.14
        Name_Planet = 'Trappist-1 g'
        Name_Folder = 'Trappist-1_g'

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

    N_H_sol = np.zeros(n_time) # number of H atoms in solid mantle
    N_H_space = np.zeros(n_time) # number of H atoms in solid mantle
    N_H_mo  = np.zeros(n_time) # number of H atoms in liquid mantle
    N_H_atm = np.zeros(n_time) # number of H atoms in atmosphere
    N_O_sol = np.zeros(n_time) # number of O atoms in solid mantle
    N_O_mo  = np.zeros(n_time) # number of O atoms in liquid mantle
    N_O_atm = np.zeros(n_time) # number of O atoms in atmosphere
    N_O_space = np.zeros(n_time) # number of H atoms in solid mantle

    N_H_tot = np.zeros(n_time) # number of O atoms in atmosphere
    N_O_tot = np.zeros(n_time) # number of O atoms in atmosphere

    round = 1e45

    TO        = 1.39e21         # mass of 1 Terr. Ocean [kg]
    AVOGADROCONST = 6.022e23

    REARTH = 6.3781e6        # m
    MEARTH = 5.972186e24     # kg
    BIGG   = 6.67428e-11     # m**3/kg/s**2
    r_p    = R_N_Planet * REARTH
    m_p    = M_N_Planet * MEARTH
    g      = (BIGG * m_p) / (r_p ** 2)
    rho_m  = 4000

    diff_old = 1e9
    diff_old_2 = 1e9

    man_sol   = 0 # Mantle solidified?
    esc_stop  = 0 # Escape stopped? (Inner edge HZ)
    atm_des   = 0 # Atmosphere desiccated?
    quasi_sol = 0 # Atm desiccated & T_surf below 1000K but not solid?

    for i in range(n_time):

        M_water_atm[i] = Press_H2O[i] * 1e5 * 4 * np.pi * r_p**2 / g
        M_O_atm[i]     = Press_O[i]   * 1e5 * 4 * np.pi * r_p**2 / g

        if (man_sol == 0) and (r_sol[i] >= R_N_Planet):
            man_sol = 1
            t_solid = i

        if (man_sol == 0) and (atm_des == 1) and (Tsurf[i] < 1000):
            man_sol = 1
            t_solid = i

        if (esc_stop == 0) and (SemiMajor[i] >= HZInnerEdge[i]):
            esc_stop = 1
            t_habit  = i

        if (atm_des == 0) and (M_water_atm[i] <= 1e-5 * TO):
            atm_des  = 1
            t_desicc = i

    # N_H_space[i] = M_H_Space[i] * AVOGADROCONST / (0.001 * round)
    # N_H_sol[i] = 2 * M_water_sol[i]*TO * AVOGADROCONST / (0.018 * round)
    # N_H_mo[i]  = 2 * (M_water_mo[i]*TO - M_water_atm[i]) * AVOGADROCONST / (0.018 * round)
    # N_H_atm[i] = 2 * M_water_atm[i] * AVOGADROCONST / (0.018 * round)
    # N_H_tot[i] = N_H_sol[i] + N_H_mo[i] + N_H_atm[i] + N_H_space[i]
    #
    # N_O_space[i] = M_O_Space[i] * AVOGADROCONST / (0.016 * round)
    # N_O_sol[i] = M_water_sol[i]*TO * AVOGADROCONST / (0.018 * round) \
    #              + M_O_sol[i] * AVOGADROCONST / (0.016 * round)
    # N_O_mo[i]  = (M_water_mo[i]*TO - M_water_atm[i]) * AVOGADROCONST / (0.018 * round) \
    #              + (M_O_mo[i] - M_O_atm[i]) * AVOGADROCONST / (0.016 * round)
    # N_O_atm[i] = M_water_atm[i] * AVOGADROCONST / (0.018 * round) \
    #              + M_O_atm[i] * AVOGADROCONST / (0.016 * round)
    # N_O_tot[i] = N_O_sol[i] + N_O_mo[i] + N_O_atm[i] + N_O_space[i]

    results = open('Results_'+Name_Folder+'.dat','w')
    results.write('# -----------------------'+str(Name_Planet)+'----------------------- # \n')
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
