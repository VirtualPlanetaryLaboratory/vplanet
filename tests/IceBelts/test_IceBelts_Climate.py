import pathlib

import astropy.units as u
import numpy as np
import pytest
from benchmark import Benchmark, benchmark

path = pathlib.Path(__file__).parents[0].absolute()
sysname = "icebelt"
plname = "earth"
timestamp = "0"


# class TestIceBelt(Benchmark):
#    pass


@pytest.fixture
def insol(vplanet_output):
    insolf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.DailyInsol.{timestamp}"
    )
    return np.loadtxt(insolf, unpack=True)


@pytest.fixture
def temp(vplanet_output):
    tempf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalTemp.{timestamp}"
    )
    return np.loadtxt(tempf, unpack=True)


@pytest.fixture
def ice(vplanet_output):
    icef = (
        path
        / "SeasonalClimateFiles"
        / f"{sysname}.{plname}.SeasonalIceBalance.{timestamp}"
    )
    return np.loadtxt(icef, unpack=True)


@pytest.fixture
def planck(vplanet_output):
    planckf = path / "SeasonalClimateFiles" / f"{sysname}.{plname}.PlanckB.{timestamp}"
    return np.loadtxt(planckf, unpack=True)


@pytest.fixture
def div(vplanet_output):
    divf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalDivF.{timestamp}"
    )
    return np.loadtxt(divf, unpack=True)


@pytest.fixture
def fmerid(vplanet_output):
    fmeridf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalFMerid.{timestamp}"
    )
    return np.loadtxt(fmeridf, unpack=True)


@pytest.fixture
def fout(vplanet_output):
    foutf = (
        path / "SeasonalClimateFiles" / f"{sysname}.{plname}.SeasonalFOut.{timestamp}"
    )
    return np.loadtxt(foutf, unpack=True)


class TestGridOutput:
    def test_insol1(self, insol):
        assert np.isclose(insol[1][0], 1055.352482)

    def test_insol2(self, insol):
        assert np.isclose(insol[1][75], 329.47451)

    def test_temp1(self, temp):
        assert np.isclose(temp[1][0], 22.591643)

    def test_temp2(self, temp):
        assert np.isclose(temp[1][75], 8.998626)

    def test_ice1(self, ice):
        assert np.isclose(ice[1][0], -0.0016)

    def test_ice2(self, ice):
        assert np.isclose(ice[1][75], -0.000306)

    def test_planck1(self, planck):
        assert np.isclose(planck[1][0], 2.09)

    def test_planck2(self, planck):
        assert np.isclose(planck[1][75], 2.09)

    def test_div1(self, div):
        assert np.isclose(div[1][0], 34.757649)

    def test_div2(self, div):
        assert np.isclose(div[1][75], 13.23595)

    def test_fmerid1(self, fmerid):
        assert np.isclose(fmerid[1][0], 804939200000000.0)

    def test_fmerid2(self, fmerid):
        assert np.isclose(fmerid[1][75], 309954000000000.0)

    def test_fout1(self, fout):
        assert np.isclose(fout[1][0], 250.516533)

    def test_fout2(self, fout):
        assert np.isclose(fout[1][75], 222.107129)
