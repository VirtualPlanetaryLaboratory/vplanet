"""Compare EQTIDE's CPL tidal constituents to the solar terms of the classical
oceanographic tide tables, and plot the resulting constituent spectrum."""

import pathlib
import subprocess
import sys

import matplotlib.pyplot as plt
import numpy as np
import vplot

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args

# Reference values for the solar constituents.
#
# Periods are exact by construction (they are ratios of the sidereal day and
# the year). Relative amplitudes are from the Cartwright & Tayler (1971) /
# Cartwright & Edden (1973) harmonic expansion of the tide-generating
# potential, normalized within each species: the tables carry a different
# geodetic factor per species, so semi-diurnal amplitudes may be compared only
# against semi-diurnal ones and diurnal against diurnal.
#
# "ct" is the tabulated Cartwright-Tayler amplitude; "ref" names the
# constituent that its species is normalized to.
REFERENCE = {
    "SemiDiurn": dict(name="S2", period_hr=12.000000, ct=0.42286, ref="S2"),
    "EccPlus": dict(name="T2", period_hr=12.016449, ct=0.02472, ref="S2"),
    "EccMinus": dict(name="R2", period_hr=11.983596, ct=0.00354, ref="S2"),
    "OblDiurn": dict(name="P1", period_hr=24.065890, ct=0.17543, ref="P1"),
    "OblSid": dict(name="K1", period_hr=23.934470, ct=0.16817, ref="P1"),
    # Sa is a loose identification, listed for completeness only. EQTIDE puts
    # this line at the Keplerian mean motion (the sidereal year, 365.256363 d)
    # while Doodson's Sa is defined on the tropical year; the two differ by
    # the precession of the equinoxes, which EQTIDE does not model. The
    # observed Sa is also dominated by radiational rather than gravitational
    # forcing, so its tabulated amplitude is not a test of this expansion.
    "Radial": dict(name="Sa*", period_hr=8766.1527, ct=None, ref=None),
}

CONSTITUENTS = ["SemiDiurn", "EccPlus", "EccMinus", "OblDiurn", "OblSid", "Radial"]


def fdPeriodHours(dFreqPerDay):
    """Convert an angular frequency in rad/day to a period in hours."""
    return 2 * np.pi / dFreqPerDay * 24


def flistLoadConstituents(output):
    """Extract the per-constituent frequency, amplitude and power at t=0."""
    earth = output.bodies[1]
    listRows = []
    for sConst in CONSTITUENTS:
        dFreq = np.atleast_1d(getattr(earth, "TidalFreq" + sConst))[0]
        dAmp = np.atleast_1d(getattr(earth, "TidalAmp" + sConst))[0]
        dPower = np.atleast_1d(getattr(earth, "TidalPower" + sConst))[0]
        listRows.append(
            dict(
                key=sConst,
                freq=float(dFreq),
                period=fdPeriodHours(float(dFreq)),
                amp=float(dAmp),
                power=float(dPower),
            )
        )
    return listRows


def fvPrintValidation(listRows, dTotalPower):
    """Print the constituent table alongside the reference values."""
    dictRows = {r["key"]: r for r in listRows}

    print("\nEQTIDE CPL constituents for the Earth-Sun system")
    print("=" * 78)
    print(
        f"{'':10s} {'Doodson':8s} {'period (h)':>12s} {'reference':>12s} "
        f"{'err':>9s} {'amp (m)':>11s}"
    )
    print("-" * 78)
    for sConst in CONSTITUENTS:
        r = dictRows[sConst]
        ref = REFERENCE[sConst]
        dErr = (r["period"] - ref["period_hr"]) / ref["period_hr"]
        print(
            f"{sConst:10s} {ref['name']:8s} {r['period']:12.6f} "
            f"{ref['period_hr']:12.6f} {dErr:9.2e} {r['amp']:11.6f}"
        )

    print("\nAmplitude ratios within each species (cross-species ratios are")
    print("not meaningful: the tide tables normalize each species separately)")
    print("-" * 78)
    print(f"{'ratio':14s} {'EQTIDE':>12s} {'Cartwright-Tayler':>20s} {'err':>10s}")
    for sConst in CONSTITUENTS:
        ref = REFERENCE[sConst]
        if ref["ct"] is None or ref["name"] == ref["ref"]:
            continue
        sRefKey = [k for k in CONSTITUENTS if REFERENCE[k]["name"] == ref["ref"]][0]
        dModel = dictRows[sConst]["amp"] / dictRows[sRefKey]["amp"]
        dObs = ref["ct"] / REFERENCE[sRefKey]["ct"]
        print(
            f"{ref['name'] + '/' + ref['ref']:14s} {dModel:12.6f} "
            f"{dObs:20.6f} {(dModel - dObs) / dObs:10.2e}"
        )

    dSum = sum(r["power"] for r in listRows)
    print("\nPower budget (TW)")
    print("-" * 78)
    for sConst in CONSTITUENTS:
        r = dictRows[sConst]
        print(f"{REFERENCE[sConst]['name']:8s} {r['power']:14.8f}")
    print(f"{'sum':8s} {dSum:14.8f}")
    print(f"{'PowerEqtide':8s} {dTotalPower:14.8f}")
    print(f"{'residual':8s} {dSum - dTotalPower:14.2e}")


def fvPlot(listRows, sExt):
    """Plot the constituent spectrum: amplitude against period."""
    fig, axes = plt.subplots(1, 2, figsize=(11, 4.5))

    listSemi = [r for r in listRows if r["key"] in ("SemiDiurn", "EccPlus", "EccMinus")]
    listDiurn = [r for r in listRows if r["key"] in ("OblDiurn", "OblSid")]

    # The semi-diurnal band spans two decades, so it reads best on a log
    # axis; the diurnal band holds two nearly equal lines, which a log axis
    # would flatten into one.
    for ax, listBand, sTitle, listXlim, bLog in (
        (axes[0], listSemi, "Semi-diurnal band", (11.96, 12.04), True),
        (axes[1], listDiurn, "Diurnal band", (23.85, 24.15), False),
    ):
        dMax = max(r["amp"] for r in listBand)
        dBase = 1e-3 * dMax if bLog else 0
        for r in listBand:
            ref = REFERENCE[r["key"]]
            ax.plot([r["period"], r["period"]], [dBase, r["amp"]], lw=2.5,
                    color=vplot.colors.dark_blue)
            ax.plot(r["period"], r["amp"], "o", color=vplot.colors.dark_blue)
            ax.annotate(
                ref["name"],
                (r["period"], r["amp"]),
                textcoords="offset points",
                xytext=(6, 4),
            )
            ax.axvline(ref["period_hr"], ls="dashed", lw=1,
                       color=vplot.colors.orange)
        if bLog:
            ax.set_yscale("log")
            ax.set_ylim(dBase, 3 * dMax)
        else:
            ax.set_ylim(0, 1.25 * dMax)
        ax.set_xlim(*listXlim)
        ax.set_xlabel("Period (hours)")
        ax.set_ylabel("Equilibrium tide amplitude (m)")
        ax.set_title(sTitle)

    axes[0].plot([], [], ls="dashed", color=vplot.colors.orange,
                 label="Tabulated period")
    axes[0].legend(loc="lower right", fontsize=9)

    fig.tight_layout()
    fig.savefig(path / f"SolarConstituents.{sExt}", bbox_inches="tight", dpi=300)


args = get_args()

if not (path / "solarconst.earth.forward").exists() or args.rerun:
    subprocess.check_output(["vplanet", "vpl.in"], cwd=str(path))

output = vplanet.get_output(path, units=False)
listRows = flistLoadConstituents(output)
dTotalPower = float(np.atleast_1d(output.bodies[1].PowerEqtide)[0])

if not args.quiet:
    fvPrintValidation(listRows, dTotalPower)

fvPlot(listRows, args.ext)
