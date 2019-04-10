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
    assert np.isclose(output.log.final.star.Luminosity, 2.013347e+23)
    assert np.isclose(output.log.final.star.LXUVStellar, 2.774075e+19)
    assert np.isclose(output.log.final.star.Radius, 7.770586e+07)
    assert np.isclose(output.log.final.star.Temperature, 2618.768109)
    assert np.isclose(output.log.final.e.SurfWaterMass, 3.273452, rtol=1e-4)
    assert np.isclose(output.log.final.star.HZLimRecVenus, 2.811125e+09)
    assert np.isclose(output.log.final.star.HZLimEarlyMars, 7.885448e+09)

if __name__ == "__main__":
    test_AbioticO2()
