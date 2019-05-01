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
# inc = 0.1
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
# eps = 0.6
# g = -eps*alpha

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


xobl = np.sin(out.b.Obliquity*np.pi/180)*np.cos((out.b.PrecA+out.b.LongA)*np.pi/180)
yobl = np.sin(out.b.Obliquity*np.pi/180)*np.sin((out.b.PrecA+out.b.LongA)*np.pi/180)

#solve quartic equation for roots (state obliquities)

a = alpha/(4*g*1j)
b = np.cos(inc)/(2j)-np.sin(inc)/2
c = 0
d = -np.cos(inc)/(2j)-np.sin(inc)/2
e = -a

p = (8*a*c-3*b**2)/(8*a**2)
q = (b**3-4*a*b*c+8*a**2*d)/(8*a**3)
del0 = c**2 - 3*b*d+12*a*e
del1 = 2*c**3-9*b*c*d + 27*b**2*e + 27*a*d**2 - 72*a*c*e
Q = ((del1+np.sqrt(del1**2-4*del0**3))/2)**(1./3)
S = 0.5*np.sqrt(-2/3*p+1/(3*a)*(Q+del0/Q))

x1 = -b/(4*a)-S+0.5*np.sqrt(-4*S**2-2*p+q/S)
x2 = -b/(4*a)-S-0.5*np.sqrt(-4*S**2-2*p+q/S)
x3 = -b/(4*a)+S+0.5*np.sqrt(-4*S**2-2*p-q/S)
x4 = -b/(4*a)+S-0.5*np.sqrt(-4*S**2-2*p-q/S)

theta1 = np.arctan2(np.imag(x1),np.real(x1))
theta2 = np.arctan2(np.imag(x2),np.real(x2))
theta3 = np.arctan2(np.imag(x3),np.real(x3))
theta4 = np.arctan2(np.imag(x4),np.real(x4))

#roots get mixed up in quartic solution so need to assign them manually

# obl4 = np.arccos(-g*np.cos(inc)/alpha)
obl4 = theta4
y4 = np.sin(obl4)
z4 = np.cos(obl4)

# obl2 = -np.arccos(-g*np.cos(inc)/alpha)
obl2 = theta1
y2 = np.sin(obl2)

# obl1 = np.arctan2(np.sin(inc),1-alpha/g)
obl1 = theta3
y1 = np.sin(obl1)

H4 = H0 - 0.5*alpha*z4**2 - g*(y4*np.sin(inc)+z4*np.cos(inc))


fig = plt.figure(figsize=(8,8))
plt.contour(Y,X,H.T,50,colors='0.5')
plt.contour(Y,X,H.T,levels=[1.00005*H4],colors='k')
plt.plot(xc,yc,'-',color='0.5')
plt.plot(xobl[out.b.Time>400e3],yobl[out.b.Time>400e3],'k.')
plt.plot([y2,y4,y1],[0,0,0],'.',ms=15,color='0.5')
plt.text(y2,0.03,r'2',color='0.5',fontweight='bold',zorder=1000)
plt.text(y4-0.06,0.0,r'4',color='0.5',fontweight='bold')
plt.text(y1,0.02,r'1',color='0.5',fontweight='bold')
plt.xlabel(r'$\sin{\epsilon}$ $\cos{(\psi+\Omega)}$')
plt.ylabel(r'$\sin{\epsilon}$ $\sin{(\psi+\Omega)}$')
plt.xlim(-1.1,1.1)
plt.ylim(-1.1,1.1)

if (sys.argv[1] == 'pdf'):
    fig.savefig('CassiniStatesSection.pdf')
if (sys.argv[1] == 'png'):
    fig.savefig('CassiniStatesSection.png')
plt.close()
