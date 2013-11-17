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
#texFile.write('\setlength\\hoffset{-87pt}\n')
#texFile.write('\setlength\\voffset{-72.5pt}\n')
#texFile.write('\setlength\\oddsidemargin{0pt}\n')
#texFile.write('\setlength\\topmargin{0pt}\n')
#texFile.write('\setlength\\headheight{0pt}\n')
#texFile.write('\setlength\\headsep{0pt}\n')
#texFile.write('\setlength\\paperwidth{' + str(width+5) + 'pt}\n')
#texFile.write('\setlength\\paperheight{' + str(height+5) + 'pt}\n')
#texFile.write('\setlength\\textwidth{' + str(width+20) + 'pt}\n')
#texFile.write('\setlength\\textheight{' + str(height) + 'pt}\n')
#texFile.write('\setlength\\marginparsep{0pt}\n')
#texFile.write('\setlength\\marginparwidth{0pt}\n')
#texFile.write('\setlength\\footskip{0pt}\n')
texFile.write('\\usepackage{graphicx}\n')
texFile.write('\DeclareGraphicsRule{*}{mps}{*}{}\n')
#texFile.write('\\pagestyle{empty}\n')
texFile.write('\\begin{document}%\n')
texFile.write('\includegraphics[scale=' + sys.argv[3] + ']{' + epsFileName + '}\n')
texFile.write('\end{document}\n')

texFile.close()

os.system('pdflatex ' + texFileName)
