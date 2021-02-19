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
out = vplot.GetOutput()
out2 = vplot.GetOutput('tides_only')

plt.figure(figsize=(8.5,6))
q = out.comp.SemiMajorAxis*(1-out.comp.Eccentricity)
plt.semilogy(out.comp.Time/1e9,out.comp.SemiMajorAxis,'k--',zorder=100,label='Semi-major axis (with encounters)')
plt.plot(out.comp.Time/1e9,q,'k-',zorder=100,label='Perihelion (with encounters)')

q = out2.comp.SemiMajorAxis*(1-out2.comp.Eccentricity)
plt.semilogy(out2.comp.Time/1e9,out2.comp.SemiMajorAxis,'--',c=vplot.colors.pale_blue,label='Semi-major axis (tide only)')
plt.plot(out2.comp.Time/1e9,q,'-',c=vplot.colors.pale_blue,label='Perihelion (tide only)')

plt.legend(loc='lower right', ncol=2, fontsize=10)
plt.ylabel('Distance (au)')
plt.xlabel('Time (Gyr)')
plt.ylim(10,2e4)

if (sys.argv[1] == 'pdf'):
    plt.savefig('GalaxyEffects.pdf')
if (sys.argv[1] == 'png'):
    plt.savefig('GalaxyEffects.png')
plt.close()
