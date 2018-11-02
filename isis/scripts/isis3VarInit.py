#!/usr/bin/env python
import argparse
import os
import sys
#Author:  Tyler Wilson
#Date  :  2018-10-05
#Description:  This script sets ISISROOT/ISIS3DATA/ISIS3TESTDATA for the user and is executed
#within the conda environment created for the ISIS3 installation.
#The data directory and test directory are optional command line arguments.  If the user chooses
#not to set them, they will both be placed created on the same level as the $ISISROOT directory
#within the conda environment.
#History:
#   Author:  Tyler Wilson
#   Date:    2018-11-01
#   Description:  Removed a pair of lines which were causing output errors on Mac OS X and were note
#                 needed anyway.
#   


parser = argparse.ArgumentParser(description='Usage:  ./isis3VarInit --data_dir <data dir path> --test_dir <test dir path')
   
isisroot = '$CONDA_PREFIX'
data_dir='$CONDA_PREFIX/data'
testdata_dir='$CONDA_PREFIX/testData'

parser.add_argument("--data-dir",default= data_dir,help="ISIS3 Mission Data Directory")
parser.add_argument("--test-dir",default=testdata_dir,help="ISIS3 Mission Test Data Directory")

args=parser.parse_args()
if (data_dir != args.data_dir):
    os.system("mkdir -p "+args.data_dir)
    data_dir = args.data_dir
else:    
    os.system("mkdir -p "+data_dir)

if (testdata_dir != args.test_dir):
    os.system("mkdir -p "+args.test_dir)
    testdata_dir=args.test_dir
else:
    os.system("mkdir -p "+testdata_dir)

os.popen('mkdir -p '+isisroot+'/etc/conda/activate.d')
os.popen('mkdir -p '+isisroot+'/etc/conda/deactivate.d')

os.popen('touch '+isisroot+'/etc/conda/activate.d/env_vars.sh')
os.popen('touch '+isisroot+'/etc/conda/activate.d/env_vars.sh')

os.popen("echo '#!/bin/sh' >> "+isisroot+ "/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISISROOT="+isisroot+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISIS3DATA="+data_dir+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISIS3TESTDATA="+testdata_dir+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")

os.popen("echo '#!/bin/sh' >> "+isisroot+ "/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISISROOT' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISIS3DATA' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISIS3TESTDATA' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")

