#!/usr/bin/env python

import bigplanet as bp
import h5py as h5
import matplotlib.pyplot as plt
import vplot as vpl
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

HDF5_File = h5.File('ParamterSweep.hdf5', 'r')

bp.PrintKeys(HDF5_File)

TMan = bp.ExtractColumn(HDF5_File,'earth_TMan_initial')
Pman = bp.ExtractColumn(HDF5_File,'earth_40KPowerMan_initial')

SurfFLuxTot = bp.ExtractColumn(HDF5_File,'earth_SurfEnFluxTotal_final')

TMan_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_TMan_initial')
Pman_uniq = bp.ExtractUniqueValues(HDF5_File,'earth_40KPowerMan_initial')



SurfFLuxTot_Matrix = bp.CreateMatrix(TMan_uniq,Pman_uniq,SurfFLuxTot)

plt.contour(TMan_uniq,Pman_uniq,SurfFLuxTot_Matrix,colors = vpl.colors.pale_blue)

plt.show()
plt.close()

plt.savefig('ParameterSweepExample.png')
