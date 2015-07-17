'''
rectify.py
----------

(C) Rodrigo Luger, Summer 2015

Rectifies the Baraffe grids and saves them to a binary file for easy C access.

Grids taken from
http://perso.ens-lyon.fr/isabelle.baraffe/BHAC15dir/BHAC15_tracks

The age grid is rebinned into logarithmically spaced ages using a cubic spline fit.

Final grid format is

  rad[M][A] = (double)
  logl[M][A] = (double)
  logt[M][A] = (double)

with shapes 

  (19+2, 500+2)

I added ghost cells at all boundaries with a simple (linear) extrapolation.

The ghost cells are located at

M = 0.0698, 1.41
A = 9.98137101e-04, 1.00182883e+01

'''

import numpy as np
import copy
import sys
import matplotlib.pyplot as plt
import struct
from scipy.interpolate import interp1d
import ctypes
from numpy.ctypeslib import ndpointer
d1ptr = ndpointer(dtype=ctypes.c_double, ndim=1)
iptr = ndpointer(dtype=ctypes.c_int, ndim=1)
lib = ctypes.CDLL('cubic/cubic.so')
fvInitSpline = lib.fvInitSpline
fvInitSpline.argtypes=[d1ptr,d1ptr,ctypes.c_int,ctypes.c_double,ctypes.c_double,d1ptr]
fdInterpSpline = lib.fdInterpSpline
fdInterpSpline.restype = ctypes.c_double
fdInterpSpline.argtypes = [d1ptr,d1ptr,d1ptr,ctypes.c_int,ctypes.c_double,iptr]

TOL = 0.005                                                                           # What's this?

def get_bounds(y):
  '''
  Find indices bounding the (bad) flat parts in the Baraffe luminosity evolution grid.
  
  '''
  
  z1 = (y[:-1] == y[1:])
  z2 = np.concatenate([[False],z1[:-1]])
  bounds = np.concatenate([(z1 != z2),[False]])
  bounds[0] = (y[0] == y[1]) 
  bounds[-1] = (y[-2] == y[-1])
  inds = np.where(bounds==True)[0]
  if (len(inds) % 2):
    print "ERROR! Something went wrong in get_bounds()."
  return inds

def curve(x0,y0):
  '''
  Remove the step-like nature of the Baraffe grids, interpolating to add
  some realistic curvature.
  
  '''
  
  x = copy.copy(x0)
  y = copy.copy(y0)
  
  # Get bounds
  inds = get_bounds(y)
  
  # Fix some stuff
  for i in range(0,len(inds),2):
    a = inds[i]
    b = inds[i+1]
    if (a > 0) and (b<len(y)-1):
      if (y[a] < y[a-1]) and (y[b] < y[b+1]):
        y[a+1] -= TOL
  
  # Linearly interpolate on flat parts
  for i in range(0,len(inds),2):
    a = inds[i]
    b = inds[i+1]
    if (a > 0) and (b<len(y)-1):
      if ((y[a] > y[a-1]) and (y[b+1] > y[b])) or ((y[a]<y[a-1]) and (y[b+1] < y[b])):
        # Bug fix
        if np.abs(y[a]-y[a-1])>0.03:
          y[a-1] = (y[a-1]+y[a])/2.
          print "WARNING: Large gradient in one of the curves. Manually verify!"
          lin = range(a,b+1)
          y[lin] = np.interp(x[lin],[x[a],x[b+1]],[y[a],y[b+1]])
        else:
          # Monotonic
          lin = range(a,b+1)
          y[lin] = np.interp(x[lin],[x[a-1],x[b+1]],[y[a-1],y[b+1]])
  return y

def get_grids(file):
  '''
  
  '''
  
  # Get number of masses
  with open(file,'r') as f:
    firstmass = 0
    mass = 0
    num_masses = 0
    for row in f:
      if '#' not in row:
        foo = row.split()
        if row[1] != " ":
          # OBSOLETE: Avoid silly formatting in the low Z file
          tmp = float(foo[0])
          if tmp != mass:
            if firstmass == 0: firstmass = tmp
            mass = tmp
            num_masses += 1

  # Now populate grids
  last_age = 0.
  with open(file,'r') as f:
    mass_arr = np.zeros(num_masses)
    mass_arr[0] = firstmass
    massnum = 0
    age_grid = [[] for i in range(num_masses)]
    teff_grid = [[] for i in range(num_masses)]
    logl_grid = [[] for i in range(num_masses)]
    rad_grid = [[] for i in range(num_masses)]
    for row in f:
      if '#' not in row:
        foo = row.split()
        tmp = float(foo[0])
        if tmp != mass_arr[massnum]:
          massnum += 1
          mass_arr[massnum] = tmp
        age = 10**(float(foo[1]) - 9.)                                                # Convert to GYR (linear)
        if age > last_age:                                                            # Prevent duplicate age values due to round-off
          age_grid[massnum] += [age]       
          teff_grid[massnum] += [float(foo[2])]
          logl_grid[massnum] += [float(foo[3])]
          rad_grid[massnum] += [float(foo[5])]
        last_age = age
  
  return mass_arr, age_grid, teff_grid, logl_grid, rad_grid

def save_bin(file,num_ages=500,verbose=False,curveit=False):
  '''
  
  '''
  
  # Read file
  mass_arr, age_grid, teff_grid, logl_grid, rad_grid = get_grids(file)
  num_masses = len(mass_arr)

  # Rectify age grid
  age_arr = np.logspace(-3,1,num_ages)
  teff_grid_rect = copy.deepcopy(teff_grid)
  logl_grid_rect = copy.deepcopy(logl_grid)
  rad_grid_rect = copy.deepcopy(rad_grid)

  for j in range(num_masses):    
    XGRID = np.array(age_grid[j])
    TGRID = np.array(teff_grid[j])
    LGRID = np.array(logl_grid[j])
    RGRID = np.array(rad_grid[j])

    if curveit: LGRID = curve(XGRID,LGRID)
    
    N = len(XGRID)
    
    # Trim the age array so that the interpolation will work
    a_min = np.min(XGRID)
    a_max = np.max(XGRID)
    i_min = np.where(age_arr>a_min)[0][0]
    if age_arr[-1] > a_max:
      i_max = np.where(age_arr>a_max)[0][0]
    else:
      i_max = len(age_arr)+1
    age_arr_trimmed = age_arr[i_min:i_max]

    # Cubic spline interpolation
    for YGRID,gnum in zip([TGRID,LGRID,RGRID],[0,1,2]):
      cub = np.zeros(len(age_arr_trimmed))
      d2 = np.zeros(N)
      for k in range(len(cub)):
        iError = np.intc([1])
        fvInitSpline(XGRID,YGRID,N,1e30,1e30,d2)
        cub[k] = fdInterpSpline(XGRID,YGRID,d2,N,age_arr_trimmed[k],iError)
        if (np.isnan(cub[k]) or iError != 0):
          import pdb; pdb.set_trace()
          print "ERROR! Something went wrong in the cubic interpolation."
          sys.exit()        
      if gnum==0:
        teff_grid_rect[j] = [np.nan]*(i_min) + list(cub) + [np.nan]*(len(age_arr)-i_max)
      elif gnum==1:
        logl_grid_rect[j] = [np.nan]*(i_min) + list(cub) + [np.nan]*(len(age_arr)-i_max)
      elif gnum==2:
        rad_grid_rect[j] = [np.nan]*(i_min) + list(cub) + [np.nan]*(len(age_arr)-i_max)
  
  # Add ghost cells to arrays
  mass_arr_final = np.concatenate([[mass_arr[0] - (mass_arr[1]-mass_arr[0])/10.],mass_arr,[mass_arr[-1] + (mass_arr[-1]-mass_arr[-2])/10.]])  
  age_arr_final = np.concatenate([[age_arr[0] - (age_arr[1]-age_arr[0])/10.],age_arr,[age_arr[-1] + (age_arr[-1]-age_arr[-2])/10.]])

  # Create final grids with ghost cells
  teff_grid_final = np.zeros((num_masses+2,num_ages+2))
  logl_grid_final = np.zeros((num_masses+2,num_ages+2))
  rad_grid_final = np.zeros((num_masses+2,num_ages+2))
  for j in range(num_masses):
    for k in range(num_ages):
      teff_grid_final[j+1][k+1] = teff_grid_rect[j][k]
      logl_grid_final[j+1][k+1] = logl_grid_rect[j][k]
      rad_grid_final[j+1][k+1] = rad_grid_rect[j][k]
  
  # Interpolate on boundaries to fill in ghost cells
  for k in range(num_ages+2):
    teff_grid_final[0][k] = teff_grid_final[1][k] - (mass_arr_final[1]-mass_arr_final[0])*(teff_grid_final[2][k]-teff_grid_final[1][k])/(mass_arr_final[2]-mass_arr_final[1])
    teff_grid_final[-1][k] = teff_grid_final[-2][k] + (mass_arr_final[-1]-mass_arr_final[-2])*(teff_grid_final[-2][k]-teff_grid_final[-3][k])/(mass_arr_final[-2]-mass_arr_final[-3])
    logl_grid_final[0][k] = logl_grid_final[1][k] - (mass_arr_final[1]-mass_arr_final[0])*(logl_grid_final[2][k]-logl_grid_final[1][k])/(mass_arr_final[2]-mass_arr_final[1])
    logl_grid_final[-1][k] = logl_grid_final[-2][k] + (mass_arr_final[-1]-mass_arr_final[-2])*(logl_grid_final[-2][k]-logl_grid_final[-3][k])/(mass_arr_final[-2]-mass_arr_final[-3])
    rad_grid_final[0][k] = rad_grid_final[1][k] - (mass_arr_final[1]-mass_arr_final[0])*(rad_grid_final[2][k]-rad_grid_final[1][k])/(mass_arr_final[2]-mass_arr_final[1])
    rad_grid_final[-1][k] = rad_grid_final[-2][k] + (mass_arr_final[-1]-mass_arr_final[-2])*(rad_grid_final[-2][k]-rad_grid_final[-3][k])/(mass_arr_final[-2]-mass_arr_final[-3])
  for j in range(num_masses+2):
    teff_grid_final[j][0] = teff_grid_final[j][1] - (age_arr_final[1]-age_arr_final[0])*(teff_grid_final[j][2]-teff_grid_final[j][1])/(age_arr_final[2]-age_arr_final[1])
    teff_grid_final[j][-1] = teff_grid_final[j][-2] + (age_arr_final[-1]-age_arr_final[-2])*(teff_grid_final[j][-2]-teff_grid_final[j][-3])/(age_arr_final[-2]-age_arr_final[-3])
    logl_grid_final[j][0] = logl_grid_final[j][1] - (age_arr_final[1]-age_arr_final[0])*(logl_grid_final[j][2]-logl_grid_final[j][1])/(age_arr_final[2]-age_arr_final[1])
    logl_grid_final[j][-1] = logl_grid_final[j][-2] + (age_arr_final[-1]-age_arr_final[-2])*(logl_grid_final[j][-2]-logl_grid_final[j][-3])/(age_arr_final[-2]-age_arr_final[-3])
    rad_grid_final[j][0] = rad_grid_final[j][1] - (age_arr_final[1]-age_arr_final[0])*(rad_grid_final[j][2]-rad_grid_final[j][1])/(age_arr_final[2]-age_arr_final[1])
    rad_grid_final[j][-1] = rad_grid_final[j][-2] + (age_arr_final[-1]-age_arr_final[-2])*(rad_grid_final[j][-2]-rad_grid_final[j][-3])/(age_arr_final[-2]-age_arr_final[-3])

  # Save the rectified grids
  for grid,fname in zip([np.log10(teff_grid_final),logl_grid_final,rad_grid_final],['logt','logl','rad']):
    grid = np.array(grid).flatten()
    sz = len(grid)
    with open(fname + ".bin", "wb") as file:
      file.write(struct.pack(str(sz)+'d', *grid))
  
  print "Save successful."
  if verbose:
    print "Mass array: (%d+2)" % (num_masses), [m for m in mass_arr_final]
    print "Age array: (%d+2)" % (num_ages), [a for a in age_arr_final]

save_bin('baraffe2015.dat',num_ages=500,curveit=False)