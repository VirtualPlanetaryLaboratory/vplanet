import numpy as np
import matplotlib.pyplot as plt
import vplot

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
plt.savefig('galactic_evol.pdf')
plt.close()
