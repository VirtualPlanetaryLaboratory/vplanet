from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_TideLockCTL():
    """Test tidal locking in the EqTide-TPL model."""
    # Remove old log file
    subprocess.run(['rm', 'gl581.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.d.RotPer, 34.706)
    assert np.isclose(output.log.final.d.Obliquity, 0.0133141669439806)

if __name__ == "__main__":
    test_TideLockCTL()
