from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.ccompiler import new_compiler
from glob import glob


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""

    def build_extensions(self):
        ct = self.compiler.compiler_type  # msvc or unix
        for ext in self.extensions:
            if ct == "msvc":
                ext.extra_compile_args = [
                    "/EHsc",
                    "/Wno-unused-variable",
                    "/Wno-missing-braces",
                    "/Wno-strict-prototypes",
                    "/Wno-sign-compare",
                    "/Wno-comment",
                ]
            else:
                ext.extra_compile_args = [
                    "-Wno-unused-variable",
                    "-Wno-missing-braces",
                    "-Wno-strict-prototypes",
                    "-Wno-sign-compare",
                    "-Wno-comment",
                ]
        build_ext.build_extensions(self)


ext_modules = [
    Extension(
        "vplanet.vplanet_core",
        glob("src/*.c"),
        include_dirs=["src"],
        language="c",
        define_macros=[
            ("VPLANET_PYTHON_INTERFACE", 1),
            (
                "VPLANET_VERSION",
                '"{}"'.format(open("VERSION", "r").read().split("\n")[0].strip()),
            ),
        ],
    )
]
cmdclass = {"build_ext": BuildExt}


setup(
    name="vplanet",
    author="Rory Barnes",
    author_email="rkb9@uw.edu",
    version=open("VERSION", "r").read().split("\n")[0].strip(),
    url="https://github.com/VirtualPlanetaryLaboratory/vplanet",
    description="The virtual planet simulator",
    long_description=open("README.md", "r").read(),
    long_description_content_type="text/markdown",
    license="MIT",
    packages=find_packages(),
    install_requires=["vplot>=0.3.3"],
    ext_modules=ext_modules,
    cmdclass=cmdclass,
    include_package_data=True,
    zip_safe=False,
    entry_points={"console_scripts": ["vplanet=vplanet:_entry_point"]},
)
