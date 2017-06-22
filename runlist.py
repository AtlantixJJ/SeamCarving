import os
import numpy as np
import scipy.misc as m

def uni(a,b):
    x = np.random.rand()
    return x * (b-a) + a

flist = open("l.txt","r").readlines()
log = open("log.txt","w")
knum = 7
for i,f in enumerate(flist):
    path = f.strip()
    print(path)
    img = m.imread(path)
    h = int(uni(0.3,0.7)*img.shape[0])
    w = int(uni(0.3,0.7)*img.shape[1])

    for j in range(knum):
        cmd = "build/src/SeamCarver %s %d %d %d new_small_%d-%d_%d-%d_%d.jpg &" % (path, j, h, w, j, img.shape[0], img.shape[1], h,w)
        log.write(cmd+"\n")
        os.system(cmd)
    
    h = int(uni(0.9,1.5)*img.shape[0])
    w = int(uni(0.9,1.5)*img.shape[1])

    for j in range(knum):
        if j == knum -1:
            fl = " "
        else:
            fl = "&"
        cmd = "build/src/SeamCarver %s %d %d %d new_enlarge_%d-%d_%d-%d_%d.jpg %s" % (path, j, h, w, j, img.shape[0], img.shape[1],h,w,fl)
        log.write(cmd+"\n")
        os.system(cmd)

log.close()