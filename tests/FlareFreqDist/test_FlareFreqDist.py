from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {   
        "log.initial.star.FlareFreq1": {"value": 0.2061877778, "unit": 1 / u.days},
        "log.initial.star.FlareFreq2": {"value": 0.1896440113 , "unit": 1 / u.days},
        "log.initial.star.FlareFreq3": {"value": 0.1744276571 , "unit": 1 / u.days},
        "log.final.star.LXUVFlare": {"value":  4.4651071943e-07, "unit": u.LSUN},
        "star.LXUVFlare": {"index": -1, "value":  4.4651071943e-07, "unit": u.LSUN},
        "star.LXUVTot": {"index": -1, "value":  4.4651071943e-07, "unit": u.LSUN}, 
    }
)
class TestFlareFreqDist(Benchmark):
    pass
