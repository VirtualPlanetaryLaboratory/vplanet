#!/usr/bin/env python

import bigplanet as bp
import h5py as h5
import matplotlib.pyplot as plt
import vplot as vpl
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

HDF5_File = h5.File('ParameterSweep.hdf5', 'r')

RIC = bp.ExtractColumn(HDF5_File,'earth_RIC_final')
RIC_units = bp.ExtractUnits(HDF5_File,'earth_RIC_final')

TMan_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')
TMan_units = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')

TCore_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TCore_initial')
TCore_units = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')

RIC_Matrix = bp.CreateMatrix(TMan_uniq,TCore_uniq,RIC)

contours = [1000,1100,1200,1300,1400]
xlabel = 'Initial Mantle Temperature ('+repr(TMan_units)+')'
ylabel = 'Initial Core Temperature ('+repr(TCore_units)+')'
title = 'Final Inner Core Radius ('+repr(RIC_units)+')'

plt.xlim(2500, 3500)
plt.ylim(5500, 6500)

plt.xlabel(xlabel,fontsize=20)
plt.ylabel(ylabel,fontsize=20)
plt.title(title,fontsize=20)

plt.tick_params(axis='both', labelsize=20)

plt.contour(TMan_uniq,TCore_uniq,RIC_Matrix,colors = vpl.colors.pale_blue,levels=contours)

plt.show()
plt.close()

plt.savefig('BigPlanetExample.png')
