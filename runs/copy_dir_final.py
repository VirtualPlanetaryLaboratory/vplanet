import os as os
import numpy as np

waterfile = open('water.txt')
water = waterfile.readlines()
waterfile.close()

planets = ['e','f','g']
masses  = [0.766,0.926,1.14]

dir_number = 2000
# File with numbers and input parameters
dir_names_file = open('dir_names_heat.txt', 'w')
dir_names_file.write('# Number \t Planet \t Water mass [TO] \n ')

for p in range(len(planets)):
    for w in range(len(water)):
        # new_directory = 'TR1_'+planets[p]+'_'+water[w][:-1]+'TO_heat'
        new_directory = 'TR1_'+str(dir_number)+''
        os.system('cp -r TR1_'+planets[p]+'_example '+new_directory+'')

        ## Change input files
        inputfile = open(''+new_directory+'/'+planets[p]+'.in')
        input = inputfile.readlines()
        inputfile.close()

        input[12] = 'dWaterMassAtm		-'+water[w][:-1]+'		  # Initial water mass in the system (neg: terrestrial oceans, default: -1) \n'
        input[18] = 'bHaltEnterHabZone	0		  # Halt when planet enters habitable zone? (default: 0 = no, 1 = yes) \n'
        input[40] = 'd40KNumMan    -'+str(1000*masses[p])+'\n'
        input[60] = 'dTidalQ         10          # Tidal phase lag \n'
        input[69] = 'dEcc          0.1                         # Eccentricity \n'


        inputfile = open(''+new_directory+'/'+planets[p]+'.in','w')
        for l in range(len(input)):
            inputfile.write(input[l])
        inputfile.close()

        # write input parameters to list of folder numbers & update folder number
        dir_names_file.write(''+str(dir_number)+'\t'+planets[p]+'\t'+water[w][:-1]+'\n')
        dir_number = dir_number + 1

dir_names_file.close()
