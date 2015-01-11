#!/usr/bin/env python3

import sys
import os

os.chdir(sys.argv[2] + '/.metaview')

epsFileName = sys.argv[1]
epsFile = open(epsFileName, 'r')

lines = epsFile.readlines()
boundingBox = lines[2].split()
llx = boundingBox[1]
lly = boundingBox[2]
urx = boundingBox[3]
ury = boundingBox[4]
height = float(ury) - float(lly)
width = float(urx) - float(llx)

i = len(epsFileName) - 1
while epsFileName[i] != '/':
    i -= 1

texFileName = sys.argv[2] + '/.metaview/' + epsFileName[i+1:len(epsFileName)-4] + '.tex'
texFile = open(texFileName, 'w')
print(texFileName)

texFile.write('\documentclass{standalone}\n')
texFile.write('\\usepackage{graphicx}\n')
texFile.write('\DeclareGraphicsRule{*}{mps}{*}{}\n')
texFile.write('\\begin{document}%\n')
texFile.write('\includegraphics[scale=' + sys.argv[3] + ']{' + epsFileName + '}\n')
texFile.write('\end{document}\n')

texFile.close()

os.system('pdflatex ' + texFileName)
