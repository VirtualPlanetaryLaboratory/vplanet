from benchmark import Benchmark, benchmark
import astropy.units as u


@benchmark(
    {
        "log.final.star.Luminosity": {"value": 7.362835e23, "unit": u.W},
        "log.final.star.LXUVStellar": {"value": 7.362835e20, "unit": u.W},
        "log.final.star.Radius": {"value": 1.186502e08, "unit": u.m},
        "log.final.star.Temperature": {"value": 2926.556751, "unit": u.Kelvin},
        "log.final.star.RadGyra": {"value": 0.466090},
        "log.final.b.SurfWaterMass": {"value": 4.187987, "unit": u.TO, "rtol": 1e-4},
        "log.final.b.OxygenMass": {"value": 251.127387, "unit": u.bar, "rtol": 1e-4},
        "log.final.e.SurfWaterMass": {"value": 7.511356, "unit": u.TO},
        "log.final.e.OxygenMass": {"value": 420.619083, "unit": u.bar},
        "log.final.e.FXUV": {"value": 3.053257, "unit": u.W / u.m ** 2},
        "log.final.e.AtmXAbsEffH2O": {"value": 0.051776},
        "log.final.e.Instellation": {"value": 3053.257033, "unit": u.kg / u.sec ** 3},
    }
)
class TestAbioticO2(Benchmark):
    pass
