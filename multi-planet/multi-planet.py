import os
import multiprocessing as mp
import sys
import subprocess as sub
import mmap
import argparse
import h5py
import numpy as np
from collections import OrderedDict


# --------------------------------------------------------------------

## parallel implementation of running vplanet over a directory ##
def parallel_run_planet(input_file, cores):
    # gets the folder name with all the sims
    folder_name, infiles = GetDir(input_file)
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
        print('WARNING: multi-planet checkpoint file already exists!')
        print('Checking if checkpoint file is corrupt...')

        sub.run(['rm', checkpoint_file])
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
                    if line:
                        if line[0] == 'dStopTime':
                            stoptime = line[1]
                        if line[0] == 'dOutputTime':
                            ouputtime = line[1]
            expected_line_num = float(stoptime)/float(ouputtime)

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

    #get logfile name
    path_vpl = os.path.join(sims[0],'vpl.in')
    with open(path_vpl, 'r') as vpl:
        content = [line.strip().split() for line in vpl.readlines()]

        for line in content:
            if line:
                if line[0] == 'sSystemName':
                    system_name = line[1]

    logfile = system_name + ".log"

    lock = mp.Lock()
    workers = []
    for i in range(cores):
        workers.append(mp.Process(target=par_worker, args=(checkpoint_file,infiles,system_name,logfile,lock)))
    for w in workers:
        w.start()
    for w in workers:
        w.join()

    master_file = folder_name + '.hdf5'
    filelist = []

    for i in sims:
        single_folder = i.split('/')[-1]
        HDF5_File = single_folder + '.hdf5'
        HDF5_path = i + '/' + HDF5_File
        filelist.append(HDF5_path)

    data = OrderedDict()

    for f in filelist:
        data[f] = load(f)

    save(master_file, merge_data(data))


def GetDir(input_file):
    """ Give it input file and returns name of folder where simulations are located. """
    infiles = []
    # gets the folder name with all the sims
    with open(input_file, 'r') as vpl:
        content = [line.strip().split() for line in vpl.readlines()]
        for line in content:
            if line:
                if line[0] == 'destfolder':
                    folder_name = line[1]

                if line[0] == 'file':
                    infiles.append(line[1])

    if folder_name is None:
        print("Name of destination folder not provided in file '%s'."
                      "Use syntax 'destfolder <foldername>'"%inputf)


    if os.path.isdir(folder_name) == False:
        print("ERROR: Folder", folder_name, "does not exist in the current directory.")
        exit()

    return folder_name, infiles


def ProcessLogFile(logfile, data):
    prop = ''
    body = 'system'

    with open(logfile, 'r') as log:
        content = [line.strip() for line in log.readlines()]

    for line in content:

        if len(line) == 0:
            continue

        if line.startswith('-'):
            tmp_line = line.replace('-', '').replace(':', '').strip().replace(' ', '_')

            if tmp_line.startswith('INITIAL_SYSTEM_PROPERTIES'):
                prop = 'initial'

            if tmp_line.startswith('FINAL_SYSTEM_PROPERTIES'):
                prop = 'final'
                body = 'system'

            if tmp_line.startswith('BODY'):
                body = tmp_line[tmp_line.find('_')+1:].strip()

            continue

        if line.startswith('('):
            fv_param = line[1:line.find(')')].strip()
            units = line[line.find('[')+1:line.find(']')].strip()

            if not units:
                units = 'nd'

            fv_value = line[line.find(':')+1:].strip()
            key_name = body + '_' +  fv_param + '_' + prop

            if key_name in data:
                data[key_name].append(fv_value)

            else:
                data[key_name] = [units, fv_value]

        if (line.startswith('Output Order') or line.startswith('Grid Output Order')) and len(line[line.find(':'):]) > 1:

            parm_key = line[:line.find(':')].replace(' ', '')
            params = line[line.find(':') + 1:].strip().split()
            key_name = body + '_' + parm_key
            out_params = []

            for i in params:
                var = i[:i.find('[')]
                units = i[i.find('[') + 1:i.find(']')].strip()

                if not units:
                    units = 'nd'

                out_params.append([var, units])

            if key_name not in data:
                data[key_name] = out_params

    return data


def ProcessForwardfile(forwardfile, data, body, OutputOrder):

    header = []

    for i in OutputOrder:
        header.append([i][0][0])

    with open(forwardfile, 'r') as f:
        content = [line.strip().split() for line in f.readlines()]

        for row in content:
             sorted = [[content[j][i] for j in range(len(content))] for i in range(len(content[0]))]

        for i,row in enumerate(sorted):
            key_name = body + '_' + header[i] + '_forward'

            if key_name in data:
                data[key_name].append(row)

            else:
                data[key_name] = [row]

    return data


def CreateHDF5(data, system_name,infiles,logfile,h5filename):
    """
    ....
    """
    data = ProcessLogFile(logfile, data)
    for file in infiles:
        if file == 'vpl.in':
            continue
        else:
            body = file.split('.')[0]
            output = body + "_OutputOrder"
            if output in data:
                OutputOrder = data[output]
                forward_name = system_name + '.' + body + '.forward'
                data = ProcessForwardfile(forward_name, data, body, OutputOrder)

    with h5py.File(h5filename, 'w') as h:
        for k, v in data.items():
            if len(v) == 2:
                v_attr = v[0]
                v_value = [v[1]]
            else:
                v_value = v
                v_attr = ''

            h.create_dataset(k, data=np.array(v_value,dtype='S'),compression = 'gzip')
            h[k].attrs['Units'] = v_attr


def merge_data(data_list):

    """Merge dictionaries with data.

    Keyword arguments:
    data_list -- the dictionary with data dictionaries
    """

    data = None

    for f in data_list:
        if not data:
            data = data_list[f]
        else:
            #check.check_keys(data, data_list[f])
            #check.check_shapes(data, data_list[f])
            for key in data_list[f]:
                data[key] = np.append(data[key], data_list[f][key], axis=0)

    return data


def load(filename):

    """Load hdf5 file to data dictionary and return it.

    Keyword arguments:
    filename -- the full path to hdf5 file
    """

    with h5py.File(filename, 'r') as f:

        data = {}

        for key in f:
            data[key] = f[key][...]

    return data


def save(filename, data):

    """Create hdf5 file with given data.

    Keyword arguments:
    filename -- the full path to hdf5 file
    data -- dictionary with data
    """

    with h5py.File(filename, 'w') as f:

        for key in data:
            f.create_dataset(key, data[key].shape, dtype=data[key].dtype,compression='gzip')[...] = data[key]


## parallel worker to run vplanet ##
def par_worker(checkpoint_file,infiles,system_name,logfile,lock):

    while True:

        lock.acquire()
        datalist = []
        data = {}

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

        #runs vplanet on folder and writes the output to the log file
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
            print("Simulation ", folder, "completed")
            print("Creating HDF5 File...")
            single_folder = folder.split('/')[-1]
            HDF5_File = single_folder + '.hdf5'
            CreateHDF5(data, system_name, infiles, logfile, HDF5_File)
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

    max_cores = mp.cpu_count()
    parser = argparse.ArgumentParser(description="Using multi-processing to run a large number of simulations")
    parser.add_argument("InputFile", help="name of the vspace file")
    parser.add_argument("-c","--cores",type=int,default=max_cores,help="The total number of processors used")
    args = parser.parse_args()

    try:
        if sys.version_info >= (3, 0):
            help = sub.getoutput("vplanet -h")
        else:
            help = sub.check_output(["vplanet", "-h"])
    except OSError:
        raise Exception("Unable to call VPLANET. Is it in your PATH?")

    parallel_run_planet(args.InputFile,args.cores)
