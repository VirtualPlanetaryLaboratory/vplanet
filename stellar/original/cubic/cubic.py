'''
TEST ROUTINE FOR CUBIC.C
'''

import ctypes
import numpy as np
import matplotlib.pyplot as plt
import sys
from scipy.interpolate import interp1d
from numpy.ctypeslib import ndpointer

d1ptr = ndpointer(dtype=ctypes.c_double, ndim=1)
iptr = ndpointer(dtype=ctypes.c_int, ndim=1)
lib = ctypes.CDLL('cubic.so')

fvInitSpline = lib.fvInitSpline
fvInitSpline.argtypes = [d1ptr,d1ptr,ctypes.c_int,ctypes.c_double,ctypes.c_double,d1ptr]

fdInterpSpline = lib.fdInterpSpline
fdInterpSpline.restype = ctypes.c_double
fdInterpSpline.argtypes = [d1ptr,d1ptr,d1ptr,ctypes.c_int,ctypes.c_double,iptr]

sz = 100
X = np.linspace(-3,1,sz)
Y = np.zeros(sz)
result = np.zeros(1)

XGRID = np.array([-2.5, -2.0, -1.5,-1.4, -1.3, -1.2, -1.0, -0.5, 0.5])
YGRID = np.sin(XGRID)
N = len(XGRID)

XTRUE = np.linspace(-3,1,sz)
YTRUE = np.sin(XTRUE)

for x,i in zip(X,range(sz)):
	d2 = np.zeros(N)
	iError = np.intc([1])
	fvInitSpline(XGRID,YGRID,N,1e30,1e30,d2)
	Y[i] = fdInterpSpline(XGRID,YGRID,d2,N,x,iError)

plt.plot(X,Y,'b-')
plt.plot(XGRID,YGRID,'bo')
plt.plot(XTRUE,YTRUE,'r--')

plt.show()
