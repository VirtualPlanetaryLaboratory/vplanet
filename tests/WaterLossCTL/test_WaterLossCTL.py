from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_WaterLossCTL():
    """Test the loss of water on planet tidally evolving via the CTL model."""
    # Remove old log file
    subprocess.run(['rm', 'WaterCTL.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.b.MeanMotion, 1.592839e-05)
    assert np.isclose(output.log.final.b.Eccentricity, 0.099920)
    assert np.isclose(output.log.final.b.SurfWaterMass, 8.974649)
    assert np.isclose(output.log.final.b.OxygenMass, 5.192614)
    assert np.isclose(output.log.final.b.XO, 0.333876)
    assert np.isclose(output.log.final.b.EtaO, 0.135210)

if __name__ == "__main__":
    test_WaterLossCTL()
