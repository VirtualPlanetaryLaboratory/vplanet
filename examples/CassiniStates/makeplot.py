import numpy as np
import matplotlib.pyplot as plt
import vplot
import sys
import scipy.signal as sig
#plt.rcParams["text.usetex"]=True
#plt.rcParams["text.latex.unicode"]=True

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

out = vplot.GetOutput()

fig = plt.figure(figsize=(8.5,6))
plt.subplot(2,2,1)
plt.plot(out.b.Time/1e6,out.b.RotPer,'k-')
plt.ylabel('Rotation Period (days)')
# plt.xlim(0,5)

plt.subplot(2,2,2)
plt.plot(out.b.Time/1e6,out.b.CassiniOne,'k-')
plt.ylabel(u'$\sin{\Psi}$')
# plt.xlim(0,5)

plt.subplot(2,2,3)
plt.plot(out.b.Time/1e6,out.b.Obliquity,'k-')
plt.xlabel('Time (10$^6$ yr)')
plt.ylabel('Obliquity ($^{\circ}$)')
plt.yscale('log')
# plt.xlim(0,5)

plt.subplot(2,2,4)
plt.plot(out.b.Time/1e6,out.b.CassiniTwo,'k-')
plt.xlabel('Time (10$^6$ yr)')
plt.ylabel(u'$\cos{\Psi}$')
# plt.xlim(0,5)

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    fig.savefig('CassiniStatesEvol.pdf')
if (sys.argv[1] == 'png'):
    fig.savefig('CassiniStatesEvol.png')
plt.close()

# X = np.sin(out.b.Obliquity*np.pi/180)*np.cos(out.b.PrecA*np.pi/180)
# Y = np.sin(out.b.Obliquity*np.pi/180)*np.sin(out.b.PrecA*np.pi/180)
#
# plt.plot(Y[200:],X[200:],'k-')
# plt.show()

#calculate Cassini state Hamiltonian from simulation
alpha = np.mean(out.b.PrecFNat[out.b.Time>500e3]/np.cos(out.b.Obliquity[out.b.Time>500e3]*np.pi/180))

inc = np.mean(out.b.Inc)*np.pi/180
H0 = 0
# eps = 0.6
# alpha = 1e-5
# g = -eps*alpha
p = np.sin(out.b.Inc/2*np.pi/180)*np.sin(out.b.LongA*np.pi/180)
q = np.sin(out.b.Inc/2*np.pi/180)*np.cos(out.b.LongA*np.pi/180)
pq = q + 1j*p
f, pow = sig.periodogram(pq,fs = 1/out.b.Time[1])
g = f[np.where(np.abs(pow)==np.max(np.abs(pow)))[0]]*360
eps = -g/alpha

X = np.linspace(-0.99,0.99,250)
Y = np.linspace(-0.99,0.99,250)

XX, YY = np.meshgrid(X,Y)
ZZ = np.sqrt(1 - XX**2 - YY**2)
sxy = np.sqrt(XX**2+YY**2)
s = np.sqrt(XX**2+YY**2+ZZ**2)
obl = np.arctan2(sxy,ZZ)*180/np.pi

ndots = ZZ
kdots = YY/s*np.sin(inc) + ZZ/s*np.cos(inc)

H = H0 - 0.5*alpha*ndots**2 - g*kdots
H[np.where(sxy>1)] = np.nan

xc = np.cos(np.linspace(0,360,1000)*np.pi/180)
yc = np.sin(np.linspace(0,360,1000)*np.pi/180)

obl4 = np.arccos(-g*np.cos(inc)/alpha)
y4 = -np.sin(obl4)
z4 = np.cos(obl4)

H4 = H0 - 0.5*alpha*z4**2 - g*(y4*np.sin(inc)+z4*np.cos(inc))

xobl = np.sin(out.b.Obliquity*np.pi/180)*np.cos((out.b.PrecA+out.b.LongA)*np.pi/180)
yobl = np.sin(out.b.Obliquity*np.pi/180)*np.sin((out.b.PrecA+out.b.LongA)*np.pi/180)

obl2 = -np.arccos(-g*np.cos(inc)/alpha)
y2 = -np.sin(obl2)

obl1 = np.arctan2(np.sin(inc),1-alpha/g)
y1 = -np.sin(obl1)

fig=plt.figure(figsize=(8,8))
plt.contour(Y,X,H.T,50,colors='0.5')
plt.contour(Y,X,H.T,levels=[1.00005*H4],colors='k')
plt.plot(xc,yc,'-',color='0.5')
plt.plot(xobl[out.b.Time>500e3],yobl[out.b.Time>500e3],'k.')
plt.plot([y2+0.01,y4+0.01,y1-0.025],[0,0,0],'.',ms=15,color='0.5')
plt.text(y2+0.05,0.0,r'2',color='0.5',fontweight='bold')
plt.text(y4-0.05,0.0,r'4',color='0.5',fontweight='bold')
plt.text(y1,0.0,r'1',color='0.5',fontweight='bold')
plt.xlabel(r'$\sin{\epsilon}$ $\cos{(\psi+\Omega)}$')
plt.ylabel(r'$\sin{\epsilon}$ $\sin{(\psi+\Omega)}$')
plt.xlim(-1.1,1.1)
plt.ylim(-1.1,1.1)

if (sys.argv[1] == 'pdf'):
    fig.savefig('CassiniStatesSection.pdf')
if (sys.argv[1] == 'png'):
    fig.savefig('CassiniStatesSection.png')
plt.close()
