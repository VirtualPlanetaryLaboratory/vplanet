from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_AbioticO2():
    """Test oxygen build up, HZ limits."""
    # Remove old log file
    subprocess.run(['rm', 'AbioticO2.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Primary Variables
    # Star
    assert np.isclose(output.log.final.star.Luminosity, 7.362835e+23)
    assert np.isclose(output.log.final.star.LXUVStellar, 7.362835e+20)
    assert np.isclose(output.log.final.star.Radius, 1.186502e+08)
    assert np.isclose(output.log.final.star.Temperature, 2926.556751)
    assert np.isclose(output.log.final.star.RadGyra, 0.466090)

    # Planet b -- checks high XUV flux environment
    assert np.isclose(output.log.final.b.SurfWaterMass, 4.187987, rtol=1e-4)
    assert np.isclose(output.log.final.b.OxygenMass, 251.127387,rtol=1e-4)

    # Planet e -- checks low XUV flux environment
    assert np.isclose(output.log.final.e.SurfWaterMass, 7.511356, rtol=1e-4)
    assert np.isclose(output.log.final.e.OxygenMass, 420.619083)

    # Other checks
    assert np.isclose(output.log.final.e.FXUV, 3.053257)
    assert np.isclose(output.log.final.e.AtmXAbsEffH2O, 0.051776)
    assert np.isclose(output.log.final.e.Instellation, 3053.257033)


if __name__ == "__main__":
    test_AbioticO2()
