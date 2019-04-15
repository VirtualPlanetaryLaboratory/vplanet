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

cpl = vplot.GetOutput('CPL')
ctl = vplot.GetOutput('CTL')

fig, ax = plt.subplots(1, 2, figsize=(12, 4))

ax[0].plot(cpl.d.Time,cpl.d.RotPer,color='k',linestyle='dashed',label='CPL')
ax[0].plot(ctl.d.Time,ctl.d.RotPer,color='k',linestyle='solid',label='CTL')
ax[0].set_xscale('log')
ax[0].set_xlabel('Time (Years)')
ax[0].set_ylabel('Rotation Period (d)')
ax[0].legend(loc='upper left',fontsize=10)

ax[1].plot(cpl.d.Time,cpl.d.Obliquity,color='k',linestyle='dashed')
ax[1].plot(ctl.d.Time,ctl.d.Obliquity,color='k',linestyle='solid')
ax[1].set_xscale('log')
ax[1].set_xlabel('Time (Years)')
ax[1].set_ylabel(r'Oblqiuity ($^\circ$)')

if (sys.argv[1] == 'pdf'):
    plt.savefig('TideLock.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('TideLock.png')
plt.close()
