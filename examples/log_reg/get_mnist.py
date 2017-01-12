# coding : utf-8

import sys
import os
import glob
import random
import shutil

source     = u'https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/multiclass'
train_file = u'mnist.scale.bz2'
test_file  = u'mnist.scale.t.bz2'

if not os.path.exists( train_file ):
    if sys.version_info[0] == 2:
        from urllib import urlretrieve
    else:
        from urllib.request import urlretrieve
    urlretrieve( os.path.join(source,train_file), train_file )
    urlretrieve( os.path.join(source,test_file), test_file )    

if not os.path.exists( train_file.replace(u'.bz2',u'') ):
    def uncompress( filename ):
        import bz2
        fin = bz2.BZ2File( filename )
        lines = fin.readlines()
        fin.close()
        fout = open( filename.replace(u'.bz2',u''), u'w' )
        for l in lines:
            print >>fout,l,
        fout.close()

    uncompress( train_file )
    uncompress( test_file )    
