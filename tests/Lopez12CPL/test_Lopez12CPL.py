from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.auto.OrbPeriod": {"value": 9.650538e05, "unit": u.sec},
        "log.final.auto.RotRate": {"value": 6.510710e-06, "unit": 1 / u.sec},
        "log.final.auto.Eccentricity": {"value": 0.129902},
        "log.final.auto.EnvelopeMass": {
            "value": 0.411359,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },  # should be earth masses but is currently dimentionless?
        "log.final.auto.PlanetRadius": {
            "value": 25.560564,
            "unit": u.Rearth,
        },  # should be earth radii but is currently dimentionless?
        "log.final.auto.DEnvMassDt": {"value": -7.802590e07, "unit": u.kg / u.sec},
        "log.final.bondi.SemiMajorAxis": {"value": 0.100004, "unit": u.AU},
        "log.final.bondi.RotVel": {"value": 40.154732, "unit": u.m / u.s},
        "log.final.bondi.KEcc": {"value": 0.200007},
        "log.final.bondi.EnvelopeMass": {
            "value": 0.0,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },  # should be earth masses but is currently dimentionless?
        "log.final.bondi.PlanetRadius": {
            "value": 1.000000,
            "unit": u.Rearth,
        },  # should be earth radii but is currently dimentionless?
        "log.final.bondi.DEnvMassDt": {"value": 0.0, "unit": u.kg / u.sec},
        "log.final.el.SemiMajorAxis": {"value": 0.096591, "unit": u.AU},
        "log.final.el.RotAngMom": {"value": 5.470333e35, "unit": u.kg * u.m ** 2 / u.s},
        "log.final.el.Eccentricity": {"value": 0.068456},
        "log.final.el.EnvelopeMass": {
            "value": 0.986370,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },  # should be earth masses but is currently dimentionless?},
        "log.final.el.PlanetRadius": {
            "value": 32.683276,
            "unit": u.Rearth,
        },  # should be earth radii but is currently dimentionless?
        "log.final.el.BondiRadius": {"value": 7.982897e08, "unit": u.m},
        "log.final.rr.MeanMotion": {"value": 6.626773e-06, "unit": 1 / u.sec},
        "log.final.rr.RotKinEnergy": {
            "value": 1.842803e30,
            "unit": u.kg * u.m ** 2 / u.sec ** 2,
        },
        "log.final.rr.Eccentricity": {"value": 0.068275},
        "log.final.rr.EnvelopeMass": {
            "value": 0.999399,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },  # should be earth masses but is currently dimentionless?},
        "log.final.rr.PlanetRadius": {
            "value": 32.861293,
            "unit": u.Rearth,
        },  # should be earth radii but is currently dimentionless?
        "log.final.rr.RocheRadius": {"value": 1.822097e08, "unit": u.m},
    }
)
class TestIoHeat(Benchmark):
    pass
