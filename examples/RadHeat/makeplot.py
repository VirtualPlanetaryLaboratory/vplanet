import numpy as np
import matplotlib.pyplot as plt
import vplot
import scipy.signal as sig
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

out = vplot.GetOutput()

fig = plt.figure(figsize=(8.5,8))
plt.subplot(2,2,1)
plt.plot(out.earth.Time,out.earth._40KPowerCore,color=vplot.colors.red,linestyle='-',label='$^{40}$K')
plt.plot(out.earth.Time,out.earth._232ThPowerCore,color=vplot.colors.orange,linestyle='-',label='$^{232}$Th')
plt.plot(out.earth.Time,out.earth._235UPowerCore,color=vplot.colors.pale_blue,linestyle='-',label='$^{235}$U')
plt.plot(out.earth.Time,out.earth._238UPowerCore,color=vplot.colors.purple,linestyle='-',label='$^{238}$U')
plt.plot(out.earth.Time,out.earth.RadPowerCore,'k-',label='Total')
plt.legend(loc='best')
plt.ylabel('Core Power (TW)')
plt.xlabel('Time (Gyr)')

plt.subplot(2,2,2)
plt.plot(out.earth.Time,out.earth._40KPowerMan,color=vplot.colors.red,linestyle='-')
plt.plot(out.earth.Time,out.earth._232ThPowerMan,color=vplot.colors.orange,linestyle='-')
plt.plot(out.earth.Time,out.earth._235UPowerMan,color=vplot.colors.pale_blue,linestyle='-')
plt.plot(out.earth.Time,out.earth._238UPowerMan,color=vplot.colors.purple,linestyle='-')
plt.plot(out.earth.Time,out.earth.RadPowerMan,'k-')
plt.ylabel('Mantle Power (TW)')
plt.xlabel('Time (Gyr)')

plt.subplot(2,2,3)
plt.plot(out.earth.Time,out.earth._40KPowerCrust,color=vplot.colors.red,linestyle='-')
plt.plot(out.earth.Time,out.earth._232ThPowerCrust,color=vplot.colors.orange,linestyle='-')
plt.plot(out.earth.Time,out.earth._235UPowerCrust,color=vplot.colors.pale_blue,linestyle='-')
plt.plot(out.earth.Time,out.earth._238UPowerCrust,color=vplot.colors.purple,linestyle='-')
plt.plot(out.earth.Time,out.earth.RadPowerCrust,'k-')
plt.ylabel('Crust Power (TW)')
plt.xlabel('Time (Gyr)')

plt.subplot(2,2,4)
plt.plot(out.earth.Time,out.earth._40KPowerTotal,color=vplot.colors.red,linestyle='-')
plt.plot(out.earth.Time,out.earth._232ThPowerTotal,color=vplot.colors.orange,linestyle='-')
plt.plot(out.earth.Time,out.earth._235UPowerTotal,color=vplot.colors.pale_blue,linestyle='-')
plt.plot(out.earth.Time,out.earth._238UPowerTotal,color=vplot.colors.purple,linestyle='-')
plt.plot(out.earth.Time,out.earth.RadPowerTotal,'k-')
plt.ylabel('Isotope Power (TW)')
plt.xlabel('Time (Gyr)')

vplot.make_pretty(fig)
if (sys.argv[1] == 'pdf'):
    plt.savefig('RadHeat.pdf', bbox_inches="tight", dpi=600)
if (sys.argv[1] == 'png'):
    plt.savefig('RadHeat.png', bbox_inches="tight", dpi=600)
plt.close()
