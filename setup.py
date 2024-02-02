from setuptools import Extension, setup 

setup(
    ext_modules=[
        Extension(
            name="bulgogi",
            sources=["bulmodule.c", "bulgogi/src/dot_bul.c"],
            include_dirs=["bulgogi/inc"],
        ),
    ]
)
