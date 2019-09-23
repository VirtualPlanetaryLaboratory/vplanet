from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_RRLimited():
    """Test radiation/recombination-limited atmospheric escape"""
    # Remove old log file
    subprocess.run(['rm', 'RRLimited.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Make sure RR-limited escape is selected
    assert np.isclose(output.log.initial.rr.HEscapeRegime, 6.0)

    # Check mass loss rate [kg/s] - needs to agree w/ Eqn. 13 from Luger+2015
    assert np.isclose(output.rr.DEnvMassDt[0], -7.111259e+04)

    # Check critical flux [W/m^2] - needs to agree w/ Eqn. A23 from Luger+2015
    # Note - system isn't actual in RR escape - we set bUseRRLimited to force
    # RR-limited escape since dFXUV < the critical flux
    assert(np.isclose(output.log.initial.rr.RRCriticalFlux, 53.688843))


if __name__ == "__main__":
    test_RRLimited()
