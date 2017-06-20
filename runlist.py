import os
import numpy as np
import scipy.misc as m

flist = open("l.txt","r").readlines()
for i,f in enumerate(flist):
    path = f.strip()
    print(path)
    img = m.imread(path)
    h = int(np.rand()*img.shape[0])
    w = int(np.rand()*img.shape[1])
    cmd = "build/src/SeamCarver %d %d %d new_"
    os.system("")