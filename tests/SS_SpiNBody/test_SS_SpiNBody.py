from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.Star.PositionXSpiNBody": {"value": -7.2996045904859365e05},
        "log.final.Star.VelZSpiNBody": {"value": -0.0074053658185989},
        "log.final.Mercury.Inc": {"value": 0.1222430915966078, "unit": u.rad},
        "log.final.Mercury.VelXSpiNBody": {"value": -5.6172614113858515e04},
        "log.final.Venus.ArgP": {"value": 0.9555745288739304, "unit": u.rad},
        "log.final.Venus.PositionXSpiNBody": {"value": 1.0815757150556880e11},
        "log.final.Earth.LongA": {"value": 6.0858921788484137, "unit": u.rad},
        "log.final.Earth.VelZSpiNBody": {"value": 1751.8263488484312802},
        "log.final.Mars.SemiMajorAxis": {"value": 2.2794298912149789e11, "unit": u.m},
        "log.final.Mars.PositionYSpiNBody": {"value": -1.9293056341095392e11},
    }
)
class TestSS_SpiNBody(Benchmark):
    pass
