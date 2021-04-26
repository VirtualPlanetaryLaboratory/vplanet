from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_CassiniMulti():
    """Test multiple planets damping into Cassini states."""
    # Remove old log file
    subprocess.run(['rm', 'TGard.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    # Star
    assert np.isclose(output.log.final.TGstar.Radius, 95.848284)
    assert np.isclose(output.log.final.TGstar.RadGyra, 0.445942)
    assert np.isclose(output.log.final.TGstar.RotVel, 296.385042)
    assert np.isclose(output.log.final.TGstar.EqRotRate, 1.407733e-05)
    assert np.isclose(output.log.final.TGstar.LXUVStellar, 1.880010e+22)
    # Planet b
    assert np.isclose(output.log.final.TGb.Instellation, 1.058375e+05)
    assert np.isclose(output.log.final.TGb.Yobl, -0.149030)
    assert np.isclose(output.log.final.TGb.Zobl, 0.856552)
    assert np.isclose(output.log.final.TGb.HEcc, 0.091315)
    assert np.isclose(output.log.final.TGb.KEcc, -0.048856)
    assert np.isclose(output.log.final.TGb.RotRate, 6.843648e-05)
    assert np.isclose(output.log.final.TGb.SemiMajorAxis, 3.769868e+09)
    assert np.isclose(output.log.final.TGb.OrbPotEnergy, -1.766030e+34)
    assert np.isclose(output.log.final.TGb.SemiTimeEqtide, 7.942139e+15)
    assert np.isclose(output.log.final.TGb.DXoblDtEqtide, -3.658103e-12)
    # Planet c
    assert np.isclose(output.log.final.TGc.Sinc, 0.037712)
    assert np.isclose(output.log.final.TGc.ArgP, 3.924108)
    assert np.isclose(output.log.final.TGc.PrecA, 2.477072)
    assert np.isclose(output.log.final.TGc.CassiniOne, 0.959339)
    assert np.isclose(output.log.final.TGc.CassiniTwo, 0.282255)

if __name__ == "__main__":
    test_CassiniMulti()
