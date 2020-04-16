from vplot import GetOutput
import subprocess
import numpy as np
import os
import re
cwd = os.path.dirname(os.path.realpath(__file__))


def test_ForceEccObl():
    """Test sinusoidal variations in eccentricity and obliquity."""
    # Remove old log file
    subprocess.run(['rm', 'tilted.log'], cwd=cwd)
    # Remove seasonal climate files
    dir = cwd+'/SeasonalClimateFiles'
    subprocess.run(['rm','-r',dir])
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check global properties
    assert np.isclose(output.log.final.earth.Eccentricity, 0.200000)
    assert np.isclose(output.log.final.earth.Obliquity, 0.855211)
    assert np.isclose(output.log.final.earth.AnnInsol, 310.172861)
    assert np.isclose(output.log.final.earth.DivFlux, -8.595168)
    assert np.isclose(output.log.final.earth.EnergyResL, -0.414500)
    assert np.isclose(output.log.final.earth.EnergyResW, 0.113084)
    assert np.isclose(output.log.final.earth.AlbedoLandLat, 0.508116)
    assert np.isclose(output.log.final.earth.AlbedoWaterLat, 0.309142)

if __name__ == "__main__":
    test_ForceEccObl()
