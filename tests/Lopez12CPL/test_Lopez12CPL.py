from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.auto.OrbPeriod": {"value": 9.650538e+05, "unit": u.sec},
        "log.final.auto.RotRate": {"value": 6.510710e-06, "unit": /u.sec},
        "log.final.auto.Eccentricity": {"value": 0.129902},
        "log.final.auto.EnvelopeMass": {"value":  0.411359, "unit": u.Mearth}, #should be earth masses but is currently dimentionless?
        "log.final.auto.PlanetRadius": {"value": 25.560564, "unit": u.Rearth}, #should be earth radii but is currently dimentionless?
        "log.final.auto.DEnvMassDt": {"value": -7.802590e+07, "unit": u.kg/ u.sec},

        "log.final.bondi.SemiMajorAxis": {"value": 0.100004, "unit": u.m},
        "log.final.bondi.RotVel": {"value": 40.154732, "unit":},
        "log.final.bondi.KEcc": {"value": 0.200007, "unit":},
        "log.final.bondi.EnvelopeMass": {"value": 0.0, "unit": u.Mearth}, #should be earth masses but is currently dimentionless?
        "log.final.bondi.PlanetRadius": {"value": 1.000000, "unit": u.Rearth}, #should be earth radii but is currently dimentionless?
        "log.final.bondi.DEnvMassDt": {"value":  0.0, "unit": u.kg/ u.sec},

        "log.final.el.SemiMajorAxis": {"value": 0.096591, "unit": u.m},
        "log.final.el.RotAngMom": {"value": 5.470333e+35, "unit":},
        "log.final.el.Eccentricity": {"value": 0.068456},
        "log.final.el.EnvelopeMass": {"value": 0.986370, "unit": u.Mearth}, #should be earth masses but is currently dimentionless?},
        "log.final.el.PlanetRadius": {"value": 32.683276, "unit": u.Rearth}, #should be earth radii but is currently dimentionless?
        "log.final.el.BondiRadius": {"value": 7.982897e+08, "unit": u.m},

        "log.final.rr.MeanMotion": {"value": 6.626773e-06, "unit": /u.sec},
        "log.final.rr.RotKinEnergy": {"value": 1.842803e+30, "unit": u.kg*u.m**2/u.sec**2},
        "log.final.rr.Eccentricity": {"value": 0.068275},
        "log.final.rr.EnvelopeMass": {"value": 0.999399, "unit": u.Mearth}, #should be earth masses but is currently dimentionless?},
        "log.final.rr.PlanetRadius": {"value": 32.861293, "unit": u.Rearth}, #should be earth radii but is currently dimentionless?
        "log.final.rr.RocheRadius": {"value": 1.822097e+08, "unit": u.m},





    }
)
class TestIoHeat(Benchmark):
    pass


    # Check
    # assert np.isclose(output.log.final.auto.OrbPeriod, 9.650538e+05)
    # assert np.isclose(output.log.final.auto.RotRate, 6.510710e-06)
    # assert np.isclose(output.log.final.auto.Eccentricity, 0.129902)
    # assert np.isclose(output.log.final.auto.EnvelopeMass,  0.411359)
    # assert np.isclose(output.log.final.auto.PlanetRadius, 25.560564)
    # assert np.isclose(output.log.final.auto.DEnvMassDt, -7.802590e+07)
    #
    # assert np.isclose(output.log.final.bondi.SemiMajorAxis, 0.100004)
    # assert np.isclose(output.log.final.bondi.RotVel, 40.154732)
    # assert np.isclose(output.log.final.bondi.KEcc, 0.200007)
    # assert np.isclose(output.log.final.bondi.EnvelopeMass,  0.0)
    # assert np.isclose(output.log.final.bondi.PlanetRadius, 1.000000)
    # assert np.isclose(output.log.final.bondi.DEnvMassDt, 0.0)
    #
    # assert np.isclose(output.log.final.el.SemiMajorAxis, 0.096591)
    # assert np.isclose(output.log.final.el.RotAngMom, 5.470333e+35)
    # assert np.isclose(output.log.final.el.Eccentricity, 0.068456)
    # assert np.isclose(output.log.final.el.EnvelopeMass, 0.986370)
    # assert np.isclose(output.log.final.el.PlanetRadius, 32.683276)
    # assert np.isclose(output.log.final.el.BondiRadius, 7.982897e+08)
    #
    # assert np.isclose(output.log.final.rr.MeanMotion, 6.626773e-06)
    # assert np.isclose(output.log.final.rr.RotKinEnergy, 1.842803e+30)
    # assert np.isclose(output.log.final.rr.Eccentricity, 0.068275)
    # assert np.isclose(output.log.final.rr.EnvelopeMass, 0.999399)
    # assert np.isclose(output.log.final.rr.PlanetRadius, 32.861293)
    # assert np.isclose(output.log.final.rr.RocheRadius, 1.822097e+08)
