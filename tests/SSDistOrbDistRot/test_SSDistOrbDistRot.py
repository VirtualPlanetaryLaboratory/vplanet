from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Mercury.ArgP": {"value": 1.016312, "unit": u.rad},
        "log.final.Venus.Eccentricity": {"value": 0.003504},
        "log.final.Earth.Obliquity": {"value": 0.394576, "unit": u.rad},
        "log.final.Mars.PrecA": {"value":  -5.6172614113858515e+04, "unit": u.rad},
        "log.final.Jupiter.Eccentricity": {"value": 0.058517},
        "log.final.Saturn.Inc": {"value": 0.039880, "unit": u.rad},
        "log.final.George.ArgP": {"value": 1.631639, "unit": u.rad},
        "log.final.Neptune.LongA": {"value": 2.287481, "unit": u.rad},

    }
)
class TestSSDistOrbDistRot(Benchmark):
    pass

    # Run our comparisons
    # assert np.isclose(output.log.final.Mercury.ArgP, 1.016312)
    # assert np.isclose(output.log.final.Venus.Eccentricity, 0.003504)
    # assert np.isclose(output.log.final.Earth.Obliquity, 0.394576)
    # assert np.isclose(output.log.final.Mars.PrecA, 0.420712)
    # assert np.isclose(output.log.final.Jupiter.Eccentricity, 0.058517)
    # assert np.isclose(output.log.final.Saturn.Inc, 0.039880)
    # assert np.isclose(output.log.final.George.ArgP, 1.631639)
    # assert np.isclose(output.log.final.Neptune.LongA, 2.287481)
