from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_AtmEscKepler36():
    """Test hydrogen envelope loss."""
    # Remove old log file
    subprocess.run(['rm', 'AtmEscKepler-36.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.star.LXUVStellar, 3.120390e+21)
    assert np.isclose(output.log.final.b.EnvelopeMass, 0)
    assert np.isclose(output.log.final.b.Radius, 6.378100e+06)
    assert np.isclose(output.log.final.c.EnvelopeMass, 3.972871)
    assert np.isclose(output.log.final.c.Radius, 3.5899080e+07)


if __name__ == "__main__":
    test_AtmEscKepler36()
