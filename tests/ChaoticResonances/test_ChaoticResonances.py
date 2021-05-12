from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.earth.PositionXSpiNBody": {
            "value": -1.0910217698713353e11,
            "unit": u.m,
            "marks": pytest.mark.xfail(
                reason="PositionXSpiNBody is unitless in vplanet, but it should have units of length."
            ),
        },
        "log.final.earth.VelZSpiNBody": {
            "value": 1.2959382255760662e04,
            "unit": u.m / u.s,
            "marks": pytest.mark.xfail(
                reason="VelZSpiNBody is unitless in vplanet, but it should have units of speed."
            ),
        },
        "log.final.earth.Inc": {"value": 0.7616663331648603, "unit": u.rad},
        "log.final.earth.MeanMotion": {
            "value": 1.9912661532603589e-07,
            "unit": 1 / u.s,
        },
        "log.final.outer.PositionYSpiNBody": {
            "value": 2.0247776958475812e11,
            "unit": u.m,
            "marks": pytest.mark.xfail(
                reason="PositionYSpiNBody is unitless in vplanet, but it should have units of length."
            ),
        },
        "log.final.outer.VelXSpiNBody": {
            "value": -1.1620721010188581e04,
            "unit": u.m / u.s,
            "marks": pytest.mark.xfail(
                reason="VelXSpiNBody is unitless in vplanet, but it should have units of speed."
            ),
        },
        "log.final.outer.ArgP": {"value": 1.7362790073768690, "unit": u.rad},
        "log.final.outer.SemiMajorAxis": {"value": 3.1118441456657074e11, "unit": u.m},
    }
)
class TestChaoticResonances(Benchmark):
    pass
