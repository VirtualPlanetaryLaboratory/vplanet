import os
import subprocess
import sys

<<<<<<< HEAD
subdir = sorted([f.name for f in os.scandir('.') if f.is_dir()])

# First make debug
print('Making VPLanet...')
sys.stdout.flush()
os.chdir('../')
#print(os.getcwd())
#subprocess.run(['make opt >& /dev/null'], shell=True)
with open('make_log','w') as m:
    
    subprocess.run('make opt',shell=True, stderr= m,stdout= m)
    subprocess.run(['rm','make_log'])

os.chdir('tests/')
=======
import numpy as np

subdir = [f.name for f in os.scandir(".") if f.is_dir()]

# First make debug
sys.stdout.write("Making VPLanet...")
sys.stdout.flush()
os.chdir("..")
subprocess.run(["make debug >& /dev/null"], shell=True)
os.chdir("tests")
>>>>>>> 0b3ea1c78b49901fe8c6b823f81d8ff0c84f81f8

print("done.")

tot_fail = 0
tot_test = 0
for sub in subdir:
    tot_test += 1
    sys.stdout.write(sub)
    #sys.stdout.write("")
    sys.stdout.flush()
    os.chdir(sub)
<<<<<<< HEAD
    fout = sub +'.valgrind'
    #cmd='/usr/bin/valgrind --track-origins=yes ../../bin/vplanet vpl.in &> '+ fout
    cmd='/usr/bin/valgrind --track-origins=yes ../../bin/vplanet vpl.in '

    #print(cmd)
    #cmd = cmd.split()
    
    with open(fout,"w+") as f:
        subprocess.run(cmd, shell=True,stdout=f,stderr=f)
=======
    fout = sub + ".valgrind"
    cmd = "valgrind --track-origins=yes ../../vplanet vpl.in >& " + fout
    subprocess.run(cmd, shell=True)
>>>>>>> 0b3ea1c78b49901fe8c6b823f81d8ff0c84f81f8
    f = open(fout, "r")
    last_line = f.readlines()[-1]
    #print(last_line)
    f.close()
    words = last_line.split()
    n_errors = int(words[3])
    if n_errors > 0:
        tot_fail += 1
<<<<<<< HEAD
    print(': '+repr(n_errors)+' error(s)')
    os.chdir('../')
    

print('Done! ')

if (tot_fail == 0):
    print('VPLanet is mem-check-clean!')
    assert(True)
else:
    print(repr(tot_fail)+'/'+repr(tot_test)+' test(s) failed.')
    assert(False)
=======
    print(": " + repr(n_errors) + " error(s)")
    os.chdir("..")

sys.stdout.write("Done! ")

if tot_fail == 0:
    print("VPLanet is mem-check-clean!")
else:
    print(repr(tot_fail) + "/" + repr(tot_test) + " test(s) failed.")
>>>>>>> 0b3ea1c78b49901fe8c6b823f81d8ff0c84f81f8
