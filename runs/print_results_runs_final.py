import os as os
import numpy as np

waterfile = open('water.txt')
water = waterfile.readlines()
waterfile.close()

planets = ['e','f','g']


num_dir = 2000

while (num_dir<2021):
    new_directory = 'TR1_'+str(num_dir)+''

    os.chdir(''+new_directory+'')
    os.system('cp ../TR1_'+planets[p]+'_example/print_results.py .')

    os.system('python print_results.py')

    os.chdir('..')


# for p in range(len(planets)):
#     for w in range(len(water)):
#         new_directory = 'TR1_'+planets[p]+'_'+water[w][:-1]+'TO'
#
#         os.chdir(''+new_directory+'')
#         # os.system('cp ../TR1_'+planets[p]+'_example/print_results.py .')
#
#         os.system('python print_results.py')
#
#         os.chdir('..')
