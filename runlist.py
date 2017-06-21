import os
import numpy as np
import scipy.misc as m

def uni(a,b):
    x = np.rand()
    return x * (b-a) + a

flist = open("l.txt","r").readlines()
log = open("log.txt","w")
knum = 5
for i,f in enumerate(flist):
    path = f.strip()
    print(path)
    img = m.imread(path)
    h = int(uni(0.4,0.6)*img.shape[0])
    w = int(uni(0.4,0.6)*img.shape[1])

    for j in range(knum):
        cmd = "build/src/SeamCarver %s %d %d new_%d_%d_%d" % (path, j, h, w, j,h,w)
        log.write(cmd+"\n")
        os.system(cmd)
