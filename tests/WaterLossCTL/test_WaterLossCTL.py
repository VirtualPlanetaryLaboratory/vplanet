from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.b.MeanMotion": {"value": 1.592839e-05, "unit":/u.sec},
        "log.final.b.Eccentricity": {"value": 0.099920},
        "log.final.b.SurfWaterMass": {"value": 8.974649, "unit": u.TO},
        "log.final.b.OxygenMass": {"value":  5.192614, "unit": u.bars},
        "log.final.b.XO": {"value": 0.333876},
        "log.final.b.EtaO": {"value": 0.135210},
    }
)
class TestWaterLossCTL(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.b.MeanMotion, 1.592839e-05)
    # assert np.isclose(output.log.final.b.Eccentricity, 0.099920)
    # assert np.isclose(output.log.final.b.SurfWaterMass, 8.974649)
    # assert np.isclose(output.log.final.b.OxygenMass, 5.192614)
    # assert np.isclose(output.log.final.b.XO, 0.333876)
    # assert np.isclose(output.log.final.b.EtaO, 0.135210)
