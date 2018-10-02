from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_IoHeat():
    """Test module eqtide, and simulations with no integrations."""
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Run our comparisons
    assert np.isclose(output.log.initial.io.PowerEqtide, 9.380954e+13)
    assert np.isclose(output.log.initial.io.SurfEnFluxEqtide, 2.243481)
    assert np.isclose(output.log.initial.io.DsemiDtEqtide, -1.671102e-09)
    assert np.isclose(output.log.initial.io.DeccDtEqtide, -8.503575e-16)
    assert np.isclose(output.log.initial.io.DOblDtEqtide, -3.792877e-13)

if __name__ == "__main__":
    test_IoHeat()
