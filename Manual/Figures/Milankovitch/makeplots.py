import numpy as np
import matplotlib.pyplot as plt
import vplot
from matplotlib import ticker


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
      longp = (body.ArgP + body.LongA + body.PrecA)*np.pi/180.0
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
    clb=plt.colorbar(c,cax=plt.axes([pos[1,0]+0.01,pos[0,1],0.01,pos[1,1]-pos[0,1]]))
    clb.set_label('Surface Temp.\n($^{\circ}$C)',fontsize=12)

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

  plt.savefig('milankovitch.pdf')

  if show:
    plt.show()
  else:
    plt.close()

comp2huybers('Earth',show=False)
