#!/usr/bin/env python3
import bulgogi as bul

c = bul.Core(from_file='project.yaml')
print('c = ' + str(c))

for t in c.raw_targets():
    print('t = ' + str(t))
    print(t.name + ', ' + str(t.id))
