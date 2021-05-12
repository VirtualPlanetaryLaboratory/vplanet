from benchmark import Benchmark, benchmark
import astropy.units as u
import pytest


@benchmark(
    {
        # Make sure the Bondi-limited escape regime has been selected
        "log.initial.bondi.HEscapeRegime": {"value": 5},
        # Ensure Bondi-limited mass loss rate is correct by comparing it to
        # Eqn. 4 from Owen & Wu (2016},": {"value":~1e-2 Earth masses / yr in kg/s
        "bondi.DEnvMassDt": {"index": 0, "value": -1.753603e15, "unit": u.kg / u.s},
        # Ensure final Roche Lobe and Bondi Radius are correct [Earth radii]
        "log.final.bondi.BondiRadius": {"value": 35.824637, "unit": u.Rearth},
        "log.final.bondi.RocheRadius": {"value": 44.88505, "unit": u.Rearth},
        # Ensure final H envelope and planetary mass are correct
        # That is, the H envelope should have been entirely lost!
        "log.final.bondi.EnvelopeMass": {
            "value": 0.0,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="EnvelopeMass has units of `Earth`, which is ambiguous."
            ),
        },
        "log.final.bondi.Mass": {
            "value": 7.0,
            "unit": u.Mearth,
            "marks": pytest.mark.xfail(
                reason="Mass has units of `Earth`, which is ambiguous."
            ),
        },
        # Test RR-limited formalism
        # Make sure RR-limited escape is selected
        "log.initial.rr.HEscapeRegime": {"value": 6},
        # Check mass loss rate [kg/s] - needs to agree w/ Eqn. 13 from Luger+2015
        "rr.DEnvMassDt": {"index": 0, "value": -7.111259e04, "unit": u.kg / u.s},
        # Check critical flux [W/m^2] - needs to agree w/ Eqn. A23 from Luger+2015
        # Note - system isn't actual in RR escape - we set bUseRRLimited to force
        # RR-limited escape since dFXUV < the critical flux
        "log.initial.rr.RRCriticalFlux": {"value": 53.688843, "unit": u.W / u.m ** 2},
    }
)
class TestAtmEscRegimes(Benchmark):
    pass
