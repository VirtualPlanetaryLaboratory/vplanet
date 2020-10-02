from vplot import GetOutput
import subprocess as sub
import numpy as np
import os
cwd = os.path.dirname(os.path.realpath(__file__))


def test_vspace_log():
    dir = cwd+'/Log_Test'
    # Removes the files created when vspace is ran
    sub.run(['rm', '-rf', dir],cwd=cwd)
    # Runs vspace
    sub.run(['python','../../vspace/vspace/vspace.py','vspace.in'],cwd=cwd)
    # Grab the output
    folders = sorted([f.path for f in os.scandir(dir) if f.is_dir()])
    semi = []
    for i in range(len(folders)):
        os.chdir(folders[i])
        with open('earth.in', 'r') as f:
            for newline in f:
                if newline.startswith("dSemi"):
                    newline = newline.strip().split()
                    semi.append(newline[1])
        os.chdir('../')
    for i in range(len(semi)):
        semi[i] = float(semi[i])

    assert np.isclose(semi[0], 1.0)
    assert np.isclose(semi[1], 2.15443469)
    assert np.isclose(semi[2], 4.64158883)
    assert np.isclose(semi[3], 10.0)
    assert np.isclose(semi[4], 21.5443469)
    assert np.isclose(semi[5], 46.41588834)
    assert np.isclose(semi[6], 100.0)
    assert np.isclose(semi[7], 215.443469)
    assert np.isclose(semi[8], 464.15888336)
    assert np.isclose(semi[9], 1000.0)

if __name__ == "__main__":
    test_vspace_log()
