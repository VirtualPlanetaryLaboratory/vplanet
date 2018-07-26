import numpy as np
import matplotlib.pyplot as plt
import vplot as vpl
import celestools as ct

m = np.array([1.0211863312304868e-05,8.40976978660401e-06])
t, a, e, inc, ap, la, ma = np.loadtxt('hnb/1.dat',unpack=True)
tc, ac, ec, incc, apc, lac, mac = np.loadtxt('hnb/2.dat',unpack=True)

out = vpl.GetOutput()

plt.subplot(2,3,1)
plt.plot(out.b.Time, out.b.SemiMajorAxis,'b-')
plt.plot(t,a,'r-')

plt.subplot(2,3,4)
plt.plot(out.c.Time, out.c.SemiMajorAxis,'b-')
plt.plot(tc,ac,'r-')

plt.subplot(2,3,2)
plt.plot(out.b.Time, out.b.Eccentricity,'b-')
plt.plot(t,e,'r-')

plt.subplot(2,3,5)
plt.plot(out.c.Time, out.c.Eccentricity,'b-')
plt.plot(tc,ec,'r-')

plt.subplot(2,3,3)
plt.plot(out.b.Time, (out.b.ArgP+out.b.LongA)%360,'b-')
plt.plot(t,(ap+la)%360,'r-')

plt.subplot(2,3,6)
plt.plot(out.c.Time, (out.c.ArgP+out.c.LongA)%360,'b-')
plt.plot(tc,(apc+lac)%360,'r-')

P = ct.Semi2Per(1.4,m[0],out.b.SemiMajorAxis)
n = 2*np.pi/P

Pc = ct.Semi2Per(1.4,m[1],out.c.SemiMajorAxis)
nc = 2*np.pi/Pc

Ph = ct.Semi2Per(1.4,m[0],a)
nh = 2*np.pi/Ph

Pch = ct.Semi2Per(1.4,m[1],ac)
nch = 2*np.pi/Pch

plt.figure()
plt.subplot(2,1,1)
plt.plot(out.b.Time,n,'b-')
plt.plot(out.b.Time,out.b.DMeanLDtDistRes*np.pi/180./365.25,'k.')
plt.plot(t,nh,'r-')
plt.subplot(2,1,2)
plt.plot(out.c.Time,nc,'b-')
plt.plot(out.b.Time,out.c.DMeanLDtDistRes*np.pi/180./365.25,'k.')

plt.plot(t,nch,'r-')

# plt.subplot(2,3,3)
# plt.plot(out.b.Time, out.b.MeanL,'b-')
# ml = (ma+la+ap)%360
# plt.plot(t,ml,'r-')
# plt.xlim(0,1)
# 
# plt.subplot(2,3,6)
# plt.plot(out.c.Time, out.c.MeanL,'b-')
# mlc = (mac+lac+apc)%360
# plt.plot(tc,mlc,'r-')
# plt.xlim(0,1)


plt.show()