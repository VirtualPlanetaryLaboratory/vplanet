#!/usr/bin/env python

# Read and plot results from vplanet.

import numpy as np
import argparse
import matplotlib.pyplot as plt
from matplotlib.pyplot import plot,ion
ion()

mass_e=5.972e24  
### Read in arguments ###
parser = argparse.ArgumentParser(description="""Analyze a vplanet run.""")
parser.add_argument('-r', '--read', action='store', type=str, dest='readfile',
                    required=False,
                    default='earth.earth.forward', help='name of .forward file to read in')
parser.add_argument('-sn', '--savename', action='store', type=str, dest='savename',
                    required=False,
                    default='plotEIt.txt', help='name of .txt file to save')
parser.add_argument('-s', '--save', action='store_const', dest='save', const=True,
                    default=False, required=False, help='save fig')
parser.add_argument('-t', '--text', action='store_const', dest='text', const=True,
                    default=False, required=False, help='save summary to text file')
args = parser.parse_args()
readfile = args.readfile
textfile = args.savename
#############################
# read in output variable list from earth.in
# Assumes saOutputOrder is followed by a blank line '\n'.
inputfile='earth.in'
s1='saOutputOrder'  #beginning of outputorder
s2='\n'             #end of outputorder
f=open(inputfile,'r')
lines=f.readlines()
i1=[i for i,s in enumerate(lines) if s.startswith(s1)][0]
i2=[i for i,s in enumerate(lines[i1:]) if s.startswith(s2)][0]+i1
#print(lines[i1:i2+1])
n_outputlines=i2-i1
labels=[]
for i in range(n_outputlines):
    tmp=[labels.append(x) for x in lines[i1+i].split()[:-1]]  #-1 to remove '$'
labels.append(lines[i2-1].split()[-1])  #get the last one.
labels=labels[1:]  #remove 'saOutputOrder'
#print(labels)

##############
# Read in output data.
#filename='sun.earth.forward'
#textfile='plot_vplanet.txt'
data=np.loadtxt(readfile)
print('Read '+readfile)

### Create dictionary and print to text file
datadict={}
for i,s in enumerate(labels): datadict[s]=data[:,i]
keys=datadict.keys()
nlabels=len(keys)
if '-Time' in keys: time=datadict['-Time']
scales={}
for i,s in enumerate(labels): scales[s]=1.  #default scaling.
masslabels=['-238UMassMan','-235UMassMan','-232ThMassMan','-40KMassMan',
            '-238UMassCore','-235UMassCore','-232ThMassCore','-40KMassCore',
            '-238UMassCrust','-235UMassCrust','-232ThMassCrust','-40KMassCrust']
for i in range(len(masslabels)):
    if masslabels[i] in keys: scales[masslabels[i]]=mass_e
### Individual arrays. ###
# Define default list of output.  Then check for their existence in datadict.
arraylabels=['-Time', '-TMan', '-TUMan', '-TLMan', '-TCMB', '-TCore', '-HflowUMan', '-HflowMeltMan', '-RadPowerMan', '-RadPowerCore', '-RadPowerCrust', '-HflowCMB', '-HflowSecMan', '-TDotMan', '-TDotCore', '-TJumpLMan', '-TJumpUMan', '-RIC', '-RayleighMan', '-ViscUMan', '-ViscJumpMan', '-MeltMassFluxMan', '-FMeltUMan', '-MagMom', '-CoreBuoyTherm', '-CoreBuoyCompo', '-CoreBuoyTotal', '-MagPauseRad', '-238UPowerMan', '-238UNumMan', '-238UMassMan', '-238UPowerCore', '-238UNumCore', '-238UMassCore', '-238UPowerCrust', '-238UNumCrust', '-238UMassCrust', '-235UPowerMan', '-235UNumMan', '-235UMassMan', '-235UPowerCore', '-235UNumCore', '-235UMassCore', '-235UPowerCrust', '-235UNumCrust', '-235UMassCrust', '-232ThPowerMan', '-232ThNumMan', '-232ThMassMan', '-232ThPowerCore', '-232ThNumCore', '-232ThMassCore', '-232ThPowerCrust', '-232ThNumCrust', '-232ThMassCrust', '-40KPowerMan', '-40KNumMan', '-40KMassMan', '-40KPowerCore', '-40KNumCore', '-40KMassCore', '-40KPowerCrust', '-40KNumCrust', '-40KMassCrust']

key='-Time'
if key in keys: Time=datadict[key]
key='-TMan'
if key in keys: TMan=datadict[key]
key='-TCore'
if key in keys: TCore=datadict[key]
key='-TUMan'
if key in keys: TUMan=datadict[key]
key='-TLMan'
if key in keys: TLMan=datadict[key]
key='-TCMB'
if key in keys: TCMB=datadict[key]
key='-HflowUMan'
if key in keys: HflowUMan=datadict[key]
key='-HflowMeltMan'
if key in keys: HflowMeltMan=datadict[key]
key='-RadPowerMan'
if key in keys: RadPowerMan=datadict[key]
key='-RadPowerCore'
if key in keys: RadPowerCore=datadict[key]
key='-RadPowerCrust'
if key in keys: RadPowerCrust=datadict[key]
key='-HflowCMB'
if key in keys: HflowCMB=datadict[key]
key='-HflowSecMan'
if key in keys: HflowSecMan=datadict[key]
key='-TDotMan'
if key in keys: TDotMan=datadict[key]
key='-TDotCore'
if key in keys: TDotCore=datadict[key]
key='-TJumpLMan'
if key in keys: TJumpLMan=datadict[key]
key='-TJumpUMan'
if key in keys: TJumpUMan=datadict[key]
key='-RIC'
if key in keys: RIC=datadict[key]
key='-RayleighMan'
if key in keys: RayleighMan=datadict[key]
key='-ViscUMan'
if key in keys: ViscUMan=datadict[key]
key='-ViscJumpMan'
if key in keys: ViscJumpMan=datadict[key]
key='-MeltMassFluxMan'
if key in keys: MeltMassFluxMan=datadict[key]
key='-FMeltUMan'
if key in keys: FMeltUMan=datadict[key]
key='-MagMom'
if key in keys: MagMom=datadict[key]
key='-CoreBuoyTherm'
if key in keys: CoreBuoyTherm=datadict[key]
key='-CoreBuoyCompo'
if key in keys: CoreBuoyCompo=datadict[key]
key='-CoreBuoyTotal'
if key in keys: CoreBuoyTotal=datadict[key]
key='-MagPauseRad'
if key in keys: MagPauseRad=datadict[key]
key='-238UPowerMan'
if key in keys: f238UPowerMan=datadict[key]
key='-238UNumMan'
if key in keys: f238NumMan=datadict[key]
key='-238UMassMan'
if key in keys: f238MassMan=datadict[key]
key='-238UPowerCore'
if key in keys: f238PowerCore=datadict[key]
key='-238UNumCore'
if key in keys: f238NumCore=datadict[key]
key='-238UMassCore'
if key in keys: f238MassCore=datadict[key]
key='-238UPowerCrust'
if key in keys: f238PowerCrust=datadict[key]
key='-238UNumCrust'
if key in keys: f238NumCrust=datadict[key]
key='-238UMassCrust'
if key in keys: f238MassCrust=datadict[key]
key='-235UPowerMan'
if key in keys: f235UPowerMan=datadict[key]
key='-235UNumMan'
if key in keys: f235UNumMan=datadict[key]
key='-235UMassMan'
if key in keys: f235UMassMan=datadict[key]
key='-235UPowerCore'
if key in keys: f235UPowerCore=datadict[key]
key='-235UNumCore'
if key in keys: f235UNumCore=datadict[key]
key='-235UMassCore'
if key in keys: f235UMassCore=datadict[key]
key='-235UPowerCrust'
if key in keys: f235UPowerCrust=datadict[key]
key='-235UNumCrust'
if key in keys: f235UNumCrust=datadict[key]
key='-235UMassCrust'
if key in keys: f235UMassCrust=datadict[key]
key='-232ThPowerMan'
if key in keys: f232ThPowerMan=datadict[key]
key='-232ThNumMan'
if key in keys: f232ThNumMan=datadict[key]
key='-232ThMassMan'
if key in keys: f232ThMassMan=datadict[key]
key='-232ThPowerCore'
if key in keys: f232ThPowerCore=datadict[key]    
key='-232ThNumCore'
if key in keys: f232ThNumCore=datadict[key]
key='-232ThMassCore'
if key in keys: f232ThMassCore=datadict[key]    
key='-232ThPowerCrust'
if key in keys: f232ThPowerCrust=datadict[key]
key='-232ThNumCrust'
if key in keys: f232ThNumCrust=datadict[key]
key='-232ThMassCrust'
if key in keys: f232ThMassCrust=datadict[key]
key='-40KPowerMan'
if key in keys: f40KPowerMan=datadict[key]
key='-40KNumMan'
if key in keys: f40KNumMan=datadict[key]
key='-40KMassMan'
if key in keys: f40KMassMan=datadict[key]
key='-40KPowerCore'
if key in keys: f40KPowerCore=datadict[key]
key='-40KNumCore'
if key in keys: f40KNumCore=datadict[key]
key='-40KMassCore'
if key in keys: f40KMassCore=datadict[key]
key='-40KPowerCrust'
if key in keys: f40KPowerCrust=datadict[key]
key='-40KNumCrust'
if key in keys: f40KNumCrust=datadict[key]
key='-40KMassCrust'
if key in keys: f40KMassCrust=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]
#key='-TMan'
#if key in keys: TMan=datadict[key]


### Plots ###
nfig=1
plt.figure(nfig,figsize=(15,10))
xlabel='Time (Gyr)'
plt.subplot(221)
plot(Time,TMan,label='TMan')
plot(Time,TUMan,label='TUMan')
plot(Time,TLMan,label='TLMan')
plot(Time,TCMB,label='TCMB')
plot(Time,TCore,label='TCore')
plt.xlabel(xlabel); plt.ylabel('T (K)')
plt.legend()
plt.subplot(222)
plot(Time,HflowUMan,label='HflowUMan')
plot(Time,HflowCMB,label='HflowCMB')
plot(Time,RadPowerMan,label='RadPowerMan')
plot(Time,RadPowerCore,label='RadPowerCore')
plt.xlabel(xlabel); plt.ylabel('Q (TW)')
plt.ylim(0,100)
plt.legend()
plt.subplot(223)
plot(Time,RIC,label='RIC')
plt.xlabel(xlabel);plt.ylabel('RIC')
plt.subplot(224)
plot(Time,MagMom,label='MagMom')
plt.xlabel(xlabel);plt.ylabel('MagMom')

if args.save:
    savename='plotEI.png'
    plt.savefig(savename)
    print('Saved '+savename)

### Write to text file. ###
if args.text:
    f=open(textfile,'w')
    ### Print initial step.
    f.write('At t=%.2f Gyr \n'%data[0,0])  #NOTE: this assumes time is first output var.
    for i,s in enumerate(labels): f.write(s+' = %.5e \n'%(datadict[s][0]*scales[s]))
    f.write('\n')
    ### Print final step.
    f.write('At t=%.2f Gyr \n'%data[-1,0])  #NOTE: this assumes time is first output var.
    for i,s in enumerate(labels): f.write(s+' = %.5e \n'%(datadict[s][-1]*scales[s]))
    f.close()
    print('Closed '+textfile)
### Print to screen
printlabels = ['-HflowUMan','-HflowMeltMan','-HflowCMB','-RadPowerMan','-RadPowerCore','-TUMan','-TCMB','-ViscUMan','-RIC','-MagMom']
for i in range(len(printlabels)):
    if printlabels[i] in datadict.keys(): print(printlabels[i]+' = %e'%datadict[printlabels[i]][-1])
print('HflowSurf = %e'%(datadict['-HflowUMan'][-1]+datadict['-HflowMeltMan'][-1]))
ind_icn=np.min(np.where(datadict['-RIC']>0.)[0])
print('ICN at %.3f Gyr, %.3f Ga.'%(time[ind_icn],time[-1]-time[ind_icn]))
