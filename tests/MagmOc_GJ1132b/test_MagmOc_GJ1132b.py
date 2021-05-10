from vplot import GetOutput
import subprocess
import numpy as np
import os

cwd = os.path.dirname(os.path.realpath(__file__))


def test_MagmOcGJ1132b():
    """Test magma ocean evolution of GJ1132b."""
    # Remove old log file
    subprocess.run(["rm", "GJ1132.log"], cwd=cwd)
    # Run vplanet
    subprocess.run(["vplanet", "vpl.in", "-q"], cwd=cwd)

    # Grab the output
    output = GetOutput(path=cwd)

    # Primary Variables: GJ1132b -- checks MagmOc parameters
    assert np.isclose(output.log.final.GJ1132b.PotTemp, 3358.229436)
    assert np.isclose(output.log.final.GJ1132b.SurfTemp, 4000.000000)
    assert np.isclose(output.log.final.GJ1132b.SolidRadius, 0.750604)
    assert np.isclose(output.log.final.GJ1132b.WaterMassMOAtm, 99.456333)
    assert np.isclose(output.log.final.GJ1132b.WaterMassSol, 0.174876)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassMOAtm, 3.946430e18)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassSol, 4.449172e17)
    assert np.isclose(output.log.final.GJ1132b.HydrogenMassSpace, 5.736131e19)
    assert np.isclose(output.log.final.GJ1132b.OxygenMassSpace, 4.508558e20)
    assert np.isclose(output.log.final.GJ1132b.CO2MassMOAtm, 1.748940e-296)
    assert np.isclose(output.log.final.GJ1132b.CO2MassSol, 1.748940e-296)


if __name__ == "__main__":
    test_MagmOcGJ1132b()
