from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))
import warnings
import h5py
import multiprocessing as mp
import sys
import bigplanet as bp

def test_bpstats():
    #gets the number of cores on the machine
    cores = mp.cpu_count()
    if cores == 1:
        warnings.warn("There is only 1 core on the machine",stacklevel=3)
    else:
        #removes checkpoint files
        cp = cwd+'/.BP_Stats'
        sub.run(['rm', cp],cwd=cwd)
        cp_hdf5 = cwd+'/.BP_Stats_hdf5'
        sub.run(['rm', cp_hdf5],cwd=cwd)
        #removes the folders from when vspace is ran
        dir = cwd+'/BP_Stats'
        sub.run(['rm', '-rf', dir],cwd=cwd)
        sub.run(['rm', '-rf', (dir + '.hdf5')],cwd=cwd)
        #runs vspace
        sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
        #runs multi-planet
        sub.run(['python', '../../multi-planet/multi-planet.py','vspace.in'],cwd=cwd)
        #runs bigplanet
        sub.run(['python', '../../bigplanet/bigplanet/bigplanet.py','vspace.in'],cwd=cwd)

        #reads in the hdf5 file
        file = h5py.File((dir + '.hdf5'),'r')

        earth_RIC_min = bp.ExtractColumn(file,'earth_RIC_min')
        earth_235UNumMan_max = bp.ExtractColumn(file,'earth_235UNumMan_max')
        print(earth_235UNumMan_max)
        earth_TCMB_mean = bp.ExtractColumn(file,'earth_TCMB_mean')
        earth_FMeltUMan_geomean = bp.ExtractColumn(file,'earth_FMeltUMan_geomean')
        earth_BLUMan_stddev = bp.ExtractColumn(file,'earth_BLUMan_stddev')

        for i in range(len(earth_RIC_min)):
            assert np.isclose(earth_RIC_min[i],0)

        for j in range(len(earth_235UNumMan_max)):
             assert np.isclose(earth_235UNumMan_max[j],2.700598e+28)

        for k in range(len(earth_TCMB_mean)):
            assert np.isclose(earth_TCMB_mean[k],4359.67230935)

        for l in range(len(earth_FMeltUMan_geomean)):
            assert np.isclose(earth_FMeltUMan_geomean[l],0.20819565)

        for m in range(len(earth_BLUMan_stddev)):
            assert np.isclose(earth_BLUMan_stddev[m],18.26509002)


if __name__ == "__main__":
    sub.run(['python -m', '../../bigplanet/bigplanet/bigplanet.py','vspace.in','-c',cores],cwd=cwd)
    test_bpstats()
