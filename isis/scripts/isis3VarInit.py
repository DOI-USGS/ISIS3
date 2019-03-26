#!/usr/bin/env python
"""
This program builds shell scripts that define ISIS3 environment variables during
conda environment activation and deactivation, and creates some directories.
"""

import argparse
import os
import sys

##########################################################################################################
#
#  This work is free and unencumbered software released into the public domain.
#  In jurisdictions that recognize copyright laws, the author or authors
#  of this software dedicate any and all copyright interest in the
#  software to the public domain.
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
#       Date:    2018-11-19
#       Description: Streamlined the program, improved documentation, and made the directory and
#                    file creation more `pythonic' rather than using system calls.
#
#       Author:  Jesse Mapel
#       Date:    2019-03-25
#       Description: Added C-Shell support.
#
#
##########################################################################################################

# There are still a lot of Python 2 installations out there, and if people don't have
# their conda environment set up properly, the error message they'll get will be hard
# to decipher.  This might help:
assert( sys.version_info >= (3,2) ) # Must be using Python 3.2 or later

# This just wraps and reports on the directory creation:
def mkdir( p ):
    if os.path.exists( p ): print( 'Tried to create '+p+', but it already exists.' )
    else:
        os.makedirs( p )
        print( 'Created '+p )
    return


# Set up and then parse the command line:
parser = argparse.ArgumentParser( description=__doc__ )

parser.add_argument('-d','--data-dir',
                    default=os.environ['CONDA_PREFIX']+'/data',
                    help='ISIS3 Data Directory, default: %(default)s' )
parser.add_argument('-t','--test-dir',
                    default=os.environ['CONDA_PREFIX']+'/testData',
                    help='ISIS3 Test Data Directory, default: %(default)s')
args=parser.parse_args()


# Create the data directories:
mkdir( args.data_dir )
mkdir( args.test_dir )

# Create the conda activation and deactivation directories:
activate_dir   = os.environ['CONDA_PREFIX']+'/etc/conda/activate.d'
deactivate_dir = os.environ['CONDA_PREFIX']+'/etc/conda/deactivate.d'

mkdir( activate_dir )
mkdir( deactivate_dir )

# Write the files that manage the ISIS3 environments:
activate_vars_sh   =   activate_dir+'/env_vars.sh'
deactivate_vars_sh = deactivate_dir+'/env_vars.sh'
activate_vars_csh   =   activate_dir+'/env_vars.csh'
deactivate_vars_csh = deactivate_dir+'/env_vars.csh'

with open( activate_vars_sh, mode='w' ) as a:
    script = """#!/bin/sh
export ISISROOT={}
export ISIS3DATA={}
export ISIS3TESTDATA={}
""".format(os.environ['CONDA_PREFIX'], args.data_dir, args.test_dir)
    a.write(script)
print( 'Wrote '+activate_vars_sh )

with open( deactivate_vars_sh, mode='w' ) as d:
    script = """#!/bin/sh
unset ISISROOT
unset ISIS3DATA
unset ISIS3TESTDATA
"""
    d.write(script)
print( 'Wrote '+deactivate_vars_sh )

with open( activate_vars_csh, mode='w' ) as a:
    script = """#!/bin/csh
setenv ISISROOT {}
setenv ISIS3DATA {}
setenv ISIS3TESTDATA {}
""".format(os.environ['CONDA_PREFIX'], args.data_dir, args.test_dir)
    a.write(script)
print( 'Wrote '+activate_vars_csh )

with open( deactivate_vars_csh, mode='w' ) as d:
    script = """#!/bin/sh
unsetenv ISISROOT
unsetenv ISIS3DATA
unsetenv ISIS3TESTDATA
"""
    d.write(script)
print( 'Wrote '+deactivate_vars_csh )
