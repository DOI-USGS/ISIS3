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

Users can select a specific set of hash algorithms using the --hasher parameter.
Note that more than one hash algorithm can be specified on the command line by
simply provide multiple --hasher values (e.g., --hasher=md5 --hasher=sha1)
or, to compute all available, use --hasher=all.

This app sets up a full data directory structure that mimics the contents of
ISISDATA (or any directory for that matter). The size of the file, its creation
and last modification dates and the md5, sha1 and sha256 hash values are created
in a dict which is then written to the new output file if its not a special ISIS
kernel db/conf file or LSK.

Finally, by setting --ghostdir '$ISISDATA', this now provides a connection to
the source file in the ISISDATA directory. This can be used to compare hash values
compute by other sources.

If a complete summary of processing log is desired, provide a file name in
--tojson. This file will contain a JSON structure with the contents of program
details and file analysis. Note that is particularly useful if you just want the
data and not the mockup replication. Just add --dryrun to produce the log file
only.

Example:

To provide a full mock up of ISISDATA directory:

%(prog)s --saveconfig --hasher all --isisdata /opt/isis4/data --outpath $PWD/isisdatamock --ghostdir '$ISISDATA' --tojson isisdata_mockup_full.json


Author: Kris J. Becker, University of Arizona
        kbecker@orex.lpl.arizona.edu

History 2023-03-15 Kris Becker Original verison
        2023-03-29 Kris Becker Added --tojson and --hasher parameters to improve utility
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
                            required=False, action='store', default=None)
        parser.add_argument('--ghostdir', help="Replaces the --isisdata path with this string to ghost the actual input directory path",
                            required=False, action='store', default=None)
        parser.add_argument('--hasher', help="Add desired/supported hash algorithms to compute",
                    action='append', choices=['md5', 'sha1', 'sha256', 'all'],
                    default=None)
        parser.add_argument('--saveconfig','-s',help='Retain *.db, *.conf files rather than replace with processs info',
                            action='store_true')
        parser.add_argument('--tojson',
                            help="Write all ISISDATA file information to output file in JSON format",
                            required=False, action='store', default=None)

        parser.add_argument('--dryrun','-n',help='Only print --outpath actions but do not execute', action='store_true')
        parser.add_argument('--verbose','-v',help='Verbose output', action='store_true')
        args = parser.parse_args()

    else:

        isisdata   = '/opt/isis3/data/base/dems'
        hasher     = None  # or ['all'], ['md5'], ['md5', 'sha1', 'md5', 'sha1', 'sha1']
        outpath    = None
        ghostdir   =  '$ISISDATA/base/dems'
        tojson     = '/Users/kbecker/ISIS/Isis4Dev/IsisData/IsisDataEval/isisdata_dem_data_none.json'
        saveconfig = True
        dryrun     = True
        verbose    = True

        args = argparse.Namespace(isisdata=isisdata, outpath=outpath, ghostdir=ghostdir,
                                  hasher=hasher, saveconfig=saveconfig, tojson=tojson,
                                  dryrun=dryrun, verbose=verbose)


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
        myhasher = hashlib.sha256()
    elif 'sha1' in method:
        myhasher = hashlib.sha1()
    elif 'md5' in method:
        myhasher = hashlib.md5()
    else:
        raise RuntimeError("Unsupported/invalid hash algorithm: " + method)

    with open( filepath, "rb" ) as fb:
        for data in iter( lambda: fb.read(4096), b""):
            myhasher.update( data )

    return myhasher.hexdigest()



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

    # Program details
    program_name     = 'isisdata_mockup.py'
    program_version  = "0.2"
    program_date     = "2023-03-29"
    program_runtime  = datetime.datetime.now(datetime.timezone.utc)
    program_runtime_str = program_runtime.strftime("%Y-%m-%dT%H:%M:%S.%f %Z")

    j_progdata = OrderedDict()
    j_progdata['name']    = program_name
    j_progdata['version'] = program_version
    j_progdata['date']    = program_date
    j_progdata['runtime'] = program_runtime_str

    #  Get the application parameters as provided by user
    args = parse_arguments()
    kwargs = vars(args)

    j_parameters = OrderedDict( kwargs )

    # Consolidate some parameters
    verbose  = args.verbose
    dryrun   = args.dryrun
    report   = verbose or dryrun

    isisdatadir = args.isisdata
    hashers     = args.hasher
    ghostdir    = args.ghostdir
    outpath     = args.outpath
    saveconfig  = args.saveconfig

    # Single test for generating output mockup
    do_mockup = (not dryrun) and ( outpath is not None )
    if outpath is None:
        outpath = '/tmp/isisdatamockingjunk'

    # Set up hashing conditions
    # Selected hashers are log to --tojson
    if hashers is None:
        hashlist = [ ]
    elif 'all' in hashers:
        hashlist = [ 'md5', 'sha1', 'sha256' ]
    else:
        # make sure the list is unique
        hashlist = list(set(hashers))


    allfiles = sorted( pathlib.Path( isisdatadir ).glob('**/*') )
    if report: print("\nTotalFilesDirsFound: ", len(allfiles) )

    missinglist = [ ]
    directories = [ ]
    filelist_j  = [ ]

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
            missinglist.append( fpath.as_posix() )
        else:

            jsondata = OrderedDict()
            jsondata["source"] = isisfile

            newisisdata = pathlib.Path( fpath.as_posix().replace( isisdatadir, outpath, 1 ) )
            # jsondata['source'] = newisisdata.as_posix()

            if fpath.is_dir():
                if do_mockup: newisisdata.mkdir( parents=True )
                directories.append( isisfile )

            else:   # its a real file
                outdir = pathlib.Path( newisisdata.parent )
                finfo = fpath.stat()

                create_ts   = os.path.getctime( fpath.as_posix() )
                modified_ts = finfo.st_mtime

                if modified_ts < create_ts:
                    create_ts, modified_ts = modified_ts, create_ts

                # This one does not work on Linux systems (gotta be root to get the correct value!)
                #createtime   = datetime.datetime.fromtimestamp( finfo.st_birthtime, tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')
                createtime   = datetime.datetime.fromtimestamp( create_ts,   tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')
                modifiedtime = datetime.datetime.fromtimestamp( modified_ts, tz=datetime.timezone.utc ).strftime('%Y-%m-%dT%H:%M:%S.%f')

                jsondata["filesize"]     = finfo.st_size
                jsondata["createtime"]   = createtime
                jsondata["modifiedtime"] = modifiedtime

                # Hashing...
                for hashmethod in hashlist:
                    hash_name = hashmethod + 'hash'
                    jsondata[hash_name] = compute_hash( fpath.as_posix(), method=hashmethod, **kwargs)

                # Add to inventory
                filelist_j.append( jsondata )

                if report:
                    print( json.dumps( jsondata, indent=4 ) )

                if not outdir.exists():
                    if report: print("CreatingDir:    ", outdir.as_posix() )
                    if do_mockup: outdir.mkdir(parents=True)

                # There are also *.conf in $ISISDATA/mro/calibration so must exclude copying of those files
                # isisdataeval requires that the LSK be valid (for time conversions) so preserve these kernels
                if saveconfig and preserve_file_contents( fpath, **kwargs) :
                    if report: print("CopyTo:         ", newisisdata.as_posix() )
                    if do_mockup: shutil.copy( fpath.as_posix(), newisisdata.as_posix() )
                else:
                    if report: print("WriteDataTo:    ", newisisdata.as_posix() )
                    if do_mockup: newisisdata.write_text( json.dumps( jsondata, indent=4 ) )


        # Program timing
        program_endtime  = datetime.datetime.now(datetime.timezone.utc)
        program_elapsed_time = str(program_endtime - program_runtime)

        j_progdata['endtime'] = program_endtime.strftime("%Y-%m-%dT%H:%M:%S.%f %Z")
        j_progdata['elapsedtime'] = program_elapsed_time

    # Write data if requested
    if args.tojson is not None:
        # Create output JSON structure
        j_datalog = OrderedDict()

        j_datalog['program']                = j_progdata
        j_datalog['program']['parameters']  = j_parameters

        j_datalog['inventory'] = OrderedDict( { 'hashlist'     : hashlist,
                                                'missing_count': len(missinglist),
                                                'files_count'  : len(filelist_j),
                                                'missing'      : missinglist,
                                                'files'        : filelist_j } )

        # Write the config results
        with open (args.tojson, "w") as json_data_file:
                json.dump(j_datalog, json_data_file, indent=2)


# In[ ]:


# Do it!
if __name__ == '__main__':
    main()
