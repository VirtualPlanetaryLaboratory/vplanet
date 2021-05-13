from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.final.star.LXUVStellar": {"value": 4.988314e+21, "unit": u.kg * u.m**2 / u.sec**3},

        "log.final.ProxCenELim.EnvelopeMass": {"value": 0.000526, "unit" : u.Mearth},
        "log.final.ProxCenELim.PlanetRadius": {"value": 1.335673, "unit": u.Rearth},
        "log.final.ProxCenELim.SurfWaterMass": {"value": 1.500000,"unit": u.TO},

        "log.final.ProxCenRRLim.EnvelopeMass": {"value": 0.000300, "unit" : u.Mearth},
        "log.final.ProxCenRRLim.PlanetRadius": {"value": 1.339401, "unit": u.Rearth},
        "log.final.ProxCenRRLim.SurfWaterMass": {"value": 1.500000,"unit": u.TO},

        "log.final.ProxCenAuto.EnvelopeMass": {"value": 0.000526, "unit" : u.Mearth},
        "log.final.ProxCenAuto.PlanetRadius": {"value": 1.335673, "unit": u.Rearth},
        "log.final.ProxCenAuto.SurfWaterMass": {"value": 1.500000,"unit": u.TO},

        "log.final.LC17ELim.EnvelopeMass": {"value": 0.000000, "unit" : u.Mearth},
        "log.final.LC17ELim.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17ELim.SurfWaterMass": {"value": 1.304141,"unit": u.TO},

        "log.final.LC17RRLim.EnvelopeMass": {"value": 0.000000, "unit" : u.Mearth},
        "log.final.LC17RRLim.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17RRLim.SurfWaterMass": {"value": 1.347668,"unit": u.TO},

        "log.final.LC17Auto.EnvelopeMass": {"value": 0.000000, "unit" : u.Mearth},
        "log.final.LC17Auto.PlanetRadius": {"value": 1.063513, "unit": u.Rearth},
        "log.final.LC17Auto.SurfWaterMass": {"value": 1.347368,"unit": u.TO},

    }
)
class TestHabEvapCore(Benchmark):
    pass


    # # Run our comparisons
    # assert np.isclose(output.log.final.star.LXUVStellar, 4.988314e+21)
    #
    # assert np.isclose(output.log.final.ProxCenELim.EnvelopeMass, 0.000526)
    # assert np.isclose(output.log.final.ProxCenELim.PlanetRadius, 1.335673)
    # assert np.isclose(output.log.final.ProxCenELim.SurfWaterMass, 1.500000)
    #
    # assert np.isclose(output.log.final.ProxCenRRLim.EnvelopeMass, 0.000300)
    # assert np.isclose(output.log.final.ProxCenRRLim.PlanetRadius, 1.339401)
    # assert np.isclose(output.log.final.ProxCenRRLim.SurfWaterMass, 1.500000)
    #
    # assert np.isclose(output.log.final.ProxCenAuto.EnvelopeMass, 0.000526)
    # assert np.isclose(output.log.final.ProxCenAuto.PlanetRadius, 1.335673)
    # assert np.isclose(output.log.final.ProxCenAuto.SurfWaterMass, 1.500000)
    #
    # assert np.isclose(output.log.final.LC17ELim.EnvelopeMass, 0.000000)
    # assert np.isclose(output.log.final.LC17ELim.PlanetRadius, 1.063513)
    # assert np.isclose(output.log.final.LC17ELim.SurfWaterMass, 1.304141)
    #
    # assert np.isclose(output.log.final.LC17RRLim.EnvelopeMass, 0.000000)
    # assert np.isclose(output.log.final.LC17RRLim.PlanetRadius, 1.063513)
    # assert np.isclose(output.log.final.LC17RRLim.SurfWaterMass, 1.347668)
    #
    # assert np.isclose(output.log.final.LC17Auto.EnvelopeMass, 0.000000)
    # assert np.isclose(output.log.final.LC17Auto.PlanetRadius, 1.063513)
    # assert np.isclose(output.log.final.LC17Auto.SurfWaterMass, 1.347368)
