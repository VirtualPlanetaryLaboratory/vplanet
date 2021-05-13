from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.d.RotPer": {"value": 34.706, "unit":u.days},
        "log.final.d.Obliquity": {"value": 0.0133141669439806, "unit":u.rad},

    }
)
class TestTideLockCTL(Benchmark):
    pass


    # # Run our comparisons
    # assert np.isclose(output.log.final.d.RotPer, 34.706)
    # assert np.isclose(output.log.final.d.Obliquity, 0.0133141669439806)
