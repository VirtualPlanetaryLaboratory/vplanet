"""
This script compares stellar spin-down and XUV evolution with and without
weakened magnetic braking.
Rory Barnes, University of Washington, 2026
"""

import pathlib
import sys

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

import vplanet

path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))

mpl.rcParams["figure.figsize"] = (9, 8)
mpl.rcParams["font.size"] = 12

LSUN_ERGS = 3.846e33
ROCRIT = 1.014
SOLAR_AGE = 4.57
SOLAR_PROT = 25.4

MODELS = [
    ("none", "No WMB", "0.35", "-"),
    ("vansaders", "van Saders cutoff", "#3f7d20", "--"),
    ("metcalfe", "Metcalfe+2025, literal N", "#c1462c", "-"),
    ("metcalfenorm", "Metcalfe+2025, renormalized", "#3b5bbf", "-"),
]

output = vplanet.run(path / "vpl.in", units=False)

fig, axes = plt.subplots(nrows=2, ncols=2, sharex=True)

for sBody, sLabel, sColor, sStyle in MODELS:
    body = getattr(output, sBody)
    dAge = body.Time / 1e9

    axes[0, 0].plot(dAge, body.RotPer, color=sColor, ls=sStyle, label=sLabel)
    axes[0, 1].plot(dAge, body.RossbyNumberNorm, color=sColor, ls=sStyle)
    axes[1, 0].plot(dAge, body.WeakMagBrakingFactor, color=sColor, ls=sStyle)
    axes[1, 1].plot(dAge, body.LXUVStellar * LSUN_ERGS, color=sColor, ls=sStyle)

axes[0, 0].plot(SOLAR_AGE, SOLAR_PROT, "o", color="k", zorder=5)
axes[0, 0].annotate("Sun", (SOLAR_AGE, SOLAR_PROT), xytext=(6, -14),
                    textcoords="offset points")
axes[0, 0].set_ylabel("Rotation Period [d]")
axes[0, 0].legend(loc="upper left", fontsize=9)

axes[0, 1].axhline(ROCRIT, color="k", ls=":", lw=1)
axes[0, 1].annotate(r"Ro$_{\rm crit}$", (0.2, ROCRIT), xytext=(0, 4),
                    textcoords="offset points", fontsize=9)
axes[0, 1].set_ylabel(r"Ro / Ro$_\odot$")

axes[1, 0].set_ylabel(r"Suppression Factor $\Psi$")
axes[1, 0].set_xlabel("Age [Gyr]")

axes[1, 1].set_yscale("log")
axes[1, 1].set_ylabel(r"L$_{\rm XUV}$ [erg s$^{-1}$]")
axes[1, 1].set_xlabel("Age [Gyr]")

for ax in axes.flatten():
    ax.set_xlim(0, 10)

fig.tight_layout()
fig.savefig(path / "WeakMagneticBraking.png", dpi=150, bbox_inches="tight")
