import numpy as np
import matplotlib.pyplot as plt
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')
from matplotlib import ticker
import os
import subprocess
import re
import sys

def clim_evol(plname,dir='.',xrange=False,orbit=False,show=True):
  """
  Creates plots of insolation, temperature, albedo, ice mass,
  and bed rock height over the length of the simulation

  Parameters
  ----------
  plname : string
    The name of the planet with .Climate data

  Keyword Arguments
  -----------------
  dir : string
    Directory of vplanet simulation (default = '.')
  xrange : float tuple, list, or numpy array
    Range of x-values (time) to restrict plot
    (default = False (no restriction))
  orbit : bool
    Plot orbital data (obliquity, eccentricity, COPP)
    (default = False)
  show : bool
    Show plot in Python (default = True)

  Output
  ------
  PDF format plot with name 'evol_<dir>.pdf'

  """
  if not isinstance(dir,(list,tuple)):
    dir = [dir]

  nfiles = len(dir)

  if nfiles > 1 and orbit == True:
    raise Exception("Error: cannot plot multiple files when orbit = True")


  fig = plt.figure(figsize=(9,8))
  fig.subplots_adjust(wspace=0.5,hspace = 0.5)

  for ii in np.arange(nfiles):
    out = vpl.GetOutput(dir[ii])

    ctmp = 0
    for p in range(len(out.bodies)):
      if out.bodies[p].name == plname:
        body = out.bodies[p]
        ctmp = 1
      else:
        if p == len(out.bodies)-1 and ctmp == 0:
          raise Exception("Planet %s not found in folder %s"%(plname,dir[ii]))

    try:
      ecc = body.Eccentricity
    except:
      ecc = np.zeros_like(body.Time)+getattr(out.log.initial,plname).Eccentricity

    try:
      inc = body.Inc
    except:
      inc = np.zeros_like(body.Time)

    try:
      obl = body.Obliquity
    except:
      obltmp = getattr(out.log.initial,plname).Obliquity
      if obltmp.unit == 'rad':
        obltmp *= 180/np.pi
      obl = np.zeros_like(body.Time)+obltmp

    f = open(dir[ii]+'/'+plname+'.in','r')
    lines = f.readlines()
    f.close()
    pco2 = 0
    #pdb.set_trace()
    for i in range(len(lines)):
      if lines[i].split() != []:
        if lines[i].split()[0] == 'dRotPeriod':
          P = -1*np.float(lines[i].split()[1])
        if lines[i].split()[0] == 'dSemi':
          semi = np.float(lines[i].split()[1])
          if semi < 0:
            semi *= -1
        if lines[i].split()[0] == 'dpCO2':
          pco2 = np.float(lines[i].split()[1])

    try:
      longp = (body.ArgP + body.LongA + body.PrecA)*np.pi/180.0
    except:
      longp = body.PrecA*np.pi/180.0

    esinv = ecc*np.sin(longp)*np.sin(obl*np.pi/180.)

    lats = np.unique(body.Latitude)
    nlats = len(lats)
    ntimes = len(body.Time)

    # plot temperature
    temp = np.reshape(body.TempLat,(ntimes,nlats))
    ax1 = plt.subplot(4,2,1)
    pos = ax1.figbox.get_points()
    c = plt.contourf(body.Time,lats,temp.T,cmap='plasma')
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.title(r'Surface Temp [$^{\circ}$C]', fontsize = 12)
    plt.ylim(-85,85)
    plt.yticks([-60,-30,0,30,60], fontsize = 9)
    plt.xticks(fontsize = 9)
    if xrange:
      plt.xlim(xrange)
    cbar = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)

    # plot albedo
    alb = np.reshape(body.AlbedoLat,(ntimes,nlats))
    ax2 = plt.subplot(4,2,3)
    pos = ax2.figbox.get_points()
    c = plt.contourf(body.Time,lats,alb.T,cmap = 'Blues_r')
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.title('Albedo [TOA]', fontsize = 12)
    plt.ylim(-85,85)
    plt.yticks([-60,-30,0,30,60], fontsize = 9)
    plt.xticks(fontsize = 9)
    if xrange:
      plt.xlim(xrange)
    cbar = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)


    # plot ice height
    ice = np.reshape(body.IceHeight,(ntimes,nlats))
    ax3 = plt.subplot(4,2,5)
    pos = ax3.figbox.get_points()
    c = plt.contourf(body.Time,lats,ice.T,cmap='Blues_r')
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.title('Ice sheet height [m]', fontsize = 12)
    plt.ylim(-85,85)
    plt.yticks([-60,-30,0,30,60], fontsize = 9)
    plt.xticks(fontsize = 9)
    if xrange:
      plt.xlim(xrange)
    cbar = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)


    # plot bedrock
    brock = np.reshape(body.BedrockH,(ntimes,nlats))
    ax4 = plt.subplot(4,2,7)
    pos = ax4.figbox.get_points()
    c = plt.contourf(body.Time,lats,brock.T,cmap='Reds_r')
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.title('Bedrock height [m]', fontsize = 12)
    plt.ylim(-85,85)
    plt.yticks([-60,-30,0,30,60], fontsize = 9)
    plt.xlabel('Time [years]',fontsize = 10)
    plt.xticks(fontsize = 9)
    if xrange:
      plt.xlim(xrange)
    cbar = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)

    # plot insolation
    insol = np.reshape(body.AnnInsol,(ntimes,nlats))
    ax5 = plt.subplot(4,2,2)
    pos = ax5.figbox.get_points()
    c = plt.contourf(body.Time,lats,insol.T,cmap='plasma')
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.title(r'Annual average instellation [W/m$^2$]', fontsize = 12)
    plt.ylim(-85,85)
    plt.yticks([-60,-30,0,30,60], fontsize = 9)
    plt.xticks(fontsize = 9)
    if xrange:
      plt.xlim(xrange)
    cbar = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)

    if dir[ii] == '.':
      dir[ii] = 'cwd'

  #fig.suptitle('\n'.join(titlestr),fontsize=20)

    if (sys.argv[1] == 'pdf'):
        plt.savefig('IceBeltClimateEvol.pdf', dpi=300)
    if (sys.argv[1] == 'png'):
        plt.savefig('IceBeltClimateEvol.png', dpi=300)
    if show:
        plt.show()
    else:
        plt.close()

def seasonal_maps(time, dir = '.', show = True):
  """
  Creates plots of insolation, temperature, and ice balance
  over the course of an orbit (4 orbits for temp)

  Parameters
  ----------
  time : float
    The time of the data you want to plot (see SeasonalClimateFiles directory)

  Keyword Arguments
  -----------------
  dir : string
    Directory of vplanet simulation (default = '.')
  show : bool
    Show plot in Python (default = True)

  Output
  ------
  PDF format plot with name 'surf_seas_<time>.pdf'

  """
  dirf = dir+'/SeasonalClimateFiles'
  if not os.path.exists(dirf):
    raise StandardError('SeasonalClimateFiles directory does not exist')
  else:
    check = 0
    for f in subprocess.check_output('echo '+dirf+'/*.DailyInsol.*',shell=True).split():
      f1 = re.split('\.',re.split('/',f.decode('ascii'))[-1])  #split apart output file

      if len(f1) == 4:
        timestamp = f1[3]
      elif len(f1) == 5:
        timestamp = f1[3]+'.'+f1[4]

      time0 = np.float(timestamp)

      if time0 == time:
        #get system and planet names
        sysname = f1[0]
        plname = f1[1]
        insolf = dirf+'/'+sysname+'.'+plname+'.DailyInsol.'+timestamp
        tempf = dirf+'/'+sysname+'.'+plname+'.SeasonalTemp.'+timestamp
        icef = dirf+'/'+sysname+'.'+plname+'.SeasonalIceBalance.'+timestamp
        check = 1
    #print(insolf,tempf,icef)
    #exit()

    if check == 0:
      raise StandardError('Climate data not found for time %f'%time)

    insol = np.loadtxt(insolf,unpack=True)
    temp = np.loadtxt(tempf,unpack=True)
    ice = np.loadtxt(icef,unpack=True)
    output = vpl.GetOutput(dir)

    ctmp = 0
    for p in range(len(output.bodies)):
      if output.bodies[p].name == plname:
        body = output.bodies[p]
        ctmp = 1
      else:
        if p == len(output.bodies)-1 and ctmp == 0:
          raise Exception("Planet %s not found in folder %s"%(plname,folders[j]))

    lats = body.Latitude[0]
    try:
      obl = planet.Obliquity[np.where(body.Time==time)[0]]
    except:
      obl = getattr(output.log.initial,plname).Obliquity
      if obl.unit == 'rad':
        obl *= 180/np.pi

    try:
      ecc = planet.Eccentricity[np.where(planet.Time==time)[0]]
    except:
      ecc = getattr(output.log.initial,plname).Eccentricity

    try:
      longp = (planet.LongP+planet.PrecA)[np.where(planet.Time==time)[0]]
    except:
      try:
        longp = getattr(output.log.initial,plname).LongP
      except:
        try:
          longp = (getattr(output.log.initial,plname).LongA+getattr(out.log.initial,plname).ArgP)
          if longp.unit == 'rad':
            longp *= 180/np.pi
          longp = longp%360
        except:
          longp = 0

    fig = plt.figure(figsize=(9,3.5))
    plt.subplot(1,3,1)
    fig.subplots_adjust(wspace=0.5)

    plt.title(r'Insolation [W/m$^2$]',fontsize=12)
    c1 = plt.contourf(np.arange(np.shape(insol)[1]),lats,insol,cmap='plasma')
    cbar = plt.colorbar(c1)
    plt.ylim(lats[0],lats[-1])
    plt.xlabel('Time [days]', fontsize = 10)
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.xticks(fontsize = 9)
    plt.yticks(fontsize = 9)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)

    scale = 4*np.shape(insol)[1]/np.shape(temp)[1]
    plt.subplot(1,3,2)
    c2 = plt.contourf(np.arange(np.shape(temp)[1])*scale,lats,temp,cmap='plasma')
    plt.title(r'Surface Temp [$^{\circ}$C]',fontsize=12)
    cbar = plt.colorbar(c2)
    plt.ylim(lats[0],lats[-1])
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.xlabel('Time [days]', fontsize = 10)
    plt.xlim(0,np.shape(temp)[1]*scale/4.)
    plt.xticks(fontsize = 9)
    plt.yticks(fontsize = 9)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)


    scale = np.shape(insol)[1]/np.shape(ice)[1]
    plt.subplot(1,3,3)
    c3 = plt.contourf(np.arange(np.shape(ice)[1])*scale,lats,ice,cmap='Blues_r')
    plt.title(r'Ice balance [kg/m$^2$/s]',fontsize=12)
    cbar = plt.colorbar(c3)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), fontsize = 9)
    plt.ylim(lats[0],lats[-1])
    plt.xlabel('Time [days]', fontsize = 10)
    plt.ylabel(r'Latitude [$^\circ$]', fontsize = 10)
    plt.xticks(fontsize = 9)
    plt.yticks(fontsize = 9)

    if (sys.argv[1] == 'pdf'):
        plt.savefig('IceBeltSeasonal.pdf', dpi=300)
    if (sys.argv[1] == 'png'):
        plt.savefig('IceBeltSeasonal.png', dpi=300)
    if show:
        plt.show()
    else:
        plt.close()

#Runs VPLANET
subprocess.call(['vplanet', 'vpl.in'])

#makes the plots
clim_evol('earth',show=False)
seasonal_maps(0,show=False)
