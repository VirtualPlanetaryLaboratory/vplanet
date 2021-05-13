from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        "log.final.earth.Eccentricity": {"value": 0.200000},
        "log.final.earth.Obliquity": {"value": 0.855211, "unit": u.rad},
        "log.final.earth.AnnInsol": {"value": 310.172861, "unit": u.W / u.m**2},
        "log.final.earth.DivFlux": {"value": -8.595168, "unit": u.W / u.m**2},
        "log.final.earth.EnergyResL": {"value": -0.414500},
        "log.final.earth.EnergyResW": {"value": 0.113084},
        "log.final.earth.AlbedoLandLat": {"value": 0.508116},
        "log.final.earth.AlbedoWaterLat": {"value": 0.309142},

    }
)
class TestForceEccObl(Benchmark):
    pass

#     # Check global properties
#     assert np.isclose(output.log.final.earth.Eccentricity, 0.200000)
#     assert np.isclose(output.log.final.earth.Obliquity, 0.855211)
#     assert np.isclose(output.log.final.earth.AnnInsol, 310.172861)
#     assert np.isclose(output.log.final.earth.DivFlux, -8.595168)
#     assert np.isclose(output.log.final.earth.EnergyResL, -0.414500)
#     assert np.isclose(output.log.final.earth.EnergyResW, 0.113084)
#     assert np.isclose(output.log.final.earth.AlbedoLandLat, 0.508116)
#     assert np.isclose(output.log.final.earth.AlbedoWaterLat, 0.309142)
#
