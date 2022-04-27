import imp
import os
import subprocess as sub
import sys
from distutils.command.clean import clean
from glob import glob

from setuptools import Extension, find_packages, setup
from setuptools.command.build_ext import build_ext
from setuptools.command.develop import develop

# Read current code version
VersionPath = os.path.join(os.path.abspath("."), "VERSION")
VERSION = open(VersionPath, "r").read().split("\n")[0].strip()


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""

    def build_extensions(self):
        ct = self.compiler.compiler_type  # msvc or unix
        for ext in self.extensions:
            if ct == "msvc":
                ext.extra_compile_args = ["/EHsc"]
            else:
                ext.extra_compile_args = [
                    "-Wno-unused-variable",
                    "-Wno-missing-braces",
                    "-Wno-strict-prototypes",
                    "-Wno-sign-compare",
                    "-Wno-comment",
                ]
        build_ext.build_extensions(self)


class Develop(develop):
    """Custom develop command that clears build cache before install."""

    def run(self):
        c = clean(self.distribution)
        c.all = True
        c.finalize_options()
        c.run()
        develop.run(self)


macros = [
    ("VPLANET_PYTHON_INTERFACE", 1),
    (
        "VPLANET_VERSION",
        '"{}"'.format(VERSION),
    ),
]
if sys.platform.startswith("win"):
    macros += [("VPLANET_ON_WINDOWS", 1)]

ext_modules = [
    Extension(
        "vplanet.vplanet_core",
        glob("src/*.c"),
        include_dirs=["src"],
        language="c",
        define_macros=macros,
    )
]
cmdclass = {"build_ext": BuildExt, "develop": Develop}

# Vplanet suite of tools
vplanet_suite = [
    "vplot>=1.0.2",
    "vspace>=2.0.0",
    "bigplanet>=2.0.0",
    "multiplanet>=2.0.0",
]

setup(
    name="vplanet",
    author="Rory Barnes",
    author_email="rkb9@uw.edu",
    url="https://github.com/VirtualPlanetaryLaboratory/vplanet",
    version=VERSION,
    description="The virtual planet simulator",
    long_description=open("README.md", "r").read(),
    long_description_content_type="text/markdown",
    license="MIT",
    packages=["vplanet"],
    #install_requires=vplanet_suite + ["astropy>=3.0", "numpy", "tqdm", "pre-commit"],
    install_requires=vplanet_suite + ["astropy>=3.0", "numpy", "tqdm", "seaborn"],
    python_requires=">=3.6",
    # use_scm_version={
    # "write_to": os.path.join("vplanet", "vplanet_version.py"),
    # "write_to_template": '__version__ = "{version}"\n',
    # },
    ext_modules=ext_modules,
    cmdclass=cmdclass,
    include_package_data=True,
    zip_safe=False,
    entry_points={"console_scripts": ["vplanet=vplanet.wrapper:_entry_point"]},
)

#sub.call(["pre-commit", "install"])
