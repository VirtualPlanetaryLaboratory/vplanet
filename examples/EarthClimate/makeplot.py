import glob
import pathlib
import re
import sys

import astropy.units as u
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import vplot
from matplotlib import ticker

import vplanet

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args


def comp2huybers(plname, xrange=False, show=True):
    """
    Creates plots of insolation, temperature, albedo, ice mass,
    and bed rock height over the length of the simulation

    Parameters
    ----------
    plname : string
      The name of the planet with .Climate data

    Keyword Arguments
    -----------------
    xrange : float tuple, list, or numpy array
      Range of x-values (time) to restrict plot
      (default = False (no restriction))
    orbit : bool
      Plot orbital data (obliquity, eccentricity, COPP)
      (default = False)
    show : bool
      Show plot in Python (default = True)

    """
    fig = plt.figure(figsize=(8, 12))
    fig.subplots_adjust(wspace=0.3, top=0.9, hspace=0.2)

    # Run vplanet
    out = vplanet.run(path / "vpl.in")

    ctmp = 0
    for p in range(len(out.bodies)):
        if out.bodies[p].name == plname:
            body = out.bodies[p]
            ctmp = 1
        else:
            if p == len(out.bodies) - 1 and ctmp == 0:
                raise Exception("Planet %s not found." % plname)

    try:
        ecc = body.Eccentricity
    except:
        ecc = np.zeros_like(body.Time) + getattr(out.log.initial, plname).Eccentricity

    try:
        inc = body.Inc
    except:
        inc = np.zeros_like(body.Time)

    try:
        obl = body.Obliquity
    except:
        obltmp = getattr(out.log.initial, plname).Obliquity
        if obltmp.unit == "rad":
            obltmp *= 180 / np.pi
        obl = np.zeros_like(body.Time) + obltmp

    plname_lower = plname.lower()
    f = open(path / f"{plname_lower}.in", "r")
    lines = f.readlines()
    f.close()
    pco2 = 0

    for i in range(len(lines)):
        if lines[i].split() != []:
            if lines[i].split()[0] == "dRotPeriod":
                P = -1 * float(lines[i].split()[1])
            if lines[i].split()[0] == "dSemi":
                semi = float(lines[i].split()[1])
                if semi < 0:
                    semi *= -1
            if lines[i].split()[0] == "dpCO2":
                pco2 = float(lines[i].split()[1])

    try:
        longp = (body.ArgP + body.LongA + body.PrecA + 180)
    except:
        longp = body.PrecA

    esinv = ecc * np.sin(longp)

    lats = np.unique(body.Latitude)
    nlats = len(lats)
    ntimes = len(body.Time)

    # plot temperature
    temp = np.reshape(body.TempLandLat, (ntimes, nlats))
    ax1 = plt.subplot(7, 1, 5)
    c = plt.contourf(
        body.Time / 1e6, lats[lats > 58 * u.deg], temp.T[lats > 58 * u.deg], 20
    )
    plt.ylabel("Latitude")

    plt.ylim(60, 83)
    plt.yticks([60, 70, 80])
    if xrange == False:
        left = 0
    else:
        left = xrange[0]

    if xrange:
        plt.xlim(xrange)

    plt.xticks(visible=False)
    clb = plt.colorbar(
        c,
        ax=plt.gca(),
        ticks=[-17, -15, -13, -11, -9, -7, -5],
    )
    clb.set_label("Surface Temp.\n($^{\circ}$C)", fontsize=12)

    # plot ice height
    ice = np.reshape(body.IceHeight + body.BedrockH, (ntimes, nlats))

    ax3 = plt.subplot(7, 1, 4)

    c = plt.contourf(
        body.Time / 1e6, lats[lats > 58 * u.deg], ice.T[lats > 58 * u.deg, :], 20
    )
    plt.ylabel("Latitude")
    plt.ylim(60, 83)

    plt.yticks([60, 70, 80])
    if xrange:
        plt.xlim(xrange)
    plt.xticks(visible=False)
    clb = plt.colorbar(c, ax=plt.gca())
    clb.set_label("Ice sheet\nheight (m)", fontsize=12)

    ax4 = plt.subplot(7, 1, 6)
    acc = np.reshape(body.IceAccum, (ntimes, nlats))
    c = plt.contourf(
        body.Time / 1e6, lats[lats > 58 * u.deg], acc.T[lats > 58 * u.deg], 20
    )
    plt.ylabel("Latitude")
    plt.ylim(60, 83)
    plt.yticks([60, 70, 80])
    plt.xticks(visible=False)
    if xrange == False:
        left = 0
    else:
        left = xrange[0]

    if xrange:
        plt.xlim(xrange)
    clb = plt.colorbar(
        c,
        ax=plt.gca(),
    )
    tloc = ticker.MaxNLocator(nbins=5)
    clb.locator = tloc
    clb.update_ticks()
    clb.set_label("Ice Accum.\n(m year$^{-1}$)", fontsize=12)

    ax5 = plt.subplot(7, 1, 7)
    abl = np.reshape(body.IceAblate, (ntimes, nlats))
    c = plt.contourf(
        body.Time / 1e6, lats[lats > 58 * u.deg], -abl.T[lats > 58 * u.deg], 20
    )
    plt.ylabel("Latitude")
    plt.ylim(60, 83)
    plt.yticks([60, 70, 80])
    plt.xlabel("Time (Myr)")
    if xrange == False:
        left = 0
    else:
        left = xrange[0]

    if xrange:
        plt.xlim(xrange)
    clb = plt.colorbar(
        c,
        ax=plt.gca(),
    )
    clb.set_label("Ice Ablation\n(m year$^{-1}$)", fontsize=12)

    plt.subplot(7, 1, 1)
    plt.plot(
        body.Time / 1e6,
        obl,
        linestyle="solid",
        marker="None",
        color=vplot.colors.dark_blue,
        linewidth=2,
    )
    plt.ylabel("Obliquity")
    plt.xticks(visible=False)
    if xrange:
        plt.xlim(xrange)

    # Hack to make axis align with the others
    c = plt.contourf([np.nan, np.nan], [np.nan, np.nan], [[1, 0], [0, 1]], alpha=0)
    cbar = plt.colorbar(c, ax=plt.gca())
    cbar.ax.set_visible(False)

    plt.subplot(7, 1, 2)
    plt.plot(
        body.Time / 1e6,
        ecc,
        linestyle="solid",
        marker="None",
        color=vplot.colors.purple,
        linewidth=2,
    )
    plt.ylabel("Eccenticity")
    plt.xticks(visible=False)
    if xrange:
        plt.xlim(xrange)

    # Hack to make axis align with the others
    c = plt.contourf([np.nan, np.nan], [np.nan, np.nan], [[1, 0], [0, 1]], alpha=0)
    cbar = plt.colorbar(c, ax=plt.gca())
    cbar.ax.set_visible(False)

    plt.subplot(7, 1, 3)
    plt.plot(
        body.Time / 1e6,
        esinv,
        linestyle="solid",
        marker="None",
        color=vplot.colors.red,
        linewidth=2,
    )
    plt.ylabel("CPP")
    plt.xticks(visible=False)
    if xrange:
        plt.xlim(xrange)

    # Hack to make axis align with the others
    c = plt.contourf([np.nan, np.nan], [np.nan, np.nan], [[1, 0], [0, 1]], alpha=0)
    cbar = plt.colorbar(c, ax=plt.gca())
    cbar.ax.set_visible(False)

    # Save
    ext = get_args().ext
    plt.savefig(path / f"EarthClimateMilankovitch.{ext}")

    if show:
        plt.show()
    else:
        plt.close()


def seasonal_maps(time, show=True):
    """
    Creates plots of insolation, temperature, and ice balance
    over the course of an orbit (4 orbits for temp)

    Parameters
    ----------
    time : float
      The time of the data you want to plot (see SeasonalClimateFiles directory)

    Keyword Arguments
    -----------------
    show : bool
      Show plot in Python (default = True)

    """
    check = 0
    for f in glob.glob(str(path / "SeasonalClimateFiles" / "*.DailyInsol.*")):
        f1 = f.split(".")

        if len(f1) == 4:
            timestamp = f1[3]
        elif len(f1) == 5:
            timestamp = f1[3] + "." + f1[4]

        time0 = float(timestamp)

        if time0 == time:
            # get system and planet names
            sysname = f1[0]
            plname = f1[1]
            insolf = (
                path
                / "SeasonalClimateFiles"
                / f"{sysname}.{plname}.DailyInsol.{timestamp}"
            )
            tempf = (
                path
                / "SeasonalClimateFiles"
                / f"{sysname}.{plname}.SeasonalTemp.{timestamp}"
            )
            icef = (
                path
                / "SeasonalClimateFiles"
                / f"{sysname}.{plname}.SeasonalIceBalance.{timestamp}"
            )
            planckbf = (
                path
                / "SeasonalClimateFiles"
                / f"{sysname}.{plname}.SeasonalFOut.{timestamp}"
            )
            check = 1

    if check == 0:
        raise StandardError("Climate data not found for time %f" % time)

    insol = np.loadtxt(insolf, unpack=True)
    temp = np.loadtxt(tempf, unpack=True)
    ice = np.loadtxt(icef, unpack=True)
    fluxo = np.loadtxt(planckbf, unpack=True)
    output = vplanet.run(path / "vpl.in")
    ctmp = 0
    for p in range(len(output.bodies)):
        if output.bodies[p].name == plname:
            body = output.bodies[p]
            ctmp = 1
        else:
            if p == len(output.bodies) - 1 and ctmp == 0:
                raise Exception("Planet %s not found." % plname)

    lats = np.unique(body.Latitude)
    try:
        obl = body.Obliquity[np.where(body.Time == time)[0]]
    except:
        obl = getattr(output.log.initial, plname).Obliquity
        if obl.unit == "rad":
            obl *= 180 / np.pi

    try:
        ecc = body.Eccentricity[np.where(body.Time == time)[0]]
    except:
        ecc = getattr(output.log.initial, plname).Eccentricity

    try:
        longp = (body.LongP + body.PrecA)[np.where(body.Time == time)[0]]
    except:
        try:
            longp = getattr(output.log.initial, plname).LongP
        except:
            try:
                longp = (
                    getattr(output.log.initial, plname).LongA
                    + getattr(out.log.initial, plname).ArgP
                )
                if longp.unit == "rad":
                    longp *= 180 / np.pi
                longp = longp % 360
            except:
                longp = 0

    fig = plt.figure(figsize=(8.5, 5))
    fig.subplots_adjust(wspace=0.3, hspace=0.2)
    ax = plt.subplot(2, 2, 1)
    c1 = plt.contourf(np.arange(np.shape(insol)[1]), lats, insol, cmap="plasma")
    plt.ylim(lats[0], lats[-1])
    plt.ylabel("Latitude ($^{\circ}$)")
    plt.yticks([-60, -30, 0, 30, 60], ["60S", "30S", "0", "30N", "60N"])
    clb = plt.colorbar(c1, ax=ax)
    clb.set_label(r"Insolation (W/m$^2$)", fontsize=12)

    scale = 4 * np.shape(insol)[1] / np.shape(temp)[1]
    ax = plt.subplot(2, 2, 2)
    c2 = plt.contourf(np.arange(np.shape(temp)[1]) * scale, lats, temp, cmap="plasma")
    plt.ylim(lats[0], lats[-1])
    plt.xlim(0, np.shape(temp)[1] * scale / 4.0)
    plt.yticks([-60, -30, 0, 30, 60], ["60S", "30S", "0", "30N", "60N"])
    clb = plt.colorbar(c2, ax=ax)
    clb.set_label(r"Surface Temp ($^{\circ}$C)", fontsize=12)

    scale = np.shape(insol)[1] / np.shape(ice)[1]
    ax = plt.subplot(2, 2, 3)
    c3 = plt.contourf(
        np.arange(np.shape(ice)[1]) * scale, lats, ice * 1e3, cmap="Blues_r"
    )
    plt.ylim(lats[0], lats[-1])
    plt.ylabel("Latitude ($^{\circ}$)")
    plt.yticks([-60, -30, 0, 30, 60], ["60S", "30S", "0", "30N", "60N"])
    plt.xlabel("Day of Year")
    clb = plt.colorbar(c3, ax=ax)
    clb.set_label(r"Ice Mass Balance (10$^{-3}$ kg/m$^2$/s)", fontsize=12)

    scale = 4 * np.shape(insol)[1] / np.shape(temp)[1]
    ax = plt.subplot(2, 2, 4)
    c2 = plt.contourf(np.arange(np.shape(fluxo)[1]) * scale, lats, fluxo, cmap="plasma")
    plt.ylim(lats[0], lats[-1])
    plt.xlim(0, np.shape(temp)[1] * scale / 4.0)
    plt.yticks([-60, -30, 0, 30, 60], ["60S", "30S", "0", "30N", "60N"])
    plt.xlabel("Day of Year")
    clb = plt.colorbar(c2, ax=ax)
    clb.set_label(r"OLR (W/m$^{2}$)", fontsize=12)

    # Save
    ext = get_args().ext
    fig.savefig(path / f"EarthClimateSeasons.{ext}", dpi=300)

    if show:
        plt.show()
    else:
        plt.close()


# Make plots
comp2huybers("Earth", show=False)
seasonal_maps(0, show=False)
