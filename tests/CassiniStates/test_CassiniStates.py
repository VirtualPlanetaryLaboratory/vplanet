from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_CassiniStates():
    """Test the coupling between eqtide, distorb and distrot."""
    # Remove old log file
    subprocess.run(['rm', 'CassiniStates.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    # Primary Variables
    assert np.isclose(output.log.final.b.Xobl, -0.706488)
    assert np.isclose(output.log.final.b.Yobl, 0.046974)
    assert np.isclose(output.log.final.b.Zobl, 0.706164)
    assert np.isclose(output.log.final.b.HEcc, -0.062959)
    assert np.isclose(output.log.final.b.KEcc, 0.010774)
    assert np.isclose(output.log.final.b.RotRate, 7.270645e-05)
    assert np.isclose(output.log.final.b.SemiMajorAxis, 1.869973e+10)

    # Other checks
    assert np.isclose(output.log.final.b.Inc, 0.008680)
    assert np.isclose(output.log.final.b.ArgP, 4.733145)
    assert np.isclose(output.log.final.b.LongA, 0.148731)
    assert np.isclose(output.log.final.b.Obliquity, 0.786733)
    assert np.isclose(output.log.final.b.PrecA, 3.075203)
    assert np.isclose(output.log.final.b.RotAngMom, 4.415998e+33)
    assert np.isclose(output.log.final.b.RotKinEnergy, 1.605358e+29)
    assert np.isclose(output.log.final.c.Inc, 8.069275e-05)
    assert np.isclose(output.log.final.c.ArgP, 4.653590)
    assert np.isclose(output.log.final.c.LongA, 1.580510)


if __name__ == "__main__":
    test_CassiniStates()
