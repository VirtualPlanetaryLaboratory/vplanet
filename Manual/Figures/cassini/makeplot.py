import numpy as np
import matplotlib.pyplot as plt
import vplot

out = vplot.GetOutput()

fig = plt.figure(figsize=(8.5,6))
plt.subplot(2,2,1)
plt.plot(out.b.Time/1e5,out.b.RotPer)
plt.ylabel('Rotation Period (days)')
plt.xlim(0,5)

plt.subplot(2,2,2)
plt.plot(out.b.Time/1e5,out.b.CassiniOne)
plt.ylabel(u'$\sin{\Psi}$')
plt.xlim(0,5)

plt.subplot(2,2,3)
plt.semilogy(out.b.Time/1e5,out.b.Obliquity)
plt.xlabel('Time (10$^5$ yr)')
plt.ylabel('Obliquity ($^{\circ}$)')
plt.xlim(0,5)

plt.subplot(2,2,4)
plt.plot(out.b.Time/1e5,out.b.CassiniTwo)
plt.xlabel('Time (10$^5$ yr)')
plt.ylabel(u'$\cos{\Psi}$')
plt.xlim(0,5)

vplot.make_pretty(fig)
plt.savefig('cassini_damp.pdf')
plt.close()
