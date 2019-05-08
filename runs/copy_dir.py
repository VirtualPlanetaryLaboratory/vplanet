import os as os
import numpy as np

waterfile = open('water.txt')
water = waterfile.readlines()
waterfile.close()

eccfile = open('ecc2.txt')
ecc = eccfile.readlines()
eccfile.close()

radfile = open('rad.txt')
rad = radfile.readlines()
radfile.close()

planets = ['e','f','g']
masses  = [0.766,0.926,1.14]

# 1 - 324: ecc locked, tidally locked
dir_number = 325

# File with numbers and input parameters
dir_names_file = open('dir_names2.txt', 'w')
dir_names_file.write('# Number \t Planet \t Water mass [TO] \t Eccentricity \t 40K abundance [Earth] \t bFixOrbit \n ')

for p in range(len(planets)):
    for w in range(len(water)):
        for e in range(len(ecc)):
            for r in range(len(rad)):
                new_directory = 'TR1_'+planets[p]+'_'+water[w][:-1]+'TO_ecc_'+ecc[e][:-1]+'_rad_'+rad[r][:-1]+''
                new_directory = 'TR1_'+str(dir_number)+''
                os.system('cp -r TR1_'+planets[p]+'_example '+new_directory+'')

                ## Change input files
                inputfile = open(''+new_directory+'/'+planets[p]+'.in')
                input = inputfile.readlines()
                inputfile.close()

                rad_p = float(rad[r][:-1])*masses[p]

                input[11] = 'dWaterMassAtm		-'+water[w][:-1]+'		  # Initial water mass in the system (neg: terrestrial oceans, default: -1) \n'
                input[38] = 'd40KNumMan    -'+str(rad_p)+'\n'
                input[62] = 'bFixOrbit 	0		   # Keep dEcc constant? \n'
                input[67] = 'dEcc          '+ecc[e][:-1]+'                         # Eccentricity \n'

                inputfile = open(''+new_directory+'/'+planets[p]+'.in','w')
                for l in range(len(input)):
                    inputfile.write(input[l])
                inputfile.close()

                ## Change time stepping for 1TO
                if (float(water[w][:-1]) < 10):
                    vplfile = open(''+new_directory+'/vpl.in')
                    vpl = vplfile.readlines()
                    vplfile.close()

                    vpl[21] = 'dEta          0.1                           # Coefficient for variable timestepping \n'

                    vplfile = open(''+new_directory+'/vpl.in','w')
                    for v in range(len(vpl)):
                        vplfile.write(vpl[v])
                    vplfile.close()

                # write input parameters to list of folder numbers & update folder number
                dir_names_file.write(''+str(dir_number)+'\t'+planets[p]+'\t'+water[w][:-1]+'\t'+ecc[e][:-1]+'\t'+rad[r][:-1]+'\t 0 \n')
                dir_number = dir_number + 1

dir_names_file.close()
