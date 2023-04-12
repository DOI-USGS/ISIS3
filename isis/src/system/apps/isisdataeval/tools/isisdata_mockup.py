#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import time
import calendar
import datetime

import math
import os
import io
import pathlib
import hashlib

import re
import glob
import fnmatch
import shutil

import json
from collections import OrderedDict

import argparse


# In[ ]:


def parse_arguments():
    """
    Parse command line arguments for the application

    Parameters
    ----------
    none

    Returns
    -------
    Namespace object
        This object contains parameters that were gathered from the users
        command line. It can be converted to dict using vars(args).
    """
    full_doc = '''\
%(prog)s will create a copy of all the files found in the directory specified by
the --isisdata parameter. All files and directories contained in --isisdata will
be copied to the --outpath destination, which specifies a new directory. This
directory will be created if it does not exist.

All *.db and *.conf files are copied as is, fully intact. Also needed to use this
mock up of ISISDATA is the NAIF LSK. This LSK kernel is always loaded by the iTime
class to concert UTC and Et times. Otherwise, all other files encountered will
be created but the contents are replaced by information regarding the --isisdata
source file. This information includes the original file opath, creation and
modification dates in UTC format, the size of the file and its hash values.

This app sets up a full data directory structure that mimics the contents of
ISISDATA (or any directory for that matter). The size of the file, its creation
and last modification dates and the md5, sha1 and sha256 hash values are created
in a dict which is then written to the new output file if its not a special ISIS
kernel db/conf file or LSK.

Finally, by setting --ghostdir '$ISISDATA', this now provides a connection to
the source file in the ISISDATA directory. This can be used to compare hash values
compute by other sources.

Example:

To provide a full mock up of ISISDATA directory:

%(prog)s --saveconfig --isisdata /opt/isis4/data --outpath $PWD/mockisisdata --ghostdir '$ISISDATA'


Author: Kris J. Becker, University of Arizona
        kbecker@orex.lpl.arizona.edu

History 2023-03-15 Kris Becker - Original verison
    '''
    # Set to None for notebook mode or True for applicaton
    is_app = True

    if is_app is True:
        parser = argparse.ArgumentParser(description="Convert ISISDATA directory to test format",
                                         add_help=True,
                                         formatter_class=argparse.RawDescriptionHelpFormatter,
                                         epilog=full_doc)

        parser.add_argument('--isisdata', help="ISISDATA directory root",
                            required=True, action='store', default=None)
        parser.add_argument('--outpath', help="NEW ISISDATA directory to create",
                            required=True, action='store', default=None)
        parser.add_argument('--ghostdir',
                            help="Replaces the --isisdata path with this string to ghost the actual input directory path ($ISISDATA is highly recommended)",
                            required=False, action='store', default=None)
        parser.add_argument('--saveconfig','-s',
                            help='Retain *.db, *.conf files rather than replace with processs inf (for isisdataeval testing)',
                            action='store_true')

        parser.add_argument('--dryrun','-n',help='Only print actions but do not execute', action='store_true')
        parser.add_argument('--verbose','-v',help='Verbose output', action='store_true')
        args = parser.parse_args()

    else:
        # This is ran when ( is_app is None )
        isisdata   = '/opt/isis/data/base/dems'
        outpath    = '/tmp/MOCKISISDATA/base/dems'
        ghostdir   = '$ISISDATA/base/dems'
        saveconfig = True
        dryrun     = True
        verbose    = True

        args = argparse.Namespace(isisdata=isisdata, outpath=outpath, ghostdir=ghostdir,
                                  saveconfig=saveconfig, dryrun=dryrun, verbose=verbose)

    return args



# In[ ]:


def compute_hash( filepath, method='md5', dryrun=False, verbose=False, **kwargs):
    """
    Compute md5, sha1 or sha256 hashes from a file

    Parameters
    ----------
    filepath : string
        Source file to conpute hash values for

    method : string
        Specifies the hash algorithm to apply:
        md5, sha1 or sha256


    Returns
    -------
    string
        Returns the hex value of the compute hash for the file
    """
    if 'sha256' in method:
        hasher = hashlib.sha256()
    elif 'sha1' in method:
        hasher = hashlib.sha1()
    else:
        hasher = hashlib.md5()

    with open( filepath, "rb" ) as fb:
        for data in iter( lambda: fb.read(4096), b""):
            hasher.update( data )

    return hasher.hexdigest()



# In[ ]:


def preserve_file_contents( filepath, **kwargs ):
    """
    Determine if the file content should be preserved. This particular
    implemenatation satisfies requirements for mocking the ISISDATA
    directly to be used for testing of isisdataeval.

    This function preserves all files that end with a .db suffix. These
    are ISIS kernel databases.

    Files that have a .conf suffix are also ISIS kernel config file.
    However, MRO/HiRISE calibration application uses files that end with
    the .conf suffix.

    And the $base/kernels/lsk LSK kernels are preserved. This is because
    isisdataeval uses the ISIS iTime class to convert UTC times to ephemeris
    times and vice versa.

    All other file types are not preserved.

    Parameters
    ----------
    filepath : Path object
        Source file to check for perservation

    kwargs : args
        Additional parameters


    Returns
    -------
    bool
        Returns True if the conditions to preserve the contents is met.
        Otherwise, returns False.
    """

    if filepath.suffix == '.db': return True
    if filepath.suffix == '.conf':
        if 'kernels' in filepath.as_posix(): return True

    if 'base/kernels/lsk/naif00' in filepath.as_posix(): return True

    # All other cases
    return False


# In[ ]:


def main():
    """
    Read ISISDATA contents and create a new directory structure
    that maps the files but replaces contents with file information
    for testing purposes. Files that end with ".db" or ".conf" file
    suffixes are copied as is. All other files are replaced with
    data file info.

    Parameters
    ----------
    See parse_arguments()

    Returns
    -------
    None
    """

    #  Get the application parameters as provided by user
    args = parse_arguments()
    kwargs = vars(args)

    # Consolidate some parameters
    verbose  = args.verbose
    dryrun   = args.dryrun
    report   = verbose or dryrun

    isisdatadir = args.isisdata
    ghostdir    = args.ghostdir
    outpath     = args.outpath
    saveconfig  = args.saveconfig

    allfiles = sorted( pathlib.Path( isisdatadir ).glob('**/*') )
    if report: print("\nTotalFilesDirsFound: ", len(allfiles) )

    missing = [ ]

    for fpath in allfiles:
        if report:  print("\n*** Processing: ", fpath.as_posix() )
        isisdatapos = fpath.as_posix().find( isisdatadir )

        # Ghost the input dir if requested
        if ghostdir is not None:
            isisfile = fpath.as_posix().replace( isisdatadir, ghostdir, 1 )
        else:
            isisfile = fpath.as_posix()

        if  isisdatapos != 0:
            if report: print("FileNotInIsisDataIgnored: ", fpath.as_posix() )
            missing.append( fpath.as_posix() )
        else:

            jsondata = OrderedDict()
            jsondata["source"] = isisfile

            newisisdata = pathlib.Path( fpath.as_posix().replace( isisdatadir, outpath, 1 ) )
            # jsondata['source'] = newisisdata.as_posix()

            if fpath.is_dir():
                if not dryrun: newisisdata.mkdir( parents=True )

            else:   # its a real file
                outdir = pathlib.Path( newisisdata.parent )
                finfo = fpath.stat()

                create_ts   = os.path.getctime( fpath.as_posix() )
                modified_ts = finfo.st_mtime

                # This is actually needed on some systems
                if modified_ts < create_ts:
                    create_ts, modified_ts = modified_ts, create_ts

                # This one does not work on Linux systems (gotta be root to get the correct datetime!)
                #createtime   = datetime.datetime.fromtimestamp( finfo.st_birthtime, tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')
                createtime   = datetime.datetime.fromtimestamp( create_ts,   tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')
                modifiedtime = datetime.datetime.fromtimestamp( modified_ts, tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')

                #  Compute the file hashes
                md5hash    = compute_hash( fpath.as_posix(), method='md5',    **kwargs)
                sha256hash = compute_hash( fpath.as_posix(), method='sha256', **kwargs)
                sha1hash   = compute_hash( fpath.as_posix(), method='sha1',   **kwargs)

                # The rest of the data for this file
                jsondata["filesize"]     = finfo.st_size
                jsondata["createtime"]   = createtime
                jsondata["modifiedtime"] = modifiedtime
                jsondata["md5hash"]      = md5hash
                jsondata["sha256hash"]   = sha256hash
                jsondata["sha1hash"]     = sha1hash

                if report:
                    print( json.dumps( jsondata, indent=4 ) )

                if not outdir.exists():
                    if report: print("CreatingDir:    ", outdir.as_posix() )
                    if not dryrun: outdir.mkdir(parents=True)

                # There are also *.conf in $ISISDATA/mro/calibration so must exclude copying of those files
                # isisdataeval requires that the LSK be valid (for time conversions) so preserve those kernels
                if saveconfig and preserve_file_contents( fpath, **kwargs):
                    if report: print("CopyTo:         ", newisisdata.as_posix() )
                    if not dryrun: shutil.copy( fpath.as_posix(), newisisdata.as_posix() )
                else:
                    if report: print("WriteDataTo:    ", newisisdata.as_posix() )
                    if not dryrun: newisisdata.write_text( json.dumps( jsondata, indent=4 ) )


# In[ ]:


# Do it!
if __name__ == '__main__':
    main()
