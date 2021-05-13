from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest

@benchmark(
    {
        "log.final.a.Luminosity": {"value": 0.002445, "unit": u.Lsun},
        "log.final.a.Temperature": {"value": 2992.330343, "unit": u.K},
        "log.final.a.Radius": {"value": 0.184359, "unit": u.Rsolar},
        "log.final.a.LostAngMom": {"value":  9.438825e+41, "unit": u.kg*u.m**2/u.sec},
        "log.final.a.RotPer": {"value": 0.299254, "unit": u.days},

        "log.final.b.Luminosity": {"value": 0.689088, "unit": u.Lsun}},
        "log.final.b.Temperature": {"value": 5539.190016, "unit": u.K},
        "og.final.b.Radius": {"value": 0.902638, "unit": u.Rsolar},
        "log.final.b.LostAngMom": {"value": 5.086001e+43, "unit": u.u.kg*u.m**2/u.sec},
        "log.final.b.RotPer": {"value": 3.075721, "unit": u.days},
        "log.final.b.Density": {"value": 1916.956602, "unit": u.kg/u.m**3},


    }
)
class TestStellarEvol(Benchmark):
    pass

    # # Star a
    # assert np.isclose(output.log.final.a.Luminosity, 0.002445)
    # assert np.isclose(output.log.final.a.Temperature, 2992.330343)
    # assert np.isclose(output.log.final.a.Radius, 0.184359, rtol=1.0e-4)
    # assert np.isclose(output.log.final.a.LostAngMom, 9.438825e+41, rtol=1.0e-4)
    # assert np.isclose(output.log.final.a.RotPer, 0.299254, rtol=1.0e-4)
    #
    # # Star b
    # assert np.isclose(output.log.final.b.Luminosity, 0.689088, rtol=1.0e-4)
    # assert np.isclose(output.log.final.b.Temperature, 5539.190016)
    # assert np.isclose(output.log.final.b.Radius, 0.902638, rtol=1.0e-4)
    # assert np.isclose(output.log.final.b.LostAngMom, 5.086001e+43, rtol=1.0e-4)
    # assert np.isclose(output.log.final.b.RotPer, 3.075721, rtol=1.0e-4)
    # assert np.isclose(output.log.final.b.Density, 1916.956602)
    #
    # #assert np.isclose(output.log.final.a.HZLimRecVenus, 6.038467e+09)
    # #assert np.isclose(output.log.final.a.HZLimEarlyMars, 1.669770e+10)
    # #assert np.isclose(output.log.final.a.HZLimRunaway, 7.949225e+09)
    # #assert np.isclose(output.log.final.a.HZLimMoistGreenhouse, 7.989892e+09)
    # #assert np.isclose(output.log.final.a.HZLimMaxGreenhouse, 1.531054e+10)
    #
    # # Energy
    # Einit = output.log.initial.system.TotEnergy
    # Efinal = output.log.initial.system.TotEnergy
    #
    # # Angular momentum
    # Linit = output.log.initial.system.TotAngMom
    # Lfinal = output.log.final.system.TotAngMom
    #
    # # Check conservation
    # assert np.isclose(Einit, Efinal, rtol=1e-3)
    # assert np.isclose(Linit, Lfinal, rtol=1e-3)
