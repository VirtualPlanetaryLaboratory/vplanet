from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_TideLockCPL():
    """Test tidal locking in the EqTide-CPL model."""
    # Remove old log file
    subprocess.run(['rm', 'gl581.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.d.RotPer, 44.658583)
    assert np.isclose(output.log.final.d.Obliquity, 0.328813)

if __name__ == "__main__":
    test_TideLockCPL()
