from setuptools import Extension, find_packages, setup 
from setuptools.command.build_ext import build_ext
from os import system
import sys

def is_macos():
    if sys.platform == 'darwin':
        return True
    else:
        return False

def Make():
    system('make -C bulgogi cibuildwheel')

def MakeMacOS():
    system('make -C bulgogi cibuildwheel-macos')

class MakeBuildExt(build_ext):
    def run(self) -> None:
        if is_macos():
            MakeMacOS()
        else:
            Make()
        super().run()

LIBBUL = 'bul'
if is_macos():
    LIBBUL = 'bul_universal'

setup(
    cmdclass={
        'build_ext': MakeBuildExt,
    },
    ext_modules=[
        Extension(
            name="bulgogi",
            sources=[
                "bulmodule.c", 
            ],
            include_dirs=["bulgogi/inc"],
            library_dirs=["bulgogi/lib"],
            libraries=[LIBBUL, "yaml"],
        ),
    ],
)
