"""
Raise error if help doesn't work. vplot depends on -H working.

"""
import subprocess


def test_Help():
    try:
        subprocess.check_output("bin/vplanet -H", shell=True)
    except subprocess.CalledProcessError:
        raise ValueError(f"Long help failed")
    else:
        pass
