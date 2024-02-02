from setuptools import Extension, setup 

setup(
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
