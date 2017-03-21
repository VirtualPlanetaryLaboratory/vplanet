import numpy as np
import matplotlib.pyplot as plt
import vplot as vpl

out = vpl.GetOutput()

plt.subplot(2,3,1)
plt.plot(out.b.Time, out.b.SemiMajorAxis,'b-')

plt.subplot(2,3,4)
plt.plot(out.c.Time, out.c.SemiMajorAxis,'b-')

plt.show()