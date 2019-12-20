import os as os
import numpy as np

water = ['1','2','5','10','20','50','100']

num_dir = 2000

Results_Trappist1_file = open('Results_Trappist1_e_co2.txt','w')
Results_Trappist1_file.write('# Results for Trappist1-e with CO2 \n')
Results_Trappist1_file.write('# Water mass [TO] \t Solidification time [Myr] \t Desiccation time [Myr] \t Water locked in mantle [TO] \t Total water in system [TO] \t Water pressure [bar] \t Oxygen pressure [bar] \t CO2 pressure [bar] \n ')

for w in range(len(water)):
    # new_directory = 'TR1_'+planets[p]+'_'+water[w]+'TO'
    new_directory = 'CO2_TR1_e_'+str(water[w])+'TO'

    resultfile = open(''+new_directory+'/Results.dat')
    results = resultfile.readlines()
    resultfile.close()

    solid_time   = float(results[4])
    if (float(results[2])==1):
        water_locked = float(results[6])
        water_tot    = float(results[12])
        press_water  = float(results[14])
        press_oxy    = float(results[16])
        press_co2    = float(results[18])
        desicc_time  = solid_time
    elif (float(results[2])==2):
        water_locked = float(results[6])
        water_tot    = float(results[12])
        press_water  = float(results[14])
        press_oxy    = float(results[16])
        press_co2    = float(results[18])
        desicc_time  = solid_time
    else:
        water_locked = float(results[27])
        water_tot    = float(results[29])
        press_water  = float(results[31])
        press_oxy    = float(results[33])
        press_co2    = float(results[35])
        if (float(results[23])==1):
            desicc_time = float(results[25])
        else:
            desicc_time = 7e3


    if (press_water<0.006):
        press_water = 0

    Results_Trappist1_file.write(''+str(water[w])+'\t'+str(solid_time)+'\t'+str(desicc_time)+'\t'+str(water_locked)+'\t'+str(water_tot)+'\t'+str(press_water)+'\t'+str(press_oxy)+'\t'+str(press_co2)+'\n')

Results_Trappist1_file.close()
