from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.sun.LXUVStellar": {"value": 0.000427, "unit":u.lsun},
        "log.final.venus.SurfWaterMass": {"value": 0.099247, "unit": u.TO},

    }
)
class TestVenusWaterLoss(Benchmark):
    pass

    # # Check
    # assert np.isclose(output.log.final.sun.LXUVStellar, 0.000427)
    # assert np.isclose(output.log.final.venus.SurfWaterMass, 0.919493, rtol=1e-4)
