"""
Verify the CPL constituent decomposition for a SYNCHRONOUS rotator.

The Earth-Sun case (tests/EqtideConstituents) exercises the fast-rotator
limit, where the six constituents separate into distinct Doodson lines. This
is the opposite limit, Enceladus at Saturn, where Omega = n collapses them.

Three properties are checked, none of which depends on the tuning:

  1. The semi-diurnal frequency is exactly zero (the permanent bulge) and the
     other five sit exactly at the orbital frequency.

  2. The heating splits between the eccentricity and radial constituents in
     the exact rational proportions 7/8, 3/28 and 1/56, which fall out of the
     CPL weights once Omega = n and the obliquity vanishes.

  3. The total power matches the textbook closed form for a synchronous
     satellite, computed here independently of VPLanet:

         Edot = (21/2) (k2/Q) G M_p^2 R^5 n e^2 / a^6
"""

import pathlib
import subprocess

import numpy as np
import pytest

import vplanet

BIGG = 6.67428e-11  # Must match BIGG in src/vplanet.h

CONSTITUENTS = ["SemiDiurn", "EccPlus", "EccMinus", "Radial", "OblDiurn", "OblSid"]

# Exact shares of the tidal power at Omega = n and zero obliquity.
# From the CPL weights: eps_1 carries 24.5 e^2, eps_5 carries 3 e^2 and
# eps_2 carries 0.5 e^2, out of 28 e^2 total.
EXACT_SHARE = {
    "EccPlus": 24.5 / 28,
    "Radial": 3.0 / 28,
    "EccMinus": 0.5 / 28,
}

path = pathlib.Path(__file__).parents[0].absolute()


@pytest.fixture(scope="module")
def enceladus():
    """Initial-state log entries for Enceladus.

    The log is used rather than the forward file because it carries full
    precision and every logged quantity, not just the requested columns.
    """
    subprocess.check_output(["vplanet", "vpl.in"], cwd=str(path))
    return vplanet.get_output(path, units=False).log.initial.enceladus


@pytest.fixture(scope="module")
def saturn():
    return vplanet.get_output(path, units=False).log.initial.saturn


def test_synchronous_spin(enceladus):
    """bForceEqSpin with bDiscreteRot must give exactly Omega = n."""
    assert float(enceladus.RotRate) == pytest.approx(
        float(enceladus.MeanMotion), rel=1e-12
    )


def test_semidiurnal_frequency_vanishes(enceladus):
    """Omega = n sends 2*Omega - 2n to zero: the permanent tide."""
    assert float(enceladus.TidalFreqSemiDiurn) == 0.0


def test_constituents_collapse_to_orbital_frequency(enceladus):
    """Every time-variable constituent must sit at the mean motion."""
    dMeanMotion = float(enceladus.MeanMotion)
    for sConst in ["EccPlus", "EccMinus", "Radial", "OblDiurn", "OblSid"]:
        dFreq = float(getattr(enceladus, "TidalFreq" + sConst))
        assert dFreq == pytest.approx(dMeanMotion, rel=1e-9), sConst


def test_permanent_tide_does_no_work(enceladus):
    """A zero-frequency constituent has zero phase lag, so it cannot heat."""
    assert float(enceladus.TidalPowerSemiDiurn) == 0.0


def test_heating_shares_are_exact_rationals(enceladus):
    """The power partition must hit the analytic CPL fractions."""
    dTotal = float(enceladus.PowerEqtide)
    for sConst, dExpected in EXACT_SHARE.items():
        dShare = float(getattr(enceladus, "TidalPower" + sConst)) / dTotal
        assert dShare == pytest.approx(dExpected, rel=1e-6), sConst


def test_total_matches_closed_form(enceladus, saturn):
    """Compare against the standard synchronous tidal-heating formula.

    This is computed from the physical parameters alone and shares no code
    with fdCPLTidePower or the constituent routines.
    """
    dK2OverQ = float(enceladus.K2) / float(enceladus.TidalQ)

    dClosedForm = (
        10.5
        * dK2OverQ
        * BIGG
        * float(saturn.Mass) ** 2
        * float(enceladus.Radius) ** 5
        * float(enceladus.MeanMotion)
        * float(enceladus.Eccentricity) ** 2
        / float(enceladus.SemiMajorAxis) ** 6
    )

    # Both in Watts. The closed form drops the O(e^4) and obliquity terms
    # that EQTIDE retains, so the agreement is not exact at machine level.
    assert float(enceladus.PowerEqtide) == pytest.approx(dClosedForm, rel=1e-3)
