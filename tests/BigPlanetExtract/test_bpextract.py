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

def test_bpextract():
    #gets the number of cores on the machine
    cores = str(mp.cpu_count())
    if cores == 1:
        warnings.warn("There is only 1 core on the machine",stacklevel=3)
    else:
        #removes checkpoint files
        cp = cwd+'/.BP_Extract'
        sub.run(['rm', cp],cwd=cwd)
        cp_hdf5 = cwd+'/.BP_Extract_hdf5'
        sub.run(['rm', cp_hdf5],cwd=cwd)
        #removes the folders from when vspace is ran
        dir = cwd+'/BP_Extract'
        sub.run(['rm', '-rf', dir],cwd=cwd)
        sub.run(['rm', '-rf', (dir + '.hdf5')],cwd=cwd)
        #runs vspace
        sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
        #runs multi-planet
        sub.run(['python', '../../multi-planet/multi-planet.py','vspace.in','-c',cores],cwd=cwd)
        #runs bigplanet
        sub.run(['python', '../../bigplanet/bigplanet/bigplanet.py','vspace.in','-c',cores],cwd=cwd)

        #reads in the hdf5 file
        file = h5py.File((dir + '.hdf5'),'r')

        earth_Instellation_final = bp.ExtractColumn(file,'earth_Instellation_final')
        sun_RotPer_initial = bp.ExtractColumn(file,'sun_RotPer_initial')

        for i in range(len(earth_Instellation_final)):
            assert np.isclose(earth_Instellation_final[0],1367.635318)
            assert np.isclose(earth_Instellation_final[1],341.90883)

        for j in range(len(sun_RotPer_initial)):
            assert np.isclose(sun_RotPer_initial[j],86400.0)





if __name__ == "__main__":
    test_bpextract()
