from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_ChaoticResonances():
    """Test Chaotic Resoannces."""
    # Remove old log file
    subprocess.run(['rm', 'ChaosRes.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.earth.PositionXSpiNBody, -1.0910217698713353e+11)
    assert np.isclose(output.log.final.earth.VelZSpiNBody, 1.2959382255760662e+04)
    assert np.isclose(output.log.final.earth.Inc, 0.7616663331648603)
    assert np.isclose(output.log.final.earth.MeanMotion, 1.9912661532603589e-07)
    assert np.isclose(output.log.final.outer.PositionYSpiNBody, 2.0247776958475812e+11)
    assert np.isclose(output.log.final.outer.VelXSpiNBody, -1.1620721010188581e+04)
    assert np.isclose(output.log.final.outer.ArgP, 1.7362790073768690)
    assert np.isclose(output.log.final.outer.SemiMajorAxis, 3.1118441456657074e+11)

if __name__ == "__main__":
    test_ChaoticResonances()
