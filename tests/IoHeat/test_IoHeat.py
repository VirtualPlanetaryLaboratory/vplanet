from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.initial.io.PowerEqtide": {"value": 9.380954e+13, "unit": u.kg},
        "log.initial.io.SurfEnFluxEqtide": {"value": 2.243481, "unit": u.kg*u.m**2/u.sec**2/(u.m**2*u.sec)},
        "log.initial.io.DsemiDtEqtide,": {"value":  -1.671102e-09, "unit": u.m/u.sec },
        "log.initial.io.DeccDtEqtide": {"value": -8.503575e-16, "unit": /u.sec},
        "log.initial.io.DOblDtEqtide": {"value": -3.792877e-13, "unit": u.rad/u.sec},
    }
)
class TestIoHeat(Benchmark):
    pass

    # # Run our comparisons
    # assert np.isclose(output.log.initial.io.PowerEqtide, 9.380954e+13)
    # assert np.isclose(output.log.initial.io.SurfEnFluxEqtide, 2.243481)
    # assert np.isclose(output.log.initial.io.DsemiDtEqtide, -1.671102e-09)
    # assert np.isclose(output.log.initial.io.DeccDtEqtide, -8.503575e-16)
    # assert np.isclose(output.log.initial.io.DOblDtEqtide, -3.792877e-13)
