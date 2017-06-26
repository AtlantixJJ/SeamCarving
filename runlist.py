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
    cet = path[7:]
    img = m.imread(path)
    h = int(uni(0.3,0.5)*img.shape[0])
    w = int(uni(0.3,0.5)*img.shape[1])

    for j in range(1,knum,1):
        fname = "res/%s_small_%d-%d_%d-%d_%d" % (cet, j, img.shape[0], img.shape[1], h,w)
        cmd = "build/src/SeamCarver %s %d %d %d %s.jpg %s_seam.jpg &" % (path, j, h, w, fname, fname)
        log.write(cmd+"\n")
        os.system(cmd)
    
    h = int(uni(1.1,1.7)*img.shape[0])
    w = int(uni(1.1,1.7)*img.shape[1])

    for j in range(knum):
        if j == knum -1:
            fl = " "
        else:
            fl = "&"
        fname = "res/%s_enlarge_%d-%d_%d-%d_%d" % (cet, j, img.shape[0], img.shape[1], h,w)
        cmd = "build/src/SeamCarver %s %d %d %d %s.jpg %s_seam.jpg &" % (path, j, h, w, fname, fname)
        log.write(cmd+"\n")
        os.system(cmd)
    
    h = int(uni(1.1,1.7)*img.shape[0])
    w = int(uni(0.3,0.5)*img.shape[1])

    for j in range(1,knum,1):
        fname = "res/%s_by_%d-%d_%d-%d_%d" % (cet, j, img.shape[0], img.shape[1], h,w)
        cmd = "build/src/SeamCarver %s %d %d %d %s.jpg %s_seam.jpg &" % (path, j, h, w, fname, fname)
        log.write(cmd+"\n")
        os.system(cmd)

log.close()