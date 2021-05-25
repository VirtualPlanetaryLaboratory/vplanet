import vplanet
import vplot
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pathlib
import sys
import re
import astropy.units as u
import glob

# Path hacks
path = pathlib.Path(__file__).parents[0].absolute()
sys.path.insert(1, str(path.parents[0]))
from get_args import get_args


def clim_evol(plname, xrange=False, show=True):
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
    show : bool
      Show plot in Python (default = True)

    """
    fig = plt.figure(figsize=(6.5, 9))
    fig.subplots_adjust(wspace=0.5, hspace=0.5)

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

    f = open(path / f"{plname}.in", "r")
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
        longp = (body.ArgP + body.LongA + body.PrecA) * np.pi / 180.0
    except:
        longp = body.PrecA * np.pi / 180.0

    esinv = ecc * np.sin(longp) * np.sin(obl * np.pi / 180.0)

    lats = np.unique(body.Latitude)
    nlats = len(lats)
    ntimes = len(body.Time)

    # plot temperature
    temp = np.reshape(body.TempLat, (ntimes, nlats))
    ax1 = plt.subplot(5, 1, 1)

    c = plt.contourf(body.Time, lats, temp.T, cmap="plasma")
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.title(r"Surface Temp [$^{\circ}$C]", fontsize=12)
    plt.ylim(-90, 90)
    plt.yticks([-60, -30, 0, 30, 60], fontsize=9)
    plt.xticks(fontsize=9)
    if xrange == False:
        left = 0
    else:
        left = xrange[0]
    if xrange:
        plt.xlim(xrange)
    cbar = plt.colorbar(c, ax=ax1)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    # plot albedo
    alb = np.reshape(body.AlbedoLat, (ntimes, nlats))
    ax2 = plt.subplot(5, 1, 3)
    c = plt.contourf(body.Time, lats, alb.T, cmap="Blues_r")
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.title("Albedo [TOA]", fontsize=12)
    plt.ylim(-90, 90)
    plt.yticks([-60, -30, 0, 30, 60], fontsize=9)
    plt.xticks(fontsize=9)
    if xrange:
        plt.xlim(xrange)
    cbar = plt.colorbar(c, ax=ax2)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    # plot ice height
    ice = np.reshape(body.IceHeight, (ntimes, nlats))
    ax3 = plt.subplot(5, 1, 4)
    c = plt.contourf(body.Time, lats, ice.T, cmap="Blues_r")
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.title("Ice sheet height [m]", fontsize=12)
    plt.ylim(-90, 90)
    plt.yticks([-60, -30, 0, 30, 60], fontsize=9)
    plt.xticks(fontsize=9)
    if xrange:
        plt.xlim(xrange)
    cbar = plt.colorbar(c, ax=ax3)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    # plot bedrock
    brock = np.reshape(body.BedrockH, (ntimes, nlats))
    ax4 = plt.subplot(5, 1, 5)
    c = plt.contourf(body.Time, lats, brock.T, cmap="Reds_r")
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.title("Bedrock height [m]", fontsize=12)
    plt.ylim(-90, 90)
    plt.yticks([-60, -30, 0, 30, 60], fontsize=9)
    plt.xlabel("Time [years]", fontsize=10)
    plt.xticks(fontsize=9)
    if xrange:
        plt.xlim(xrange)
    cbar = plt.colorbar(c, ax=ax4)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    # plot insolation
    insol = np.reshape(body.AnnInsol, (ntimes, nlats))
    ax5 = plt.subplot(5, 1, 2)
    c = plt.contourf(body.Time, lats, insol.T, cmap="plasma")
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.title(r"Annual average insolation [W/m$^2$]", fontsize=12)
    plt.ylim(-90, 90)
    plt.yticks([-60, -30, 0, 30, 60], fontsize=9)
    plt.xticks(fontsize=9)
    if xrange:
        plt.xlim(xrange)
    cbar = plt.colorbar(c, ax=ax5)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    # Save
    ext = get_args().ext
    plt.savefig(path / f"IceBeltClimateEvol.{ext}", dpi=300)

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
            check = 1

    if check == 0:
        raise StandardError("Climate data not found for time %f" % time)

    insol = np.loadtxt(insolf, unpack=True)
    temp = np.loadtxt(tempf, unpack=True)
    ice = np.loadtxt(icef, unpack=True)
    output = vplanet.run(path / "vpl.in")

    ctmp = 0
    for p in range(len(output.bodies)):
        if output.bodies[p].name == plname:
            body = output.bodies[p]
            ctmp = 1
        else:
            if p == len(output.bodies) - 1 and ctmp == 0:
                raise Exception("Planet %s not found" % plname)

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

    fig = plt.figure(figsize=(9, 3.5))
    plt.subplot(1, 3, 1)
    fig.subplots_adjust(wspace=0.5)

    plt.title(r"Insolation [W/m$^2$]", fontsize=12)
    c1 = plt.contourf(np.arange(np.shape(insol)[1]), lats, insol, cmap="plasma")
    cbar = plt.colorbar(c1)
    plt.ylim(lats[0], lats[-1])
    plt.xlabel("Time [days]", fontsize=10)
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.xticks(fontsize=9)
    plt.yticks(fontsize=9)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    scale = 4 * np.shape(insol)[1] / np.shape(temp)[1]
    plt.subplot(1, 3, 2)
    c2 = plt.contourf(np.arange(np.shape(temp)[1]) * scale, lats, temp, cmap="plasma")
    plt.title(r"Surface Temp [$^{\circ}$C]", fontsize=12)
    cbar = plt.colorbar(c2)
    plt.ylim(lats[0], lats[-1])
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.xlabel("Time [days]", fontsize=10)
    plt.xlim(0, np.shape(temp)[1] * scale / 4.0)
    plt.xticks(fontsize=9)
    plt.yticks(fontsize=9)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)

    scale = np.shape(insol)[1] / np.shape(ice)[1]
    plt.subplot(1, 3, 3)
    c3 = plt.contourf(np.arange(np.shape(ice)[1]) * scale, lats, ice, cmap="Blues_r")
    plt.title(r"Ice balance [kg/m$^2$/s]", fontsize=12)
    cbar = plt.colorbar(c3)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize=9)
    plt.ylim(lats[0], lats[-1])
    plt.xlabel("Time [days]", fontsize=10)
    plt.ylabel(r"Latitude [$^\circ$]", fontsize=10)
    plt.xticks(fontsize=9)
    plt.yticks(fontsize=9)

    # Save
    ext = get_args().ext
    fig.savefig(path / f"IceBeltSeasonal.{ext}", dpi=300)

    if show:
        plt.show()
    else:
        plt.close()


# makes the plots
print("Making evolution plot.")
clim_evol("earth", show=False)
print("Making seasonal plot.")
seasonal_maps(0, show=False)
