from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.earth.TotIceMass": {"value": 3.071389e+15, "unit": u.kg},
        "log.final.earth.TotIceBalance": {"value": 9.223785e-08, "unit": u.kg},
        "log.final.earth.AreaIceCov": {"value":  0.054040},
        "log.final.earth.IceAccum": {"value": 0.458812},
        "log.final.earth.IceAblate": {"value": -0.343222},
        "log.final.earth.TempMaxLand": {"value": 337.829716,"unit": u.sec},
        "log.final.earth.TempMaxWater": {"value": 294.261368,"unit": u.sec},
        "log.final.earth.PeakInsol": {"value": 1069.614002,"unit": u.kg*u.m**2/u.sec**2/(u.m**2*u.sec)},
    }
)
class TestIceBelt(Benchmark):
    pass


    # Check global properties
    # assert np.isclose(output.log.final.earth.TotIceMass, 3.071389e+15)
    # assert np.isclose(output.log.final.earth.TotIceBalance, 9.223785e-08)
    # assert np.isclose(output.log.final.earth.AreaIceCov, 0.054040)
    # assert np.isclose(output.log.final.earth.IceAccum, 0.458812)
    # assert np.isclose(output.log.final.earth.IceAblate, -0.343222)
    # assert np.isclose(output.log.final.earth.TempMaxLand, 337.829716)
    # assert np.isclose(output.log.final.earth.TempMaxWater, 294.261368)
    # assert np.isclose(output.log.final.earth.PeakInsol, 1069.614002)

    # Check Grid Output
    # sysname='icebelt'
    # plname='earth'
    # timestamp='0'
    # insolf = dir+'/'+sysname+'.'+plname+'.DailyInsol.'+timestamp
    # tempf = dir+'/'+sysname+'.'+plname+'.SeasonalTemp.'+timestamp
    # icef = dir+'/'+sysname+'.'+plname+'.SeasonalIceBalance.'+timestamp
    # planckf = dir+'/'+sysname+'.'+plname+'.PlanckB.'+timestamp
    # divf = dir+'/'+sysname+'.'+plname+'.SeasonalDivF.'+timestamp
    # fmeridf = dir+'/'+sysname+'.'+plname+'.SeasonalFMerid.'+timestamp
    # foutf = dir+'/'+sysname+'.'+plname+'.SeasonalFOut.'+timestamp
    #
    # insol = np.loadtxt(insolf,unpack=True)
    # temp = np.loadtxt(tempf,unpack=True)
    # ice = np.loadtxt(icef,unpack=True)
    # planck = np.loadtxt(planckf,unpack=True)
    # div = np.loadtxt(divf,unpack=True)
    # fmerid = np.loadtxt(fmeridf,unpack=True)
    # fout = np.loadtxt(fmeridf,unpack=True)
    #
    # # Check two different latitudes
    # assert np.isclose(insol[1][0], 1055.352482)
    # assert np.isclose(insol[1][75], 329.47451)
    # assert np.isclose(temp[1][0], 22.591643)
    # assert np.isclose(temp[1][75], 8.998626)
    # assert np.isclose(ice[1][0], -0.0016)
    # assert np.isclose(ice[1][75], -0.000306)
    # assert np.isclose(planck[1][0], 2.09)
    # assert np.isclose(planck[1][75], 2.09)
    # assert np.isclose(div[1][0], 34.757649)
    # assert np.isclose(div[1][75], 13.23595)
    # assert np.isclose(fmerid[1][0], 804939200000000.0)
    # assert np.isclose(fmerid[1][75], 309954000000000.0)
    # assert np.isclose(fout[1][0], 804939200000000.0)
    # assert np.isclose(fout[1][75], 309954000000000.0)
