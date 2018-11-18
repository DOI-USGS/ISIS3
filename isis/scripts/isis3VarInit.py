#!/usr/bin/env python

import argparse
import os
import sys

##########################################################################################################
#
#  This work is free of known copyright restrictions.  USGS-authored or produced data, 
#  information, and software are in the public domain.
#
#
#   Description:  This program builds the shell scripts that define the 
#       ISISROOT/ISIS3DATA/ISIS3TESTDATA environment variables for the user 
#       when the ISIS3 conda environment is activated, and clean up when it is
#       deactivated.
#
#       The data directory and test directory are optional command line arguments.  
#       If the user chooses not to set them, they will both be created in the
#       $ISISROOT directory.
#
#   History:
#       Author:  Tyler Wilson, USGS
#       Date:    2018-10-05
#       Description: Initial commit.
#
#       Author:  Tyler Wilson, USGS
#       Date:    2018-11-01
#       Description:  Removed a pair of lines which were causing output errors on Mac OS X and were not
#                     required anyway.
#
#       Author:  Ross Beyer
#       Date:    2018-11-
#       Description: Streamlined the program, improved documentation, and made the directory and
#                    file creation more `pythonic' rather than using system calls.
#       To the extent possible under law, Ross Beyer has waived all copyright and related or 
#       neighboring rights to his contribution to this file.  This work is published from
#       the United States.
#   
#
##########################################################################################################

# There are still a lot of Python 2 installations out there, and if people don't have
# their conda environment set up properly, the error message they'll get will be hard
# to decipher.  This might help:
assert( sys.version_info >= (3,2) ) # Must be using Python 3.2 or later, is conda set up?


# This just wraps and reports on the directory creation:
def mkdir( p ):
    if os.path.exists( p ): print( 'Tried to create '+p+', but it already exists.' )
    else:
        os.makedirs( p )
        print( 'Created '+p )
    return


# Set up and then parse the command line:
parser = argparse.ArgumentParser( description='This program builds shell scripts that define ISIS3 environment variables during conda environment activation and deactivation, and creates some directories.' )

parser.add_argument('-d','--data-dir', 
                    default=os.environ['CONDA_PREFIX']+'/data',
                    help='ISIS3 Data Directory, default: %(default)s' )
parser.add_argument('-t','--test-dir',
                    default=os.environ['CONDA_PREFIX']+'/testData',
                    help='ISIS3 Test Data Directory, default: %(default)s')
args=parser.parse_args()
<<<<<<< HEAD
=======
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

os.popen("echo '#!/bin/sh' > "+isisroot+ "/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISISROOT="+isisroot+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISIS3DATA="+data_dir+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")
os.popen("echo 'export ISIS3TESTDATA="+testdata_dir+"' >>"+isisroot+"/etc/conda/activate.d/env_vars.sh")

os.popen("echo '#!/bin/sh' > "+isisroot+ "/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISISROOT' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISIS3DATA' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")
os.popen("echo 'unset ISIS3TESTDATA' >>"+isisroot+"/etc/conda/deactivate.d/env_vars.sh")
>>>>>>> upstream/dev

# Create the data directories:
mkdir( args.data_dir )
mkdir( args.test_dir )

# Create the conda activation and deactivation directories:
activate_dir   = os.environ['CONDA_PREFIX']+'/etc/conda/activate.d'
deactivate_dir = os.environ['CONDA_PREFIX']+'/etc/conda/deactivate.d'

mkdir( activate_dir )
mkdir( deactivate_dir )

# Write the files that manage the ISIS3 environments:
activate_vars   =   activate_dir+'/env_vars.sh'
deactivate_vars = deactivate_dir+'/env_vars.sh'

with open( activate_vars, mode='w' ) as a:
    a.write('#!/bin/sh\n')
    a.write('export ISISROOT='+      os.environ['CONDA_PREFIX']+'\n')
    a.write('export ISIS3DATA='+     args.data_dir +'\n')
    a.write('export ISIS3TESTDATA='+ args.test_dir +'\n')
print( 'Wrote '+activate_vars )

with open( deactivate_vars, mode='w' ) as d:
    d.write('#!/bin/sh\n')
    d.write('unset ISISROOT\n')
    d.write('unset ISIS3DATA\n')
    d.write('unset ISIS3TESTDATA\n')
print( 'Wrote '+deactivate_vars )
