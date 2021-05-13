from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.d.RotPer": {"value": 44.658583, "unit":u.days},
        "log.final.d.Obliquity": {"value": 0.328813, "unit":u.rad},

    }
)
class TestTideLockCPL(Benchmark):
    pass


    # # Run our comparisons
    # assert np.isclose(output.log.final.d.RotPer, 44.658583)
    # assert np.isclose(output.log.final.d.Obliquity, 0.328813)
