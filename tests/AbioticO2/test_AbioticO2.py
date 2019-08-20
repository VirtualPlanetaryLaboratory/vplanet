from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_AbioticO2():
    """Test oxygen build up, HZ limits."""
    # Remove old log file
    subprocess.run(['rm', 'trappist1.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.star.Luminosity, 7.362896e+23)
    assert np.isclose(output.log.final.star.LXUVStellar, 7.362896e+20)
    assert np.isclose(output.log.final.star.Radius, 1.186507e+08)
    assert np.isclose(output.log.final.star.Temperature, 2926.558123)
    assert np.isclose(output.log.final.e.SurfWaterMass, 7.403340, rtol=1e-4)
    assert np.isclose(output.log.final.star.HZLimRecVenus, 5.349670e+09)
    assert np.isclose(output.log.final.star.HZLimEarlyMars, 1.482975e+10)

if __name__ == "__main__":
    test_AbioticO2()
