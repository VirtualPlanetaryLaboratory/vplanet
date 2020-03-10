from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_NBodyAtmEsc():
    """Test coupling between AtmEsc and SpiNBody."""
    # Remove old log file
    subprocess.run(['rm', 'PSR1257.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.Pulsar.PositionXSpiNBody, 1.72523e+05)
    assert np.isclose(output.log.final.Pulsar.VelZSpiNBody, 0.10003)
    assert np.isclose(output.log.final.A.EnvelopeMass, 0.99913)
    assert np.isclose(output.log.final.A.PlanetRadius, 59.80530)
    assert np.isclose(output.log.final.A.ArgP, 2.52340)
    assert np.isclose(output.log.final.A.PositionXSpiNBody, 2.61203e+10)
    assert np.isclose(output.log.final.B.LongA, 6.27639)
    assert np.isclose(output.log.final.B.VelXSpiNBody, 4.40398e+04)
    assert np.isclose(output.log.final.C.SemiMajorAxis,  6.97244e+10)
    assert np.isclose(output.log.final.C.PositionZSpiNBody, 4.87735e+10)


if __name__ == "__main__":
    test_NBodyAtmEsc()
