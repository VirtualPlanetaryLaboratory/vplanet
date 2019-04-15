from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_SS_NBody():
    """Test NBody Evolution."""
    # Remove old log file
    subprocess.run(['rm', 'SpiNBody.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    assert np.isclose(output.log.final.Star. PositionXSpiNBody, -7.2996045904859365e+05)
    assert np.isclose(output.log.final.Star.VelZSpiNBody, -0.0074053658185989)
    assert np.isclose(output.log.final.Mercury.Inc, 0.1222430915966078)
    assert np.isclose(output.log.final.Mercury.VelXSpiNBody, -5.6172614113858515e+04)
    assert np.isclose(output.log.final.Venus.ArgP, 0.9555745288739304)
    assert np.isclose(output.log.final.Venus.PositionXSpiNBody, 1.0815757150556880e+11)
    assert np.isclose(output.log.final.Earth.LongA, 6.0858921788484137)
    assert np.isclose(output.log.final.Earth.VelZSpiNBody, 1751.8263488484312802)
    assert np.isclose(output.log.final.Mars.SemiMajorAxis,  2.2794298912149789e+11)
    assert np.isclose(output.log.final.Mars.PositionYSpiNBody, -1.9293056341095392e+11)


if __name__ == "__main__":
    test_SS_NBody()
