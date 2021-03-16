import numpy as np
import matplotlib.pyplot as plt
import vplot
from matplotlib import ticker
import os
import subprocess
import re
import sys

def comp2huybers(plname,dir='.',xrange=False,show=True):
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


  fig = plt.figure(figsize=(8,12))

  fig.subplots_adjust(wspace=0.3,top=0.9,hspace=0.2)

  for ii in np.arange(nfiles):
    out = vplot.GetOutput(dir[ii])
    #pdb.set_trace()

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
      longp = (body.ArgP + body.LongA + body.PrecA + 180)*np.pi/180.0
    except:
      longp = body.PrecA*np.pi/180.0

    esinv = ecc*np.sin(longp)

    lats = np.unique(body.Latitude)
    nlats = len(lats)
    ntimes = len(body.Time)

    # plot temperature
    temp = np.reshape(body.TempLandLat,(ntimes,nlats))

    ax1 = plt.subplot(7,1,5)
    pos = ax1.figbox.get_points()
    c = plt.contourf(body.Time/1e6,lats[lats>58],temp.T[lats>58],20)
    plt.ylabel('Latitude')

    plt.ylim(60,83)
    plt.yticks([60,70,80])
    if xrange == False:
      left = 0
    else:
      left = xrange[0]
#     plt.text(left,140,'\n'.join(titlestr),fontsize=20)
    if xrange:
      plt.xlim(xrange)
    # plt.contour(body.Time/1e6,lats[lats>60],temp.T[lats>60],levels=[0],colors='w')
    plt.xticks(visible=False)
    clb=plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]),ticks=[-17,-15,-13,-11,-9,-7,-5])
    clb.set_label('Surface Temp.\n($^{\circ}$C)',fontsize=12)
    # clb.ax.set_yticklabels([-17,-15,-13,-11,-9,-7])
    # plot ice height
    ice = np.reshape(body.IceHeight+body.BedrockH,(ntimes,nlats))

    ax3 = plt.subplot(7,1,4)
    pos = ax3.figbox.get_points()
#     pdb.set_trace()
    c = plt.contourf(body.Time/1e6,lats[lats>58],ice.T[lats>58,:],20)
    plt.ylabel('Latitude')
    plt.ylim(60,83)
  #   plt.xlim(0,2e6)
    plt.yticks([60,70,80])
    if xrange:
      plt.xlim(xrange)
    # plt.contour(body.Time,lats[lats>60],ice.T[lats>60],levels=[0],colors='w')
    plt.xticks(visible=False)
    clb=plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    #clb.set_label('Ice height (m)')
    clb.set_label('Ice sheet\nheight (m)',fontsize=12)

    # ax3p = ax3.twinx()
  #   plt.plot(body.Time,esinv,linestyle = 'solid',marker='None',color='salmon',linewidth=2)


    ax4 = plt.subplot(7,1,6)
    pos = ax4.figbox.get_points()
    acc = body.IceAccum
    c = plt.contourf(body.Time/1e6,lats[lats>58],acc.T[lats>58],20)
    plt.ylabel('Latitude')
    plt.ylim(60,83)
    plt.yticks([60,70,80])
    plt.xticks(visible=False)
    if xrange == False:
      left = 0
    else:
      left = xrange[0]
#     plt.text(left,140,'\n'.join(titlestr),fontsize=20)
    if xrange:
      plt.xlim(xrange)
    clb=plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    tloc = ticker.MaxNLocator(nbins=5)
    clb.locator=tloc
    clb.update_ticks()
    clb.set_label('Ice Accum.\n(m year$^{-1}$)',fontsize=12)

    ax5 = plt.subplot(7,1,7)
    pos = ax5.figbox.get_points()
    abl = body.IceAblate
    c = plt.contourf(body.Time/1e6,lats[lats>58],-abl.T[lats>58],20)
    plt.ylabel('Latitude')
    # plt.title(r'Ice Ablation (m year$^{-1}$)',fontsize=12)
    plt.ylim(60,83)
    plt.yticks([60,70,80])
    plt.xlabel('Time (Myr)')
    if xrange == False:
      left = 0
    else:
      left = xrange[0]
#     plt.text(left,140,'\n'.join(titlestr),fontsize=20)
    if xrange:
      plt.xlim(xrange)
    clb = plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label('Ice Ablation\n(m year$^{-1}$)',fontsize=12)

    plt.subplot(7,1,1)
    plt.plot(body.Time/1e6,obl,linestyle = 'solid',marker='None',color=vplot.colors.dark_blue,linewidth =2)
    plt.ylabel('Obliquity')
    plt.xticks(visible=False)
    if xrange:
      plt.xlim(xrange)

    plt.subplot(7,1,2)
    plt.plot(body.Time/1e6,ecc,linestyle = 'solid',marker='None',color=vplot.colors.purple,linewidth =2)
    plt.ylabel('Eccenticity')
    plt.xticks(visible=False)
    if xrange:
      plt.xlim(xrange)

    plt.subplot(7,1,3)
    plt.plot(body.Time/1e6,esinv,linestyle = 'solid',marker='None',color=vplot.colors.red,linewidth=2)
    plt.ylabel('CPP')
    plt.xticks(visible=False)
    if xrange:
      plt.xlim(xrange)

    if dir[ii] == '.':
      dir[ii] = 'cwd'

  if (sys.argv[1] == 'pdf'):
      plt.savefig('EarthClimateMilankovitch.pdf')
  if (sys.argv[1] == 'png'):
      plt.savefig('EarthClimateMilankovitch.png')
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
        planckbf = dirf+'/'+sysname+'.'+plname+'.SeasonalFOut.'+timestamp
        check = 1

    if check == 0:
      raise StandardError('Climate data not found for time %f'%time)

    insol = np.loadtxt(insolf,unpack=True)
    temp = np.loadtxt(tempf,unpack=True)
    ice = np.loadtxt(icef,unpack=True)
    fluxo = np.loadtxt(planckbf,unpack=True)
    output = vplot.GetOutput(dir)
    ctmp = 0
    for p in range(len(output.bodies)):
      if output.bodies[p].name == plname:
        body = output.bodies[p]
        ctmp = 1
      else:
        if p == len(output.bodies)-1 and ctmp == 0:
          raise Exception("Planet %s not found in folder %s"%(plname,dir))

    lats = body.Latitude[0]
    try:
      obl = body.Obliquity[np.where(body.Time==time)[0]]
    except:
      obl = getattr(output.log.initial,plname).Obliquity
      if obl.unit == 'rad':
        obl *= 180/np.pi

    try:
      ecc = body.Eccentricity[np.where(body.Time==time)[0]]
    except:
      ecc = getattr(output.log.initial,plname).Eccentricity

    try:
      longp = (body.LongP+body.PrecA)[np.where(body.Time==time)[0]]
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

    fig = plt.figure(figsize=(8.5,5))
    fig.subplots_adjust(wspace=0.3,hspace=0.2)
    # fig.suptitle('Time = %f, Obl = %f, Ecc = %f, LongP = %f'%(time,obl,ecc,longp),fontsize=20)
    ax = plt.subplot(2,2,1)
    ax.set_position(pos=[0.1,0.54,0.3,0.41])
    pos = ax.figbox.get_points()
    c1=plt.contourf(np.arange(np.shape(insol)[1]),lats,insol,cmap='plasma')
    plt.ylim(lats[0],lats[-1])
    plt.ylabel('Latitude ($^{\circ}$)')
    plt.yticks([-60,-30,0,30,60],['60S','30S','0','30N','60N'])
    # plt.xticks(visible=False)
    clb = plt.colorbar(c1,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label(r'Insolation (W/m$^2$)',fontsize=12)

    scale = 4*np.shape(insol)[1]/np.shape(temp)[1]
    ax = plt.subplot(2,2,2)
    ax.set_position(pos=[0.6,0.54,0.3,0.41])
    pos = ax.figbox.get_points()
    c2=plt.contourf(np.arange(np.shape(temp)[1])*scale,lats,temp,cmap='plasma')
    plt.ylim(lats[0],lats[-1])
    # plt.ylabel('Latitude ($^{\circ}$)')
    plt.xlim(0,np.shape(temp)[1]*scale/4.)
    plt.yticks([-60,-30,0,30,60],['60S','30S','0','30N','60N'])
    # plt.xticks(visible=False)
    clb=plt.colorbar(c2,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label(r'Surface Temp ($^{\circ}$C)',fontsize=12)

    scale = np.shape(insol)[1]/np.shape(ice)[1]
    ax = plt.subplot(2,2,3)
    ax.set_position(pos=[0.1,0.05,0.3,0.41])
    pos = ax.figbox.get_points()
    c3=plt.contourf(np.arange(np.shape(ice)[1])*scale,lats,ice*1e3,cmap='Blues_r')
    plt.ylim(lats[0],lats[-1])
    plt.ylabel('Latitude ($^{\circ}$)')
    plt.yticks([-60,-30,0,30,60],['60S','30S','0','30N','60N'])
    plt.xlabel('Day of Year')
    clb =plt.colorbar(c3,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label(r'Ice Mass Balance (10$^{-3}$ kg/m$^2$/s)',fontsize=12)


    scale = 4*np.shape(insol)[1]/np.shape(temp)[1]
    ax = plt.subplot(2,2,4)
    ax.set_position(pos=[0.6,0.05,0.3,0.41])
    pos = ax.figbox.get_points()
    c2=plt.contourf(np.arange(np.shape(fluxo)[1])*scale,lats,fluxo,cmap='plasma')
    plt.ylim(lats[0],lats[-1])
    plt.xlim(0,np.shape(temp)[1]*scale/4.)
    # plt.ylabel('Latitude ($^{\circ}$)')
    plt.yticks([-60,-30,0,30,60],['60S','30S','0','30N','60N'])
    plt.xlabel('Day of Year')
    clb=plt.colorbar(c2,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label(r'OLR (W/m$^{2}$)',fontsize=12)

    if (sys.argv[1] == 'pdf'):
        plt.savefig('EarthClimateSeasons.pdf', dpi=300)
    if (sys.argv[1] == 'png'):
        plt.savefig('EarthClimateSeasons.png', dpi=300)
    if show:
        plt.show()
    else:
        plt.close()

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

#Runs VPLANET
subprocess.call(['vplanet', 'vpl.in'])

# Make plots
comp2huybers('Earth',show=False)
seasonal_maps(0,show=False)
