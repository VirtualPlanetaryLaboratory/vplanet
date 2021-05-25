from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Mercury.ArgP": {"value": 1.016312, "unit": u.rad},
        "log.final.Venus.Eccentricity": {"value": 0.003504},
        "log.final.Earth.Obliquity": {"value": 0.394576, "unit": u.rad},
        "log.final.Mars.PrecA": {"value": 0.420712, "unit": u.rad},
        "log.final.Jupiter.Eccentricity": {"value": 0.058517},
        "log.final.Saturn.Inc": {"value": 0.039880, "unit": u.rad},
        "log.final.George.ArgP": {"value": 1.631639, "unit": u.rad},
        "log.final.Neptune.LongA": {"value": 2.287481, "unit": u.rad},
    }
)
class TestSSDistOrbDistRot(Benchmark):
    pass
