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

    # Check global properties
    assert np.isclose(output.log.final.earth.TotIceMass, 3.071389e+15)
    assert np.isclose(output.log.final.earth.TotIceBalance, 9.223785e-08)
    assert np.isclose(output.log.final.earth.AreaIceCov, 0.054040)
    assert np.isclose(output.log.final.earth.IceAccum, 0.458812)
    assert np.isclose(output.log.final.earth.IceAblate, -0.343222)
    assert np.isclose(output.log.final.earth.TempMaxLand, 337.829716)
    assert np.isclose(output.log.final.earth.TempMaxWater, 294.261368)
    assert np.isclose(output.log.final.earth.PeakInsol, 1069.614002)

    # Check Grid Output
    for f in subprocess.check_output('echo '+dir+'/*.DailyInsol.*',shell=True).split():
      f1 = re.split('\.',re.split('/',f.decode('ascii'))[-1])  #split apart output file

      if len(f1) == 4:
        timestamp = f1[3]
      elif len(f1) == 5:
        timestamp = f1[3]+'.'+f1[4]

      time0 = np.float(timestamp)

    sysname = f1[0]
    plname = f1[1]
    insolf = dir+'/'+sysname+'.'+plname+'.DailyInsol.'+timestamp
    tempf = dir+'/'+sysname+'.'+plname+'.SeasonalTemp.'+timestamp
    icef = dir+'/'+sysname+'.'+plname+'.SeasonalIceBalance.'+timestamp
    planckf = dir+'/'+sysname+'.'+plname+'.PlanckB.'+timestamp
    divf = dir+'/'+sysname+'.'+plname+'.SeasonalDivF.'+timestamp
    fmeridf = dir+'/'+sysname+'.'+plname+'.SeasonalFMerid.'+timestamp
    foutf = dir+'/'+sysname+'.'+plname+'.SeasonalFOut.'+timestamp

    insol = np.loadtxt(insolf,unpack=True)
    temp = np.loadtxt(tempf,unpack=True)
    ice = np.loadtxt(icef,unpack=True)
    planck = np.loadtxt(planckf,unpack=True)
    div = np.loadtxt(divf,unpack=True)
    fmerid = np.loadtxt(fmeridf,unpack=True)
    fout = np.loadtxt(fmeridf,unpack=True)

    # Check two different latitudes
    assert np.isclose(insol[1][0], 1055.352482)
    assert np.isclose(insol[1][75], 329.47451)
    assert np.isclose(temp[1][0], 22.591643)
    assert np.isclose(temp[1][75], 8.998626)
    assert np.isclose(ice[1][0], -0.0016)
    assert np.isclose(ice[1][75], -0.000306)
    assert np.isclose(planck[1][0], 2.09)
    assert np.isclose(planck[1][75], 2.09)
    assert np.isclose(div[1][0], 34.757649)
    assert np.isclose(div[1][75], 13.23595)
    assert np.isclose(fmerid[1][0], 804939200000000.0)
    assert np.isclose(fmerid[1][75], 309954000000000.0)
    assert np.isclose(fout[1][0], 804939200000000.0)
    assert np.isclose(fout[1][75], 309954000000000.0)

    #print(fmerid[1][0])
    #print(fmerid[1][75])

if __name__ == "__main__":
    test_IceBelts()
