import numpy as np
import matplotlib.pyplot as plt
try:
    import vplot
except:
    print('Cannot import vplot. Please install vplot.')
from matplotlib.ticker import FormatStrFormatter

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

Earth = ReadData('SpiNBody.Earth.forward')

star = Planet()
Time, dt, star.a, star.e, star.i, star.LongP, star.LongA, star.MeanA, AngMom, OrbEnergy = np.loadtxt('SpiNBody.Star.forward', unpack=True)

hnEarth = ReadHNBody('plan3.dat')
hnEarth.Mass = 1.

def PlotPlanet(planet,hnplanet,Name,EndTime=1e13):
    hnarray = (hnplanet.Time <= EndTime)
    array   = (planet.Time <= EndTime)
    fig,([ax1,ax2],[ax3,ax4],[ax5,ax6])=plt.subplots(3,2,figsize=[18,12])

    ax1.plot(hnplanet.Time[hnarray],hnplanet.a[hnarray],'r.')
    ax1.plot(planet.Time[array],planet.a[array],'.')

    #plt.xlabel('Time (yrs)')
    ax1.set_ylabel('Distance (AU)')
    ax1.yaxis.set_major_formatter(FormatStrFormatter('%.6f'))

    ax2.plot(hnplanet.Time[hnarray], hnplanet.e[hnarray],'r')
    ax2.plot(planet.Time[array], planet.e[array])
    #plt.xlabel('Time (yrs)')
    ax2.set_ylabel('Eccentricity')

    plt.subplot(3,2,3)
    plt.plot(hnplanet.Time[hnarray], hnplanet.i[hnarray],'r')
    plt.plot(planet.Time[array], planet.i[array])

    #plt.xlabel('Time (yrs)')
    plt.ylabel('Inclination (deg)')

    plt.subplot(3,2,4)
    plt.plot(hnplanet.Time[hnarray], hnplanet.LongP[hnarray],'r')
    plt.plot(planet.Time[array], planet.LongP[array])
    plt.ylabel('Longitude of Pericenter (deg)')

    plt.subplot(3,2,5)
    plt.plot(hnplanet.Time[hnarray], hnplanet.LongA[hnarray],'r')
    plt.plot(planet.Time[array], planet.LongA[array])
    plt.ylabel('Longitude of Ascending Node (deg)')
    plt.xlabel('Time (yrs)')
    plt.legend(['HNBody','VPlanet'])

    plt.subplot(3,2,6)
    plt.plot(hnplanet.Time[hnarray], hnplanet.MeanA[hnarray],'r')
    plt.plot(planet.Time[array], planet.MeanA[array])
    plt.ylabel('Mean Anomaly (deg)')
    plt.xlabel('Time (yrs)')

    plt.savefig('SpiNBody_'+Name+'.png')
    plt.savefig('SpiNBody_'+Name+'.pdf')

PlotPlanet(Earth,hnEarth,'Earth')

#Plot the fractional angular momentum over time
fig,([ax1,ax2])=plt.subplots(2,1,figsize=[8,8])

plt.subplot(2,1,1)
dEta=(AngMom/AngMom[0]-1)
plt.plot(Time,dEta*1e11)
plt.ylabel(r'$(\Delta L / L - 1)/10^{11}$', fontsize=16)

#Now for energy
plt.subplot(2,1,2)
Energy = OrbEnergy
dEnergy = (Energy/Energy[0]-1)
plt.plot(Time,dEnergy*1e9,'r')
plt.xlabel('Time (yr)', fontsize=16)
plt.ylabel(r'$(\Delta E / E - 1)/10^{9}$', fontsize=16)
plt.savefig('SpiNBody_Conservation.pdf')
plt.savefig('SpiNBody_Conservation.png')
