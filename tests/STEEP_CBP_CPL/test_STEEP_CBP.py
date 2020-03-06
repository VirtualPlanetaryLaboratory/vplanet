from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_SteepCBPCPL():
    """Test STEEP effect on a circumbinary planet (Fleming et al., 2018, ApJ, 858, 86)."""
    # Remove old log file
    subprocess.run(['rm', 'STEEP_CPL.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.final.cbp.FreeEcc, 0.030000)
    assert np.isclose(output.log.final.cbp.Eccentricity, 0.030249)
    assert np.isclose(output.log.final.cbp.SemiMajorAxis, 1.052219e+11)
    assert np.isclose(output.log.final.secondary.Eccentricity, 0.194791)
    assert np.isclose(output.log.final.secondary.SemiMajorAxis, 0.072294)
    assert np.isclose(output.log.final.secondary.CriticalSemiMajorAxis, 0.211427)


if __name__ == "__main__":
    test_SteepCBPCPL()
