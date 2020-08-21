from vplot import GetOutput
import subprocess
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_MagmOcGJ1132b():
    """Test magma ocean evolution of GJ1132b."""
    # Remove old log file
    subprocess.run(['rm', 'GJ1132.log'], cwd=cwd)
    # Run vplanet
    subprocess.run(['vplanet', 'vpl.in', '-q'], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Primary Variables: GJ1132b -- checks MagmOc parameters
    assert np.isclose(output.log.final.GJ1132b.PotTemp, 3546.506520)
    assert np.isclose(output.log.final.GJ1132b.SurfTemp, 3544.050059)
    assert np.isclose(output.log.final.GJ1132b.SolidRadius, 0.671686)
    assert np.isclose(output.log.final.GJ1132b.WaterMassMOAtm, 99.569302)
    assert np.isclose(output.log.final.GJ1132b.WaterMassSol, 0.061907)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassMOAtm, 4.187362e+18)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassSol, 2.039860e+17)
    assert np.isclose(output.log.final.GJ1132b.HydrogenMassSpace, 5.736134e+19)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassSpace, 4.508560e+20)
    assert np.isclose(output.log.final.GJ1132b.CO2MassMOAtm, 1.749861e-296)
    assert np.isclose(output.log.final.GJ1132b.CO2MassSol, 1.749861e-296)


if __name__ == "__main__":
    test_MagmOcGJ1132b()
