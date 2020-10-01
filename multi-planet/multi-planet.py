import os
import multiprocessing as mp
import sys
import subprocess as sub
import math
import mmap
from concurrent.futures import ProcessPoolExecutor
import time

# --------------------------------------------------------------------

## parallel implementation of running vplanet over a directory ##
def parallel_run_planet(input_file, cores):
    # gets the folder name with all the sims
    with open(input_file, 'r') as vsf:
        vspace_all = vsf.readlines()
        dest_line = vspace_all[1]
        folder_name = dest_line.strip().split(None, 1)[1]
        if folder_name is None:
            raise IOError("Name of destination folder not provided in file '%s'. Use syntax 'destfolder <foldername>'"%inputf)

    if os.path.isdir(folder_name) == False:
        raise Exception("Vspace must be ran prior to running Multi-Planet")

    #gets the list of sims
    sims = sorted([f.path for f in os.scandir(folder_name) if f.is_dir()])


    #initalizes the checkpoint file
    checkpoint_file = os.getcwd() + '/' + '.' + folder_name

    #checks if the files doesn't exist and if so then it creates it
    if os.path.isfile(checkpoint_file) == False:
        with open(checkpoint_file,'w') as cp:
            cp.write('Vspace File: ' + os.getcwd() + '/' + input_file + '\n')
            cp.write('Total Number of Simulations: '+ str(len(sims)) + '\n')
            for f in range(len(sims)):
                cp.write(sims[f] + " " + "-1 \n")
            cp.write('THE END \n')

    #if it does exist, it checks for any 0's (sims that didn't complete) and
    #changes them to -1 to be re-ran
    else:
        datalist = []

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

            for l in datalist:
                if l[1] == '0':
                    folder = l[0]
                    l[1] = '-1'

        if datalist[-1][1] != 'END':
            print('WARNING: multiplanet checkpoint file is corrupted!')
            print('Attempting to recreate...')

            #restarts the checkpoint file from scratch
            with open(checkpoint_file,'w') as cp:
                cp.write('Vspace File: ' + os.getcwd() + '/' + input_file + '\n')
                cp.write('Total Number of Simulations: '+ str(len(sims)) + '\n')
                for f in range(len(sims)):
                    cp.write(sims[f] + " " + "-1 \n")
                cp.write('THE END \n')

            datalist = []

            with open(checkpoint_file, 'r') as f:
                for newline in f:
                    datalist.append(newline.strip().split())

            for i in range(len(sims)):
                sim_dir = sims[i]

                vp = os.path.join(sim_dir, 'vpl.in')
                with open(vp, 'r') as vpl:
                    content = [line.strip().split() for line in vpl.readlines()]
                    for line in content:
                        if line[0] == 'dStopTime':
                            stoptime = line[1]
                        if line[0] == 'dOutputTime':
                            ouputtime = line[1]
                expected_line_num = int(stoptime)/int(ouputtime)

                fd = [f for f in os.listdir(sim_dir) if f.endswith('foward')]
                if len(fd) == 0:
                    for l in datalist:
                        if l[0] == sim_dir:
                            l[1] = '-1'
                else:
                    fd = os.path.join(sim_dir, fd[0])
                    with open(fd, 'r') as f:
                        buf = mmap.mmap(f.fileno(),0)
                        lines = 0
                        readline = buf.readline
                        while readline():
                            lines += 1
                        last = readline[-1]
                        if last == stoptime and lines == expected_line_num:
                            for l in datalist:
                                if l[0] == sim_dir:
                                    l[1] = '1'
                        else:
                            for l in datalist:
                                if l[0] == sim_dir:
                                    l[1] = '-1'

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')

    lock = mp.Lock()
    workers = []
    for i in range(cores):
        workers.append(mp.Process(target=par_worker, args=(checkpoint_file, lock, sims)))
    for w in workers:
        w.start()
    for w in workers:
        w.join()


## parallel worker to run vplanet ##
def par_worker(checkpoint_file, lock, sims):

    while True:

        lock.acquire()
        datalist = []

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

        folder = ''

        for l in datalist:
            if l[1] == '-1':
                folder = l[0]
                l[1] = '0'
                break

        if not folder:
            lock.release()
            return

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')

        lock.release()

        os.chdir(folder)

        with open('vplanet_log','a+') as vplf:
            vplanet = sub.Popen("vplanet vpl.in", shell=True, stdout=sub.PIPE, stderr=sub.PIPE, universal_newlines=True)
            return_code = vplanet.poll()
            for line in vplanet.stderr:
                vplf.write(line)

            for line in vplanet.stdout:
                vplf.write(line)

        lock.acquire()
        datalist = []

        with open(checkpoint_file, 'r') as f:
            for newline in f:
                datalist.append(newline.strip().split())

        if return_code is None:
            for l in datalist:
                if l[0] == folder:
                    l[1] = '1'
                    break

            print(folder, "completed")
        else:
            for l in datalist:
                if l[0] == folder:
                    l[1] = '-1'
                    break
            print("\t" + folder, "failed")

        with open(checkpoint_file, 'w') as f:
            for newline in datalist:
                f.writelines(' '.join(newline)+'\n')
        lock.release()

        os.chdir("../../")

if __name__ == "__main__":
    if len(sys.argv) < 3:
      raise Exception("Must enter an input file name and number of cores")
    else:
        par_input = sys.argv[1]
        cores = int(sys.argv[2])

    if cores > mp.cpu_count():
        raise Exception("Error: the maximum number of cores is %s" %mp.cpu_count())


    try:
        if sys.version_info >= (3, 0):
            help = sub.getoutput("vplanet -h")
        else:
            help = sub.check_output(["vplanet", "-h"])
    except OSError:
        raise Exception("Unable to call VPLANET. Is it in your PATH?")

    parallel_time = 0
    start = time.perf_counter()
    parallel_run_planet(par_input, cores)
    parallel_time += time.perf_counter() - start

    print('Duration: {:.2f} ms'.format(parallel_time*1000))
