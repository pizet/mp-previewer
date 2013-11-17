#!/usr/bin/env python3

import sys
import os

i = len(sys.argv[1]) - 1
while sys.argv[1][i] != '/':
    i = i - 1

os.chdir(sys.argv[1][:i+1])

if os.system('mpost --halt-on-error ' + sys.argv[1]) > 0:
    sys.exit(1)
