from benchmark import Benchmark, benchmark
import astropy.units as u


@benchmark(
    [
        ("log.final.star.Luminosity", 7.362835e23 * u.W),
        ("log.final.star.LXUVStellar", 7.362835e20 * u.W),
        ("log.final.star.Radius", 1.186502e08 * u.m),
        ("log.final.star.Temperature", 2926.556751 * u.Kelvin),
        ("log.final.star.RadGyra", 0.466090),
        ("log.final.b.SurfWaterMass", 4.187987 * u.TO, dict(rtol=1e-4)),
        ("log.final.b.OxygenMass", 251.127387 * u.bar, dict(rtol=1e-4)),
        ("log.final.e.SurfWaterMass", 7.511356 * u.TO),
        ("log.final.e.OxygenMass", 420.619083 * u.bar),
        ("log.final.e.FXUV", 3.053257 * u.W / u.m ** 2),
        ("log.final.e.AtmXAbsEffH2O", 0.051776),
        ("log.final.e.Instellation", 3053.257033 * u.F_F_Earth),
    ]
)
class TestApseLock(Benchmark):
    pass
