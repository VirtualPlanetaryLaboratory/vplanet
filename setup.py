from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.develop import develop
from distutils.command.clean import clean
from glob import glob
import sys


# Read current code version
VERSION = open("VERSION", "r").read().split("\n")[0].strip()


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
    ("VPLANET_VERSION", '"{}"'.format(VERSION),),
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
    "vspace>=1.0.2",
    "bigplanet>=1.0.1",
    "multiplanet>=1.0.1",
]

setup(
    name="vplanet",
    author="Rory Barnes",
    author_email="rkb9@uw.edu",
    version=VERSION,
    url="https://github.com/VirtualPlanetaryLaboratory/vplanet",
    description="The virtual planet simulator",
    long_description=open("README.md", "r").read(),
    long_description_content_type="text/markdown",
    license="MIT",
    packages=["vplanet"],
    install_requires=vplanet_suite + ["astropy>=3.0", "numpy", "tqdm",],
    python_requires=">=3.6",
    ext_modules=ext_modules,
    cmdclass=cmdclass,
    include_package_data=True,
    zip_safe=False,
    entry_points={"console_scripts": ["vplanet=vplanet.wrapper:_entry_point"]},
)
