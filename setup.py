from setuptools import Extension, setup 

from contextlib import suppress 
from pathlib import Path 
from setuptools import Command 
from setuptools.command.build import build 

class MakeCommand(Command):
    def initialize_options(self) -> None:
        self.bdist_dir = None 

    def finalize_options(self) -> None:
        with suppress(Exception):
            self.bdist_dir = Path(self.get_finalized_command("bdist_wheel").bdist_dir)

    def run(self) -> None:
        if self.bdist_dir:
            print("self.bdist_dir was successfully found!")

class MakeBuild(build):
    sub_commands = [('build_custom', None)] + build.sub_commands

setup(
    cmdclass={'build': MakeBuild, 'build_custom': MakeCommand},
    ext_modules=[
        Extension(
            name="bulgogi",
            sources=[
                "bulmodule.c", 
                "bulgogi/src/dot_bul.c", 
                "bulgogi/src/engine.c",
                "bulgogi/src/fs.c",
                "bulgogi/src/yaml_ext.c",
            ],
            include_dirs=["bulgogi/inc"],
        ),
    ]
)
