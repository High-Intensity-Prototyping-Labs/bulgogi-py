#!/usr/bin/env python3
import bulgogi as bul

c = bul.Core(from_file='project.yaml')

print('#-- c.raw_targets(): --#')
for t in c.raw_targets():
    print('Target: ' + t.name)
    for dep in t.deps:
        if dep.name == 'src':
            print('  sources:')
            for src in dep.deps:
                print('    ' + src.name)
        elif dep.name == 'inc':
            print('  include:')
            for inc in dep.deps:
                print('    ' + inc.name)
        elif dep.name == 'dep':
            print('  deps:')
            for d in dep.deps:
                print('    ' + d.name)

print('')
print('#-- c.targets() --#')
for t in c.targets():
    print('Target: ' + t.name)
    for dep in t.deps:
        if dep.name == 'src':
            print('  sources:')
            for src in dep.deps:
                print('    ' + src.name)
        elif dep.name == 'inc':
            print('  include:')
            for inc in dep.deps:
                print('    ' + inc.name)
        elif dep.name == 'dep':
            print('  deps:')
            for d in dep.deps:
                print('    ' + d.name)
