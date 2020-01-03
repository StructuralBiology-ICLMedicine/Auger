#!/bin/env python

import sys
import numpy as np

def main():
    
    try:
        f = open(sys.argv[1], 'r')
        obj = f.readlines()
    except:
        print "\n Usage "+sys.argv[0]+" surface.obj \n"
        exit(1)

    headr = np.zeros(80, dtype = np.uint8)
    blank = np.zeros(1,  dtype = np.int16)
    count = np.zeros(1,  dtype = np.int32)

    vtx = []
    fct = []

    obj.reverse()

    while len(obj):

        try:

            line = obj.pop()
            
            if len(line) > 2:

                if line[:2] == 'v ':
                    tmp = np.zeros([3], dtype = np.float32)
                    line = line.split()
                    tmp[0] = float(line[1])
                    tmp[1] = float(line[2])
                    tmp[2] = float(line[3])
                    vtx.append(tmp)

                if line[:2] == 'f ':
                    line = line.split()
                    v1 = int(line[1].split('/')[0]) - 1
                    v2 = int(line[2].split('/')[0]) - 1
                    v3 = int(line[3].split('/')[0]) - 1
                    cr = np.cross((vtx[v2] - vtx[v1]), (vtx[v3] - vtx[v2]))
                    n  = cr / np.linalg.norm(cr)
                    fct.append([n, vtx[v1], vtx[v2], vtx[v3]])
                    count[0] += 1

        except:
            continue

    print "\n    " + str(count[0]) + " facets    \n"

    f = open(sys.argv[1].replace('.obj', '.stl'), 'w')
    f.write(headr[:80])
    f.write(count[0].tostring())
    for face in fct:
        for vector in face:
            f.write(vector.tostring())
        f.write(blank.tostring())
    f.close()

    print "\n    ++++ ++++ That's All Folks! ++++ ++++ \n"

    return 0

if __name__ == "__main__":
    sys.exit(main())
