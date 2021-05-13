from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest
import pathlib
import numpy as np


@benchmark(
    {
        "log.final.earth.TotIceMass": {"value": 3.071389e15, "unit": u.kg},
        "log.final.earth.TotIceBalance": {"value": 9.223785e-08, "unit": u.kg},
        "log.final.earth.AreaIceCov": {"value": 0.054040},
        "log.final.earth.IceAccum": {"value": 0.458812},
        "log.final.earth.IceAblate": {"value": -0.343222},
        "log.final.earth.TempMaxLand": {"value": 337.829716, "unit": u.sec},
        "log.final.earth.TempMaxWater": {"value": 294.261368, "unit": u.sec},
        "log.final.earth.PeakInsol": {
            "value": 1069.614002,
            "unit": u.kg * u.m ** 2 / u.sec ** 2 / (u.m ** 2 * u.sec),
        },
    }
)
class TestIceBelt(Benchmark):
    pass


def test_grid_output(vplanet_output):

    # Get the Seasonal Climate Files
    path = pathlib.Path(__file__).parents[0].absolute()
    sysname = "icebelt"
    plname = "earth"
    timestamp = "0"
    insolf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.DailyInsol.{timestamp}"
    )
    tempf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalTemp.{timestamp}"
    )
    icef = (
        path
        / "SeasonalClimateFiles"
        / f"{sysname}.{plname}.SeasonalIceBalance.{timestamp}"
    )
    planckf = path / "SeasonalClimateFiles" / f"{sysname}.{plname}.PlanckB.{timestamp}"
    divf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalDivF.{timestamp}"
    )
    fmeridf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalFMerid.{timestamp}"
    )
    foutf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalFOut.{timestamp}"
    )
    insol = np.loadtxt(insolf, unpack=True)
    temp = np.loadtxt(tempf, unpack=True)
    ice = np.loadtxt(icef, unpack=True)
    planck = np.loadtxt(planckf, unpack=True)
    div = np.loadtxt(divf, unpack=True)
    fmerid = np.loadtxt(fmeridf, unpack=True)
    fout = np.loadtxt(fmeridf, unpack=True)

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
