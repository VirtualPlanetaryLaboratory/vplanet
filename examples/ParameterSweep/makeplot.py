#!/usr/bin/env python

import bigplanet as bp
import h5py as h5
import matplotlib.pyplot as plt
import vplot as vpl
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

data = h5.File('ParameterSweep.hdf5', 'r')

RIC = bp.ExtractColumn(data,'earth_RIC_final')
RIC_units = bp.ExtractUnits(data,'earth_RIC_final')

TCore_uniq = bp.ExtractUniqueValues(data,'earth_TCore_initial')
TCore_units = bp.ExtractUnits(data,'earth_TCore_initial')

K40_uniq = bp.ExtractUniqueValues(data,'earth_40KPowerCore_final')
K40_units = bp.ExtractUnits(data,'earth_40KPowerCore_final')

RIC_Matrix = bp.CreateMatrix(TCore_uniq,K40_uniq,RIC)

#print(RIC_Matrix)

contours = [0,500,1000,1500,2000,2500]
xlabel = 'Initial Core Temperature ('+TCore_units+')'
ylabel = 'Current Potassium-40 Power ('+K40_units+')'
title = 'Final Inner Core Radius ('+RIC_units+')'

#plt.xlim(2500, 3500)
#plt.ylim(0, 0.5)

plt.xlabel(xlabel,fontsize=20)
plt.ylabel(ylabel,fontsize=20)
plt.title(title,fontsize=20)

plt.tick_params(axis='both', labelsize=20)

cont = plt.contour(TCore_uniq,K40_uniq,RIC_Matrix,colors = vpl.colors.pale_blue,levels=contours)
plt.clabel(cont,fmt="%.0f",fontsize=15)

cont = plt.contour(TCore_uniq,K40_uniq,RIC_Matrix,levels=[1221])
plt.clabel(cont,fmt="%.0f",fontsize=15)

#plt.show()
#plt.close()

plt.savefig('BigPlanetExample.png')
