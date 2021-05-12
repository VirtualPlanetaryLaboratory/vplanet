from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.TGstar.Radius": {
            "value": 0.878726,
            "unit": u.Rsun,
            "marks": pytest.mark.xfail(
                reason="Radius has units of `Solar`, which is ambiguous."
            ),
        },
        "log.final.TGstar.RadGyra": {"value": 0.445942},
        "log.final.TGstar.RotVel": {"value": 296.385042, "unit": u.m / u.s},
        "log.final.TGstar.EqRotRate": {"value": 1.407733e-05, "unit": 1 / u.s},
        "log.final.TGstar.LXUVStellar": {"value": 1.880010e22, "unit": u.W},
        "log.final.TGb.Instellation": {"value": 1.058375e05, "unit": u.F_F_Earth},
        "log.final.TGb.Yobl": {"value": -0.149030},
        "log.final.TGb.Zobl": {"value": 0.856552},
        "log.final.TGb.HEcc": {"value": 0.091315},
        "log.final.TGb.KEcc": {"value": -0.048856},
        "log.final.TGb.RotRate": {"value": 6.843648e-05, "unit": 1 / u.s},
        "log.final.TGb.SemiMajorAxis": {"value": 3.769868e09, "unit": u.m},
        "log.final.TGb.OrbPotEnergy": {"value": -1.766030e34, "unit": u.J},
        "log.final.TGb.SemiTimeEqtide": {"value": 7.942139e15, "unit": u.s},
        "log.final.TGb.DXoblDtEqtide": {"value": -3.658103e-12, "unit": 1 / u.year},
        "log.final.TGc.Sinc": {"value": 0.037712},
        "log.final.TGc.ArgP": {"value": 3.924108, "unit": u.rad},
        "log.final.TGc.PrecA": {"value": 2.477072, "unit": u.rad},
        "log.final.TGc.CassiniOne": {"value": 0.959339},
        "log.final.TGc.CassiniTwo": {"value": 0.282255},
    }
)
class TestCassiniMulti(Benchmark):
    pass
