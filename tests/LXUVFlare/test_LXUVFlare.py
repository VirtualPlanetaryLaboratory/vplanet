from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.star.LXUVFlare": {"value": 4.8097270621e-07, "unit": u.LSUN},
        "log.final.star.LXUVStellar": {"value": 1.9376774040e-08, "unit": u.LSUN},
    }
)
class TestLXUVFlare(Benchmark):
    pass
