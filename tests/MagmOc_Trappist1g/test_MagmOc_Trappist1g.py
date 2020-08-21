from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_MagmOcTrappist1g():
    """Test magma ocean evolution of Trappist-1 g."""
    # Remove old log file
    subprocess.run(['rm', 'Trappist1.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Primary Variables: Trappist-1 g -- checks MagmOc parameters
    assert np.isclose(output.log.final.g.PotTemp, 2441.144927)
    assert np.isclose(output.log.final.g.SurfTemp, 2426.714881)
    assert np.isclose(output.log.final.g.SolidRadius, 0.975285)
    assert np.isclose(output.log.final.g.WaterMassMOAtm, 1.976368)
    assert np.isclose(output.log.final.g.WaterMassSol, 0.021065)
    assert np.isclose(output.log.final.g.OxygenMassMOAtm, 2.211655e+17)
    assert np.isclose(output.log.final.g.OxygenMassSol, 8.785523e+16)
    assert np.isclose(output.log.final.g.HydrogenMassSpace, 3.992771e+17)
    assert np.isclose(output.log.final.g.OxygenMassSpace, 2.859836e+18)
    assert np.isclose(output.log.final.g.CO2MassMOAtm, 1.755087e-297)
    assert np.isclose(output.log.final.g.CO2MassSol, 1.755087e-297)


if __name__ == "__main__":
    test_MagmOcTrappist1g()
