from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_WaterLossCPL():
    """Test the loss of water on planet tidally evolving via the CPL model."""
    # Remove old log file
    subprocess.run(['rm', 'WaterCPL.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.b.OrbPeriod, 3.943855e+05)
    assert np.isclose(output.log.final.b.Eccentricity, 0.099247)
    assert np.isclose(output.log.final.b.SurfWaterMass, 9.974648)

if __name__ == "__main__":
    test_WaterLossCPL()
