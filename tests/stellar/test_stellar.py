from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_stellar():
    """Test the stellar module."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.a.Luminosity, 0.000869)
    assert np.isclose(output.log.final.a.Temperature, 2811.000999)
    assert np.isclose(output.log.final.a.Radius, 0.124)
    assert np.isclose(output.log.final.a.LostAngMom, 1.290268e+40,
                      rtol=1e-3)
    assert np.isclose(output.log.final.a.RotPer, 1.921597, rtol=1e-3)
    assert np.isclose(output.log.final.b.Luminosity, 1.021926)
    assert np.isclose(output.log.final.b.Temperature, 5730.814496)
    assert np.isclose(output.log.final.b.Radius, 1.026419)
    assert np.isclose(output.log.final.b.LostAngMom, 1.648878e+43,
                      rtol=1e-3)
    assert np.isclose(output.log.final.b.RotPer, 23.680862, rtol=1e-3)
    assert np.isclose(output.log.final.system.TotAngMom, 1.729433e+43,
                      rtol=1e-3)
    assert np.isclose(output.log.final.system.TotEnergy, -2.468221e+41,
                      rtol=1e-3)


if __name__ == "__main__":
    test_stellar()
