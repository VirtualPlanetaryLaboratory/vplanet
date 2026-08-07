"""Show how EqTide's six CPL tidal constituents behave for a synchronous
satellite, using Enceladus at Saturn, and contrast with the Earth-Sun case."""

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

CONSTITUENTS = ["SemiDiurn", "EccPlus", "EccMinus", "Radial", "OblDiurn", "OblSid"]

BIGG = 6.67428e-11  # Must match BIGG in src/vplanet.h

# Exact shares of the tidal power once Omega = n and the obliquity vanishes.
# The CPL weights leave eps_1 with 24.5 e^2, eps_5 with 3 e^2 and eps_2 with
# 0.5 e^2, out of 28 e^2 total.
EXACT_SHARE = {"EccPlus": "7/8", "Radial": "3/28", "EccMinus": "1/56"}

# Tabulated periods of the Earth-Sun constituents, in hours, for contrast.
# These are external ground truth from the oceanographic tide tables, not
# VPLanet output; examples/SolarConstituents verifies that EqTide reproduces
# them. Radial is the sidereal year rather than Doodson's tropical-year Sa.
EARTH_PERIOD_HR = {
    "SemiDiurn": ("S2", 12.000000),
    "EccPlus": ("T2", 12.016449),
    "EccMinus": ("R2", 11.983596),
    "Radial": ("Sa*", 8766.1527),
    "OblDiurn": ("P1", 24.065890),
    "OblSid": ("K1", 23.934470),
}

# Short labels describing what each constituent is, for the Enceladus panel
ENCELADUS_LABEL = {
    "SemiDiurn": "permanent",
    "EccPlus": "ecc +",
    "EccMinus": "ecc -",
    "Radial": "radial",
    "OblDiurn": "obl 1",
    "OblSid": "obl 2",
}


def fdPeriodHours(dFreqPerDay):
    """Convert an angular frequency in rad/day to a period in hours.

    Returns infinity for a zero frequency, which is the permanent tide.
    """
    if dFreqPerDay <= 0:
        return np.inf
    return 2 * np.pi / dFreqPerDay * 24


def fdFirst(body, sName):
    """First (t=0) value of an output column."""
    return float(np.atleast_1d(getattr(body, sName))[0])


def flistLoadConstituents(output):
    """Extract per-constituent frequency, amplitude and power at t=0."""
    enceladus = output.bodies[1]
    listRows = []
    for sConst in CONSTITUENTS:
        dFreq = fdFirst(enceladus, "TidalFreq" + sConst)
        listRows.append(
            dict(
                key=sConst,
                freq=dFreq,
                period=fdPeriodHours(dFreq),
                amp=fdFirst(enceladus, "TidalAmp" + sConst),
                power=fdFirst(enceladus, "TidalPower" + sConst),
            )
        )
    return listRows


def fvPrintTable(listRows, dTotalPower, dOrbPerHr):
    """Print the constituent table and the power partition."""
    print("\nEqTide CPL constituents for Enceladus at Saturn")
    print("=" * 78)
    print(
        f"{'':10s} {'':10s} {'period (h)':>12s} {'/ orbit':>9s} "
        f"{'amp (m)':>12s} {'power (GW)':>12s}"
    )
    print("-" * 78)
    for r in listRows:
        sPeriod = "infinite" if np.isinf(r["period"]) else f"{r['period']:12.4f}"
        dRatio = np.nan if np.isinf(r["period"]) else r["period"] / dOrbPerHr
        sRatio = "  --" if np.isnan(dRatio) else f"{dRatio:9.6f}"
        print(
            f"{r['key']:10s} {ENCELADUS_LABEL[r['key']]:10s} {sPeriod:>12s} "
            f"{sRatio:>9s} {r['amp']:12.4f} {r['power'] * 1e3:12.6f}"
        )
    print("-" * 78)
    print(f"{'orbital period':21s} {dOrbPerHr:12.4f} h")
    print(f"{'total power':21s} {dTotalPower * 1e3:12.6f} GW")
    dSum = sum(r["power"] for r in listRows)
    print(f"{'sum of constituents':21s} {dSum * 1e3:12.6f} GW")

    print("\nShare of tidal heating          exact CPL fraction at Omega = n")
    print("-" * 78)
    for r in sorted(listRows, key=lambda x: -x["power"]):
        if dTotalPower > 0:
            sExact = EXACT_SHARE.get(r["key"], "")
            print(
                f"{ENCELADUS_LABEL[r['key']]:12s} {r['power'] / dTotalPower:9.4%}"
                f"          {sExact}"
            )


def fvPrintClosedForm(output):
    """Check the total against the textbook synchronous heating formula.

    Edot = (21/2) (k2/Q) G M_p^2 R^5 n e^2 / a^6, computed here from the
    physical parameters alone, independently of fdCPLTidePower.
    """
    saturn = output.log.initial.saturn
    enceladus = output.log.initial.enceladus

    dClosedForm = (
        10.5
        * (float(enceladus.K2) / float(enceladus.TidalQ))
        * BIGG
        * float(saturn.Mass) ** 2
        * float(enceladus.Radius) ** 5
        * float(enceladus.MeanMotion)
        * float(enceladus.Eccentricity) ** 2
        / float(enceladus.SemiMajorAxis) ** 6
    )
    # The log echoes the negative-option unit, so PowerEqtide is in TW here
    dTotal = float(enceladus.PowerEqtide) * 1e12

    print("\nTotal against the closed-form synchronous result")
    print("-" * 78)
    print(f"{'EqTide':22s} {dTotal / 1e9:12.6f} GW")
    print(f"{'(21/2)(k2/Q)GM^2R^5ne^2/a^6':22s} {dClosedForm / 1e9:12.6f} GW")
    print(f"{'relative difference':22s} {(dTotal - dClosedForm) / dClosedForm:12.2e}")


def fvPrintLoveNumbers(output):
    """Report the Love numbers, with the subsurface-ocean caveat attached."""
    enceladus = output.log.initial.enceladus

    print("\nLove numbers")
    print("-" * 78)
    print(f"{'k_2':16s} {float(enceladus.K2):10.6f}")
    print(f"{'h_2':16s} {float(enceladus.H2):10.6f}   (unset: default (5/3)k_2)")
    print(f"{'1 + k_2 - h_2':16s} {float(enceladus.TidalDiminish):10.6f}")
    print(
        "\nThe diminishing factor is printed for completeness only. It converts\n"
        "a raising tide into the tide of a SURFACE ocean, measured against the\n"
        "solid surface beneath it. Enceladus' ocean is beneath its ice shell,\n"
        "so this is not the reduction that applies there, and h_2 has in any\n"
        "case never been measured for Enceladus. Neither number affects the\n"
        "heating above, which is fixed by k_2/Q alone."
    )


def flistGroupByPeriod(dictPeriods, dTolerance=0.02):
    """Collapse constituents whose periods agree to within dTolerance.

    Earth's semi-diurnal lines sit within 0.03 h of each other, so labelling
    them individually produces unreadable overlapping text. Grouping them
    reproduces how the tide tables present species.
    """
    listGroups = []
    for sName, dPeriod in sorted(dictPeriods.items(), key=lambda kv: kv[1]):
        for group in listGroups:
            if abs(dPeriod - group["period"]) / group["period"] < dTolerance:
                group["names"].append(sName)
                break
        else:
            listGroups.append(dict(period=dPeriod, names=[sName]))
    return listGroups


def fvPlotSpectra(ax, listRows, dOrbPerHr):
    """Contrast the Earth-Sun and Enceladus-Saturn constituent periods."""
    dictRows = {r["key"]: r for r in listRows}

    # Earth-Sun: six constituents in three well-separated species
    dictEarth = {EARTH_PERIOD_HR[k][0]: EARTH_PERIOD_HR[k][1] for k in CONSTITUENTS}
    for group in flistGroupByPeriod(dictEarth):
        ax.plot(group["period"], 1, "o", ms=9, color=vplot.colors.dark_blue,
                zorder=3)
        ax.annotate(
            " ".join(group["names"]),
            (group["period"], 1),
            textcoords="offset points",
            xytext=(0, 13),
            ha="center",
            fontsize=9,
        )

    # Enceladus-Saturn: every time-variable constituent at the orbital period
    dictEnc = {
        ENCELADUS_LABEL[r["key"]]: r["period"]
        for r in listRows
        if not np.isinf(r["period"])
    }
    for group in flistGroupByPeriod(dictEnc):
        ax.plot(group["period"], 0, "o", ms=9, color=vplot.colors.orange, zorder=3)
        # Wrap the name list so it does not run into the neighbouring text
        listNames = group["names"]
        sLabel = "\n".join(
            [", ".join(listNames[:3]), ", ".join(listNames[3:])]
        ).strip(", \n")
        ax.annotate(
            sLabel,
            (group["period"], 0),
            textcoords="offset points",
            xytext=(0, 13),
            ha="center",
            fontsize=9,
        )

    ax.annotate(
        "all five time-variable constituents coincide\n"
        "at the orbital period; the semi-diurnal line\n"
        "goes to zero frequency (permanent bulge,\n"
        "no dissipation)",
        (dOrbPerHr, 0),
        textcoords="offset points",
        xytext=(55, -55),
        fontsize=9,
        color="0.3",
        arrowprops=dict(arrowstyle="->", lw=1, color="0.5",
                        connectionstyle="arc3,rad=0.2"),
    )

    ax.set_xscale("log")
    ax.set_xlim(8, 3e4)
    ax.set_ylim(-0.85, 1.6)
    ax.set_yticks([0, 1])
    ax.set_yticklabels(["Enceladus\n(synchronous)", "Earth\n(fast rotator)"])
    ax.set_xlabel("Constituent period (hours)")
    ax.set_title("Where the six CPL constituents sit")
    ax.grid(axis="x", ls="dotted", alpha=0.5)


def fvPlotPower(ax, listRows):
    """Bar chart of the tidal heating carried by each constituent."""
    listActive = [r for r in listRows if r["power"] > 0]
    listActive.sort(key=lambda r: r["power"])
    listY = np.arange(len(listActive))
    ax.barh(
        listY,
        [r["power"] * 1e3 for r in listActive],
        color=vplot.colors.orange,
        height=0.6,
    )
    ax.set_yticks(listY)
    ax.set_yticklabels([ENCELADUS_LABEL[r["key"]] for r in listActive])
    ax.set_xscale("log")
    ax.set_xlabel("Tidal power (GW)")
    ax.set_title("Which constituent heats Enceladus")
    ax.grid(axis="x", ls="dotted", alpha=0.5)


def fvPlot(listRows, dOrbPerHr, sExt):
    fig, axes = plt.subplots(1, 2, figsize=(13, 4.6),
                             gridspec_kw={"width_ratios": [1.7, 1]})
    fvPlotSpectra(axes[0], listRows, dOrbPerHr)
    fvPlotPower(axes[1], listRows)
    fig.tight_layout()
    fig.savefig(path / f"EnceladusTides.{sExt}", bbox_inches="tight", dpi=300)


args = get_args()

if not (path / "enceladus.enceladus.forward").exists() or args.rerun:
    subprocess.check_output(["vplanet", "vpl.in"], cwd=str(path))

output = vplanet.get_output(path, units=False)
listRows = flistLoadConstituents(output)
enceladus = output.bodies[1]
dTotalPower = fdFirst(enceladus, "PowerEqtide")
dOrbPerHr = fdFirst(enceladus, "OrbPeriod") * 24

if not args.quiet:
    fvPrintTable(listRows, dTotalPower, dOrbPerHr)
    fvPrintClosedForm(output)
    fvPrintLoveNumbers(output)

fvPlot(listRows, dOrbPerHr, args.ext)
