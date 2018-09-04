#!/usr/local/bin/python3.7

# Script to run all vspace runs
import sys
import string
import subprocess as subp
import matplotlib.pyplot as plt
import vplot as vpl

# Arrays ecc,obl,heat now contain the data to make the figure


f, ax = plt.subplots(3,2,figsize=(8.5,11))

nlines=1548
t=[0 for j in range(nlines)]
erot=[0 for j in range(nlines)]
eobl=[0 for j in range(nlines)]
a=[0 for j in range(nlines)]
e=[0 for j in range(nlines)]
en=[0 for j in range(nlines)]
am=[0 for j in range(nlines)]

efile=open('earth.backward','r')
mfile=open('moon.backward','r')

j=0
for line in efile:
    words=str.split(line)
    t[j]=float(words[0])
    erot[j]=float(words[1])*24 # Convert to hours
    eobl[j]=float(words[2])
    j += 1

j=0
for line in mfile:
    words=str.split(line)
    t[j]=float(words[0])
    a[j]=float(words[1])
    e[j]=float(words[2])
    en[j]=float(words[3])
    am[j]=float(words[4])

    # Calculate relative change in Energy and Ang. Mom.
    if j > 0:
        en[j]=(en[j] - en[0])/en[0]
        en[j] /= 1e-9
        am[j]=(am[j] - am[0])/am[0]

    j += 1

#Tidy up first data point
en[0]=en[1]
am[0]=am[1]

# Semi-major axis
ax[0][0].set_xlim(-1.75,0)
ax[0][0].set_ylim(0,65)
ax[0][0].tick_params(axis='both', labelsize=20)
ax[0][0].set_ylabel(r'Semi-major Axis (R$_\oplus$)', fontsize=20)
ax[0][0].set_xlabel('Time (Gyr)',fontsize=20)
ax[0][0].set_xticks([-1.5,-1,-0.5,0])
ax[0][0].plot(t,a,linestyle='-', color='k', linewidth=2)

ax[0][1].set_xlim(-1.75,0)
ax[0][1].set_ylim(-0.02,25)
ax[0][1].tick_params(axis='both', labelsize=20)
ax[0][1].set_ylabel('Earth\'s Rot. Period (hr)', fontsize=20)
ax[0][1].set_xlabel('Time (Gyr)',fontsize=20)
ax[0][1].set_xticks([-1.5,-1,-0.5,0])
ax[0][1].plot(t,erot,linestyle='-', color='k', linewidth=2)

ax[1][0].set_xlim(-1.75,0)
ax[1][0].set_ylim(0,0.07)
ax[1][0].tick_params(axis='both', labelsize=20)
ax[1][0].set_ylabel('Eccentricity', fontsize=20)
ax[1][0].set_xlabel('Time (Gyr)',fontsize=20)
ax[1][0].set_xticks([-1.5,-1,-0.5,0])
ax[1][0].plot(t,e,linestyle='-', color='k', linewidth=2)

ax[1][1].set_xlim(-1.75,0)
ax[1][1].set_ylim(-0.02,25)
ax[1][1].tick_params(axis='both', labelsize=20)
ax[1][1].set_ylabel(r'Obliquity ($^\circ$)', fontsize=20)
ax[1][1].set_xlabel('Time (Gyr)',fontsize=20)
ax[1][1].set_xticks([-1.5,-1,-0.5,0])
ax[1][1].plot(t,eobl,linestyle='-', color='k', linewidth=2)

ax[2][0].set_xlim(-1.75,0)
ax[2][0].set_ylim(-10,10)
ax[2][0].tick_params(axis='both', labelsize=20)
ax[2][0].set_ylabel(r'Change in Energy($\times 10^{-9}$)', fontsize=20)
ax[2][0].set_xlabel('Time (Gyr)',fontsize=20)
ax[2][0].set_xticks([-1.5,-1,-0.5,0])
ax[2][0].plot(t,en,linestyle='-', color='k', linewidth=2)

ax[2][1].set_xlim(-1.75,0)
ax[2][1].set_ylim(-1e-5,5e-5)
ax[2][1].tick_params(axis='both', labelsize=20)
ax[2][1].set_ylabel('Change in Ang. Mom.', fontsize=20)
ax[2][1].set_xlabel('Time (Gyr)',fontsize=20)
ax[2][1].set_xticks([-1.5,-1,-0.5,0])
ax[2][1].plot(t,am,linestyle='-', color='k', linewidth=2)

plt.tight_layout()

plt.savefig('earthmoontides.png')
