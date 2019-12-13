from vplot import GetOutput
import subprocess
import numpy as np
import os
import re
cwd = os.path.dirname(os.path.realpath(__file__))


def test_IceBelts():
    """Test ice belt formation with POISE."""
    # Remove old log file
    subprocess.run(['rm', 'icebelt.log'], cwd=cwd)
    dir = 'SeasonalClimateFiles'
    subprocess.run(['rm','-r',dir])
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    #assert np.isclose(output.log.final.earth.TGlobal, 14.649773)
    #assert np.isclose(output.log.final.earth.Eccentricity, 0.0167)
    #assert np.isclose(output.log.final.earth.Obliquity, 0.409103)
    #assert np.isclose(output.log.final.earth.AlbedoGlobal, 0.344298)
    #assert np.isclose(output.log.final.earth.FluxOutGlobal, 233.918026)

    # Check Grid Output
    for f in subprocess.check_output('echo '+dir+'/*.DailyInsol.*',shell=True).split():
      f1 = re.split('\.',re.split('/',f.decode('ascii'))[-1])  #split apart output file

      print(f1)

      if len(f1) == 4:
        timestamp = f1[3]
      elif len(f1) == 5:
        timestamp = f1[3]+'.'+f1[4]

      time0 = np.float(timestamp)
      print(time0)

    sysname = f1[0]
    plname = f1[1]
    insolf = dir+'/'+sysname+'.'+plname+'.DailyInsol.'+timestamp
    tempf = dir+'/'+sysname+'.'+plname+'.SeasonalTemp.'+timestamp
    icef = dir+'/'+sysname+'.'+plname+'.SeasonalIceBalance.'+timestamp

    insol = np.loadtxt(insolf,unpack=True)
    temp = np.loadtxt(tempf,unpack=True)
    ice = np.loadtxt(icef,unpack=True)

    print(insol)

if __name__ == "__main__":
    test_IceBelts()
