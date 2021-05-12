from benchmark import Benchmark, benchmark
import astropy.units as u


@benchmark(
    [
        ("log.final.b.HEcc", -0.042671),
        ("log.final.b.KEcc", -0.069231),
        ("log.final.b.DHEccDtEqtide", 2.269581e-15 / u.year),
        ("log.final.b.DKEccDtEqtide", 3.682246e-15 / u.year),
        ("log.final.b.DHeccDtDistOrb", -2.488505e-11 / u.year),
        ("log.final.b.DKeccDtDistOrb", -1.344098e-11 / u.year),
        ("log.final.c.SemiMajorAxis", 6.911428e09 * u.m),
        ("log.final.b.Eccentricity", 0.081326),
        ("log.final.b.ArgP", 3.693953 * u.rad),
        ("log.final.b.SemiMajorAxis", 2.812439e09 * u.m),
        ("log.final.c.Eccentricity", 0.193811),
        ("log.final.c.ArgP", 5.015526 * u.rad),
    ]
)
class TestApseLock(Benchmark):
    pass
