# bulgogi-py
The Python interface for [bulgogi](https://github.com/high-intensity-prototyping-labs/bulgogi).

## Motive
1. Rather than write a CLI from scratch, using a scripting language _as the interface_ seemed cool and appropriate for this project.
2. Build configurations are slow-changing and repititious, making scripts ideal for the job.

## Build dependencies 
The following system dependencies are required:
- git 
- gcc
- make
- libtool
- python3
  - pip
  - build 
- python3-devel

And any dependencies not listed here but found in: [yaml/libyaml](https://github.com/yaml/libyaml).

## Build instructions 
Use the `build.sh` script to make sure necessary submodules are cloned and setup correctly:

```

./build.sh 

```

## Usage
To get started, install bulgogi using pip:
```
pip install --user bulgogi 
```

Once installed at the user-level, use it in your latest build project.

### 1. Create `setup.py`
First create the `setup.py` file in the root of your build project and declare your project:

```py 
# setup.py 
import bulgogi as bul 

bul.new_project('My Project')
bul.set_version('v1.0.0')

...

```

### 2. Declare targets 
The next step is to declare targets based on your project layout:

```py

...

target1 = bul.add_target('target1', bul.LIB)
execute = bul.add_target('execute', bul.EXE)

...

```

### 3. Declare relations
Targets need to be linked in some kind of way. This is usually referred to as 'dependency linking'.

In the Python interface for bulgogi, dependencies are linked by target ID:

```py 

...

bul.add_target_dep(execute, target1)

...

```

### 4. Commit the setup 
Lastly, the build configuration must be commit to disk before it can be built:

```py 

...

bul.commit()

```

## Putting it all together 
```py 
# setup.py 
import bulgogi as bul 

bul.new_project('My Project')
bul.set_version('v1.0.0')

target1 = bul.add_target('target1', bul.LIB)
execute = bul.add_target('execute', bul.EXE)

bul.add_target_dep(execute, target1)

bul.commit()
```

This will generate a `project.yaml` file in the root of your directory and resemble something like:

```yaml 
execute:
  - target1
```

Usually configuration files are guarded from manual edits - but bulgogi in fact encourages the developer to modify the `project.yaml` file as they please.

## Where are all the sources?
Although targets have been declared and dependencies linked, the best bulgogi can do to find the source files to copmile is to guess.

As a sane default, it will guess that each target has a directory matching its name (`target1/` and `execute/` in this case) and look for `src` and `inc` directories within.

This means - yes - that in theory the `setup.py` script can be ommitted altogether. This is not defeatist - it's to highlight that the script is most useful to standardize project configuration for all bulgogi targets, even if the project isn't your own.

## Declaring sources 
In the `setup.py`, target sources can be declared:

```py 

...

bul.add_sources(target1, 'target1/src/*.c')
bul.add_headers(target1, 'target1/inc/*.h')

...

bul.commit()
```

This can be done anytime _after_ `target1` has been added but _before_ the configuration is committed to disk.

### Patterns
It is evident that a specific files were not specified for the target sources. Patterns such as `*.c` are used instead and are known as 'globbing'.

### Globbing
Globbing is a mixed subject among build system afficionados. Bulgogi offers it as a built-in feature and allows users to decide how they would like to approach.


```py 

...

# Adding individual files is allowed.
bul.add_sources(target1, 'target1/src/hello.c')
bul.add_sources(target1, 'target1/src/dog.c')
bul.add_sources(target1, 'target1/src/cat.c')

...

# Files can be 'globbed' by file-extension.
bul.add_sources(target1, 'target1/src/*.c')
bul.add_sources(target1, 'target1/src/*.cpp')

...

# Globbing can also be performed recursively.
bul.add_headers(target1, 'target1/inc/**.c')


bul.commit()
```

### Variables 
A benefit of using a feature-complete scripting language is the built-in availability of variables.


```py 

...

targets = ['target1', 'execute']
src_dir = 'src'
inc_dir = 'inc'

for target in targets:
    bul.add_sources(target, bul.names(target) + '/' + SRC_DIR + '/*.c')
    bul.add_headers(target, bul.names(target) + '/' + INC_DIR + '/*.h')

bul.commit()
```

TODO: Complete section and make it accurate / stop inventing stuff.
