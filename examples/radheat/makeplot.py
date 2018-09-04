import numpy as np
import matplotlib.pyplot as plt
import vplot
import scipy.signal as sig
plt.rcParams["text.usetex"]=True
plt.rcParams["text.latex.unicode"]=True

out = vplot.GetOutput()

fig = plt.figure(figsize=(8.5,6))
plt.subplot(2,2,1)
plt.plot(out.earth.Time,out.earth.40KPowerCore,'k-')
plt.plot(out.earth.Time,out.earth.232ThPowerCore,color=vplot.colors.orange,'-')
plt.plot(out.earth.Time,out.earth.235UPowerCore,color=vplot.colors.paleblue,'-')
plt.plot(out.earth.Time,out.earth.238UPowerCore,color=vplot.colors.purple,'-')
plt.ylabel('Core Power (TW)')

#plt.subplot(2,2,2)

#plt.subplot(2,2,3)

#plt.subplot(2,2,4)

vplot.make_pretty(fig)
plt.savefig('radheat.pdf')
plt.close()
