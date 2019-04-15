from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_HEC():
    """Test Habitable Evaporated Core scenario."""
    # Remove old log file
    subprocess.run(['rm', 'proxima.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Check
    #assert np.isclose(output.log.final.star.Luminosity, 5.876542e+23)
    assert np.isclose(output.log.final.star.Luminosity, 5.882512e+23)
    assert np.isclose(output.log.final.b.DsemiDtEqtide,  -8.304900e-10)
    assert np.isclose(output.log.final.b.SurfEnFluxTotal, 1.694527)
    assert np.isclose(output.log.final.b.SurfWaterMass, 1.702222, rtol=1e-4)
    assert np.isclose(output.log.final.b.EnvelopeMass, 0)
    assert np.isclose(output.log.final.b.OxygenMass, 472.789158)

if __name__ == "__main__":
    test_HEC()
