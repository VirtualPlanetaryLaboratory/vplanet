# -*- coding: iso-8859-1 -*-
import numpy as np
import matplotlib.pyplot as plt

# inner solar system
th, eh, inch, argph, longah = np.loadtxt('solarsys.Mercury.forward',unpack = True)
tv, ev, incv, argpv, longav, oblv, pAv = np.loadtxt('solarsys.Venus.forward',unpack = True)
te, ee, ince, argpe, longae, oble, pAe = np.loadtxt('solarsys.Earth.forward',unpack = True)
tm, em, incm, argpm, longam, oblm, pAm = np.loadtxt('solarsys.Mars.forward',unpack = True)

#outer
tj, ej, incj, argpj, longaj = np.loadtxt('solarsys.Jupiter.forward',unpack = True)
ts, es, incs, argps, longas = np.loadtxt('solarsys.Saturn.forward',unpack = True)
tg, eg, incg, argpg, longag = np.loadtxt('solarsys.George.forward',unpack = True)
tn, en, incn, argpn, longan = np.loadtxt('solarsys.Neptune.forward',unpack = True)

plt.figure()
plt.subplot(2,1,1)
plt.plot(th/1000,eh,color='slategray',label='Mercury')
plt.plot(tv/1000,ev,color='orange',label='Venus')
plt.plot(te/1000,ee,color='b',label='Earth')
plt.plot(tm/1000,em,color='darkred',label='Mars')
plt.ylabel('eccentricity')

plt.subplot(2,1,2)
plt.plot(th/1000, inch,color='slategray',label='Mercury')
plt.plot(tv/1000,incv,color='orange',label='Venus')
plt.plot(te/1000,ince,color='b',label='Earth')
plt.plot(tm/1000,incm,color='darkred',label='Mars')
plt.ylabel('inclination (degrees)')
plt.xlabel('time [kyr]')
plt.legend(fontsize=10)

plt.figure()
plt.subplot(2,1,1)
plt.plot(tj/1000,ej,color='r',label='Jupiter')
plt.plot(ts/1000,es,color='gold',label='Saturn')
plt.plot(tg/1000,eg,color='skyblue',label='George')
plt.plot(tn/1000,en,color='darkblue',label='Neptune')
plt.ylabel('eccentricity')

plt.subplot(2,1,2)
plt.plot(tj/1000, incj,color='r',label='Jupiter')
plt.plot(ts/1000,incs,color='gold',label='Saturn')
plt.plot(tg/1000,incg,color='skyblue',label='George')
plt.plot(tn/1000,incn,color='darkblue',label='Neptune')
plt.ylabel('inclination (degrees)')
plt.xlabel('time [kyr]')
plt.ylim(0.5,4)
plt.legend(fontsize=10)

plt.figure()
plt.plot(tv/1000,180-oblv,color='orange',label='Venus (180-obl)')
plt.plot(tv/1000,oble,color='b',label='Earth')
plt.plot(tm/1000,oblm,color='darkred',label='Mars')
plt.ylabel('obliquity (degrees)')
plt.xlabel('time [kyr]')
plt.legend(fontsize=10, loc=(0.8,0.1))

plt.show()