#!/usr/bin/env python
"""Compare VPLanet solar-analog runs with and without weakened magnetic braking
at a few diagnostic ages."""

import argparse

COLUMNS = ["Time", "RotPer", "RoNorm", "Psi", "LXUV", "WindTorque", "Teff", "Lum"]
LSUN_ERGS = 3.846e33


def flistReadForward(sPath):
  """Return the forward-file rows as a list of float lists."""
  listRows = []
  with open(sPath) as fp:
    for sLine in fp:
      listVals = sLine.split()
      if len(listVals) == len(COLUMNS):
        listRows.append([float(s) for s in listVals])
  return listRows


def fdRowAtAge(listRows, fAge):
  """Return the row whose Time is closest to fAge."""
  return min(listRows, key=lambda row: abs(row[0] - fAge))


def fsFormatRun(sLabel, listRows, listAges):
  """Return the report block for one run."""
  listOut = ["%s" % sLabel,
             "  %10s %9s %9s %9s %12s %12s" %
             ("age(Gyr)", "Prot(d)", "Ro/Rosun", "Psi", "Lx(erg/s)", "torque")]
  for fAge in listAges:
    listRow = fdRowAtAge(listRows, fAge)
    listOut.append("  %10.2f %9.2f %9.3f %9.4f %12.3e %12.3e" %
                   (listRow[0] / 1e9, listRow[1], listRow[2], listRow[3],
                    listRow[4] * LSUN_ERGS, listRow[5]))
  return "\n".join(listOut)


def fnMain():
  parser = argparse.ArgumentParser(description=__doc__)
  parser.add_argument("saForward", nargs="+",
                      help="vplanet .forward files, one per model")
  parser.add_argument("--out", required=True, help="path for the text report")
  parser.add_argument("--ages", type=float, nargs="+",
                      default=[1e8, 1e9, 4.57e9, 8e9, 1e10],
                      help="ages in years at which to report")
  args = parser.parse_args()

  listBlocks = ["Weakened magnetic braking: solar analog, Baraffe + Matt+2015",
                "Reference: the Sun at 4.57 Gyr has Prot = 25.4 d and "
                "Lx = 2.0e27 erg/s", ""]
  for sPath in args.saForward:
    listBlocks.append(fsFormatRun(sPath, flistReadForward(sPath), args.ages))
    listBlocks.append("")

  sReport = "\n".join(listBlocks)
  with open(args.out, "w") as fp:
    fp.write(sReport + "\n")
  print(sReport)


if __name__ == "__main__":
  fnMain()
