from setuptools import Extension, setup 
from setuptools.command.build_ext import build_ext
from os import system

def Make():
    system('make -C bulgogi lib/libyaml.so')

class MakeBuildExt(build_ext):
    def run(self) -> None:
        Make()
        super().run()

setup(
    cmdclass={
        'build_ext': MakeBuildExt,
    },
    ext_modules=[
        Extension(
            name="bulgogi",
            sources=[
                "bulmodule.c", 
                "bulgogi/src/core.c"
            ],
            include_dirs=["bulgogi/inc"],
            library_dirs=["bulgogi/lib"],
            libraries=["yaml"],
        ),
    ]
)
