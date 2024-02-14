from setuptools import Extension, find_packages, setup 
from setuptools.command.build_ext import build_ext
from os import system

def Make():
    system('make -C bulgogi cibuildwheel')

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
            ],
            include_dirs=["bulgogi/inc"],
            library_dirs=["bulgogi/lib"],
            libraries=["bul", "yaml"],
        ),
    ],
    packages=find_packages(where="bulgogi"),
    package_dir={"": "bulgogi"},
    package_data={"lib": ["*.a"]},
    include_package_data=True,
)
