from setuptools import Extension, setup 

setup(
    ext_modules=[
        Extension(
            name="bulgogi",
            sources=["bulmodule.c"],
        ),
    ]
)
