"""Produce the rotation, XUV, and Rossby figures for the weakened magnetic
braking note from VPLanet forward files."""

import argparse
import pathlib

import matplotlib as mpl
import matplotlib.pyplot as plt

mpl.use("Agg")
mpl.rcParams.update({
    "font.size": 11,
    "axes.labelsize": 12,
    "legend.fontsize": 9.5,
    "figure.dpi": 150,
    "savefig.bbox": "tight",
})

COLUMNS = ["Time", "RotPer", "RoNorm", "Psi", "LXUV", "WindTorque", "Teff",
           "Lum"]
LSUN_ERGS = 3.846e33
ROCRIT = 1.014
SOLAR_AGE = 4.57
SOLAR_PROT = 25.4

# The fiducial is drawn first as a thick pale line so that the WMB tracks,
# which coincide with it before the onset of suppression, remain visible on
# top of it without hiding it.
MODELS = [
    ("none", "Fiducial (no WMB)", "#9aa0a6", "-", 5.0),
    ("vansaders", "van Saders cutoff", "#2e7d32", "--", 1.8),
    ("metcalfe", r"Metcalfe, $N=0.354$", "#c62828", "-.", 1.8),
    ("metcalfenorm", r"Metcalfe, normalized", "#1565c0", "-", 1.8),
]


def fdictReadRuns(sDir, sPrefix):
  """Return {model name: list of column arrays} for every model."""
  dictRuns = {}
  for tModel in MODELS:
    sPath = pathlib.Path(sDir) / ("%s.%s.forward" % (sPrefix, tModel[0]))
    listCols = [[] for _ in COLUMNS]
    with open(sPath) as fp:
      for sLine in fp:
        listVals = sLine.split()
        if len(listVals) == len(COLUMNS):
          for i, sVal in enumerate(listVals):
            listCols[i].append(float(sVal))
    dictRuns[tModel[0]] = listCols
  return dictRuns


def fvStyleAge(ax):
  """Apply the shared age-axis styling."""
  ax.set_xscale("log")
  ax.set_xlim(0.01, 10)
  ax.set_xlabel("Age [Gyr]")


def fvPlotRotation(dictRuns, sOut):
  """Write the rotation period figure."""
  fig, ax = plt.subplots(figsize=(6.2, 4.4))
  for sKey, sLabel, sColor, sStyle, fWidth in MODELS:
    listCols = dictRuns[sKey]
    ax.plot([f / 1e9 for f in listCols[0]], listCols[1],
            color=sColor, ls=sStyle, lw=fWidth, label=sLabel)
  ax.plot(SOLAR_AGE, SOLAR_PROT, "o", color="#f9a825", ms=9,
          markeredgecolor="k", markeredgewidth=0.8, zorder=6, label="Sun")
  fvStyleAge(ax)
  ax.set_ylim(0, 55)
  ax.set_ylabel("Rotation Period [d]")
  ax.legend(loc="upper left", frameon=False)
  fig.savefig(sOut)
  plt.close(fig)


def fvPlotXUV(dictRuns, sOut):
  """Write the XUV luminosity figure."""
  fig, ax = plt.subplots(figsize=(6.2, 4.4))
  for sKey, sLabel, sColor, sStyle, fWidth in MODELS:
    listCols = dictRuns[sKey]
    ax.plot([f / 1e9 for f in listCols[0]],
            [f * LSUN_ERGS for f in listCols[4]],
            color=sColor, ls=sStyle, lw=fWidth, label=sLabel)
  # No solar marker here: the models predict broadband XUV, whereas the
  # measured solar value is X-rays alone, some three orders of magnitude
  # lower. Plotting them together invites a comparison that is not valid.
  fvStyleAge(ax)
  ax.set_yscale("log")
  ax.set_ylabel(r"$L_{\rm XUV}$ [erg s$^{-1}$]")
  ax.legend(loc="upper right", frameon=False)
  fig.savefig(sOut)
  plt.close(fig)


def fvPlotRossby(dictRuns, sOut):
  """Write the Rossby number and suppression factor figure."""
  fig, axes = plt.subplots(nrows=2, figsize=(6.2, 6.4), sharex=True)
  for sKey, sLabel, sColor, sStyle, fWidth in MODELS:
    listCols = dictRuns[sKey]
    listAge = [f / 1e9 for f in listCols[0]]
    axes[0].plot(listAge, listCols[2], color=sColor, ls=sStyle, lw=fWidth,
                 label=sLabel)
    axes[1].plot(listAge, listCols[3], color=sColor, ls=sStyle, lw=fWidth)

  axes[0].axhline(ROCRIT, color="0.4", ls=":", lw=1.2)
  axes[0].annotate(r"Ro$_{\rm crit}$", (0.1, ROCRIT), xytext=(0, -14),
                   textcoords="offset points", fontsize=9, color="0.3")
  axes[0].plot(SOLAR_AGE, 1.0, "o", color="#f9a825", ms=9,
               markeredgecolor="k", markeredgewidth=0.8, zorder=6)
  axes[0].set_ylabel(r"Ro / Ro$_\odot$")
  axes[0].set_ylim(0, 2.0)
  axes[0].legend(loc="upper left", frameon=False)

  axes[1].set_ylabel(r"Suppression Factor $\Psi$")
  axes[1].set_ylim(-0.05, 1.1)
  fvStyleAge(axes[1])
  fig.savefig(sOut)
  plt.close(fig)


def fnMain():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("sRunDir", help="directory holding the .forward files")
  parser.add_argument("--prefix", default="wmb",
                      help="system name prefix of the forward files")
  parser.add_argument("--outdir", default=".", help="where to write figures")
  args = parser.parse_args()

  dictRuns = fdictReadRuns(args.sRunDir, args.prefix)
  sOut = pathlib.Path(args.outdir)
  fvPlotRotation(dictRuns, sOut / "wmbRotation.pdf")
  fvPlotXUV(dictRuns, sOut / "wmbXUV.pdf")
  fvPlotRossby(dictRuns, sOut / "wmbRossby.pdf")
  print("wrote wmbRotation.pdf, wmbXUV.pdf, wmbRossby.pdf to %s" % sOut)


if __name__ == "__main__":
  fnMain()
