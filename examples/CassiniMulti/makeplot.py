import numpy as np
import matplotlib.pyplot as plt
import vplot as vpl
import sys
import scipy.signal as sig
import subprocess

# Check correct number of arguments
if (len(sys.argv) != 2):
    print('ERROR: Incorrect number of arguments.')
    print('Usage: '+sys.argv[0]+' <pdf | png>')
    exit(1)
if (sys.argv[1] != 'pdf' and sys.argv[1] != 'png'):
    print('ERROR: Unknown file format: '+sys.argv[1])
    print('Options are: pdf, png')
    exit(1)

subprocess.call(['vplanet', 'vpl.in'])
out = vpl.GetOutput()

time = out.TGstar.Time/1e3

fig = plt.figure(figsize=(6.5,8))
plt.subplot(3,2,1)
plt.plot(time,out.TGb.Obliquity,color='k')
plt.plot(time,out.TGc.Obliquity,color=vpl.colors.red)
plt.ylabel(r'Obliquity ($^\circ$)')

plt.subplot(3,2,2)
plt.plot(time,out.TGb.Eccentricity,color='k')
plt.plot(time,out.TGc.Eccentricity,color=vpl.colors.red)
plt.ylabel('Eccentricity')

plt.subplot(3,2,3)
plt.plot(time,out.TGb.RotPer,color='k')
plt.plot(time,out.TGc.RotPer,color=vpl.colors.red)
plt.ylabel('Rotation Period (days)')

plt.subplot(3,2,4)
plt.plot(time,out.TGb.DynEllip,color='k',label='b')
plt.plot(time,out.TGc.DynEllip,color=vpl.colors.red,label='c')
plt.ylabel('Dynamical Ellipticity')
plt.legend(loc="upper right", fontsize=12, ncol=1)

plt.subplot(3,2,5)
plt.plot(time,out.TGb.CassiniOne,color='k')
plt.plot(time,out.TGc.CassiniOne,color=vpl.colors.red)
plt.xlabel('Time (kyr)')
plt.ylabel(u'$\sin{\Psi}$')

plt.subplot(3,2,6)
plt.plot(time,out.TGb.CassiniTwo,color='k')
plt.plot(time,out.TGc.CassiniTwo,color=vpl.colors.red)
plt.xlabel('Time (kyr)')
plt.ylabel(u'$\cos{\Psi}$')

vpl.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    fig.savefig('CassiniMulti.pdf')
if (sys.argv[1] == 'png'):
    fig.savefig('CassiniMulti.png')
plt.close()
