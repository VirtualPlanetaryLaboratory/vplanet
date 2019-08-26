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
    assert np.isclose(output.log.final.star.Luminosity, 7.362970e+23)
    assert np.isclose(output.log.final.star.LXUVStellar, 7.362970e+20)
    assert np.isclose(output.log.final.star.Radius, 1.186513e+08)
    assert np.isclose(output.log.final.star.Temperature, 2926.559809)
    assert np.isclose(output.log.final.star.RadGyra, 0.466090)
    # Planet
    assert np.isclose(output.log.final.e.SurfWaterMass, 7.403320, rtol=1e-4)
    assert np.isclose(output.log.final.e.OxygenMass, 418.194151)

    # Other checks
    assert np.isclose(output.log.final.e.FXUV, 3.053313)
    assert np.isclose(output.log.final.e.AtmXAbsEffH2O, 0.051776)


if __name__ == "__main__":
    test_AbioticO2()
