import os as os
import numpy as np

waterfile = open('foldernames.txt')
water = waterfile.readlines()
waterfile.close()

for i in range(len(water)):

    os.chdir(''+water+'')
    os.system('python plot_trappist.py')
    os.chdir('..')
