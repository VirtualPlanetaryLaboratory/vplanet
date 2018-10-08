# -*- coding: iso-8859-1 -*-
import numpy as np
import matplotlib.pyplot as plt
import vplot
import sys

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

plt.rcParams["text.usetex"]=True
plt.rcParams["text.latex.unicode"]=True

out = vplot.GetOutput()

fig = plt.figure(figsize=(8.5,8))
fig.subplots_adjust(hspace=0.1)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/1.dat',unpack=True)
ax=plt.subplot(4,2,1)
plt.plot(out.Mercury.Time/1e6,out.Mercury.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.85,'Mercury',transform=ax.transAxes)

plt.subplot(4,2,2)
plt.plot(out.Mercury.Time/1e6,out.Mercury.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)
plt.legend(loc='upper right',fontsize=10)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/2.dat',unpack=True)
ax=plt.subplot(4,2,3)
plt.plot(out.Venus.Time/1e6,out.Venus.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.85,'Venus',transform=ax.transAxes)

plt.subplot(4,2,4)
plt.plot(out.Venus.Time/1e6,out.Venus.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/3.dat',unpack=True)
ax=plt.subplot(4,2,5)
plt.plot(out.Earth.Time/1e6,out.Earth.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.85,'Earth',transform=ax.transAxes)

plt.subplot(4,2,6)
plt.plot(out.Earth.Time/1e6,out.Earth.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/4.dat',unpack=True)
ax=plt.subplot(4,2,7)
plt.plot(out.Mars.Time/1e6,out.Mars.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xlabel('Time (Myr)')
plt.text(0.05,0.05,'Mars',transform=ax.transAxes)

plt.subplot(4,2,8)
plt.plot(out.Mars.Time/1e6,out.Mars.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xlabel('Time (Myr)')

if (sys.argv[1] == 'pdf'):
    plt.savefig('SSDistOrbDistRotInner.pdf',dpi=300)
if (sys.argv[1] == 'png'):
    plt.savefig('SSDistOrbDistRotInner.png',dpi=300)
plt.close()

######################################################################
fig = plt.figure(figsize=(8.5,8))
fig.subplots_adjust(hspace=0.1,wspace=0.23)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/5.dat',unpack=True)
ax=plt.subplot(4,2,1)
plt.plot(out.Jupiter.Time/1e6,out.Jupiter.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.05,'Jupiter',transform=ax.transAxes)

plt.subplot(4,2,2)
plt.plot(out.Jupiter.Time/1e6,out.Jupiter.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylim(1,2.3)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)
plt.legend(loc='upper right',fontsize=10,ncol=2)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/6.dat',unpack=True)
ax=plt.subplot(4,2,3)
plt.plot(out.Saturn.Time/1e6,out.Saturn.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.9,'Saturn',transform=ax.transAxes)

plt.subplot(4,2,4)
plt.plot(out.Saturn.Time/1e6,out.Saturn.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/7.dat',unpack=True)
ax=plt.subplot(4,2,5)
plt.plot(out.George.Time/1e6,out.George.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xticks(visible=False)
plt.text(0.05,0.85,'Uranus',transform=ax.transAxes)

plt.subplot(4,2,6)
plt.plot(out.George.Time/1e6,out.George.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xticks(visible=False)

t, a, e, inc, longa, argp, ma = np.loadtxt('hnbdata/8.dat',unpack=True)
ax=plt.subplot(4,2,7)
plt.plot(out.Neptune.Time/1e6,out.Neptune.Eccentricity,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,e,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$e$')
plt.xlabel('Time (Myr)')
plt.text(0.05,0.85,'Neptune',transform=ax.transAxes)

plt.subplot(4,2,8)
plt.plot(out.Neptune.Time/1e6,out.Neptune.Inc,'k-',label='DistOrb',zorder=100)
plt.plot(t/1e6,inc,'0.5',label='HNBody')
plt.xlim(0,1)
plt.ylabel('$i$ ($^{\circ}$)')
plt.xlabel('Time (Myr)')

if (sys.argv[1] == 'pdf'):
    plt.savefig('SSDistOrbDistRotOuter.pdf',dpi=300)
if (sys.argv[1] == 'png'):
    plt.savefig('SSDistOrbDistRotOuter.png',dpi=300)
plt.close()


out2 = vplot.GetOutput('womoon')
# plt.rcParams["mathtext."]
tm, oblm, pAm, precf = np.loadtxt('marshnb/solarsys.Mars.backward',unpack = True)
tm1, em1, incm1, argpm1, longam1, oblm1, pAm1, precf1 = np.loadtxt('marsvpl/solarsys.Mars.backward',unpack = True)

fig = plt.figure(figsize=(8.5,4))
fig.subplots_adjust(wspace=0.25)

plt.subplot(1,2,1)
plt.plot(out.Earth.Time/1e6,out.Earth.Obliquity,'-',c=vplot.colors.pale_blue,label='Earth with Moon')
plt.plot(out2.Earth.Time/1e6,out2.Earth.Obliquity,'-',c=vplot.colors.dark_blue,label='Earth without Moon')
plt.ylabel(r'$\varepsilon$ ($^{\circ}$)')
plt.xlabel('Time (Myr)')
plt.xlim(0,1)
plt.legend(loc='upper right',fontsize=10)

ax=plt.subplot(1,2,2)
plt.plot(tm1/1e6,oblm1,'-',color=vplot.colors.orange,zorder=100,lw=1,label='Mars (DistOrb+DistRot)')
plt.plot(tm/1e6,oblm,'-',c=vplot.colors.red,lw=1,label='Mars (HNBody+DistRot)')
#plt.plot(out4.Mars.Time/1e6,out4.Mars.Obliquity,'k-',label='Mars')
plt.ylabel(r'$\varepsilon$ ($^{\circ}$)')
plt.xlabel('Time (Myr)')
plt.xlim(-10,0)
plt.ylim(5,55)
plt.legend(loc='upper right', fontsize=10)
# plt.ylim(0,10)
#plt.text(0.2,0.5,'Mars evolution here?',transform=ax.transAxes)

if (sys.argv[1] == 'pdf'):
    plt.savefig('SSDistOrbDistRotObliq.pdf',dpi=300)
if (sys.argv[1] == 'png'):
    plt.savefig('SSDistOrbDistRotObliq.png',dpi=300)
plt.close()
