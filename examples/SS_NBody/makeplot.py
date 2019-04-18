import numpy as np
import matplotlib.pyplot as plt
import sys
try:
    import vplot as vpl
except:
    print('Cannot import vplot. Please install vplot.')
from matplotlib.ticker import FormatStrFormatter

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

class Planet():
    a = 0.
    e = 0.
    i = 0.
    LongP = 0.
    LongA = 0.
    MeanA = 0.
    x1 = 0.
    x2 = 0.
    x3 = 0.
    v1 = 0.
    v2 = 0.
    v3 = 0.
    Time = []
    Mass = 0.

def ReadData(FileName):
    planet = Planet()
    planet.Time, _, planet.a, planet.e, planet.i, planet.LongP, planet.LongA, planet.MeanA = np.loadtxt(FileName, unpack=True)
    planet.a = planet.a#/1.49597870700e11
    return planet

def ReadHNBody(FileName):
    planet = Planet()
    planet.Time, planet.a, planet.e, planet.i, planet.LongP, planet.LongA, planet.MeanA, planet.x1, planet.x2, planet.x3, planet.v1, planet.v2, planet.v3 = np.loadtxt(FileName, skiprows=17, unpack=True)
    planet.MeanA[planet.MeanA<0]+=360
    planet.LongP[planet.LongP<0]+=360
    planet.LongA[planet.LongA<0]+=360
    planet.a = planet.a
    return planet

SS = vpl.GetOutput()

#star = Planet()
#Time, dt, star.a, star.e, star.i, star.LongP, star.LongA, star.MeanA, AngMom, OrbEnergy = np.loadtxt('SpiNBody.Star.forward', unpack=True)

hnEarth = ReadHNBody('plan3.dat')
hnEarth.Mass = 1.


#hnarray = (hnEarth.Time <= EndTime)
#array   = (hnEarth.Time <= EndTime)
fig,([ax1,ax2],[ax3,ax4],[ax5,ax6])=plt.subplots(3,2,figsize=[18,12])

ax1.plot(hnEarth.Time,hnEarth.a,color=vpl.colors.red)
ax1.plot(SS.Earth.Time,SS.Earth.SemiMajorAxis,'k')
#plt.xlabel('Time (yrs)')
ax1.set_ylabel('Semi-Major Axis (AU)')
ax1.yaxis.set_major_formatter(FormatStrFormatter('%.6f'))

ax2.plot(hnEarth.Time, hnEarth.e,color=vpl.colors.red)
ax2.plot(SS.Earth.Time,SS.Earth.Eccentricity,'k')
#plt.xlabel('Time (yrs)')
ax2.set_ylabel('Eccentricity')

plt.subplot(3,2,3)
plt.plot(hnEarth.Time, hnEarth.i,color=vpl.colors.red)
plt.plot(SS.Earth.Time, SS.Earth.SpiNBodyInc,'k')

#plt.xlabel('Time (yrs)')
plt.ylabel('Inclination ($^\circ$)')

plt.subplot(3,2,4)
plt.plot(hnEarth.Time, hnEarth.LongP,color=vpl.colors.red)
plt.plot(SS.Earth.Time, SS.Earth.LongP,'k')
plt.ylabel('Longitude of Pericenter ($^\circ$)')

plt.subplot(3,2,5)
plt.plot(hnEarth.Time, hnEarth.LongA,color=vpl.colors.red)
plt.plot(SS.Earth.Time, SS.Earth.SpiNBodyLongA,'k')
plt.ylabel('Longitude of Ascending Node ($^\circ$)')
plt.xlabel('Time (yrs)')
plt.legend(['HNBody','VPlanet'])

plt.subplot(3,2,6)
plt.plot(hnEarth.Time, hnEarth.MeanA,color=vpl.colors.red)
plt.plot(SS.Earth.Time, SS.Earth.MeanAnomaly,'k')
plt.ylabel('Mean Anomaly ($^\circ$)')
plt.xlabel('Time (yrs)')

if (sys.argv[1] == 'pdf'):
    plt.savefig('SS_NBody.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('SS_NBody.png')

plt.close()

#Plot the fractional angular momentum over time
#fig,([ax1,ax2])=plt.subplots(2,1,figsize=[8,8])

#plt.subplot(2,1,1)
#dEta=(AngMom/AngMom[0]-1)
#plt.plot(Time,dEta*1e11)
#plt.ylabel(r'$(\Delta L / L - 1)/10^{11}$', fontsize=16)

#Now for energy
#plt.subplot(2,1,2)
#Energy = OrbEnergy
#dEnergy = (Energy/Energy[0]-1)
#plt.plot(Time,dEnergy*1e9,'r')
#plt.xlabel('Time (yr)', fontsize=16)
#plt.ylabel(r'$(\Delta E / E - 1)/10^{9}$', fontsize=16)
#plt.savefig('SpiNBody_Conservation.pdf')
#plt.savefig('SpiNBody_Conservation.png')
