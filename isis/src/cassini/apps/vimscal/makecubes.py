import os
import re
import subprocess
import sys
import numpy as np

"""
Author:  2016-09-09 Tyler Wilson
History:

Description:  This script creates calibration cubes for use
by the VIMS calibration program vimscal.

Input:  Calibration text files.
Output: Calibration cubes.

Usage:  python makecubes.py <path to calibration files> <prefix for calibration files> <version>

Example:  python makecubes.py /home/tjwilson/cal_files/RC19-mults RC19 0001

The calibration files in RC19-mults are named RC19-2004.txt, RC19-2005.txt,....
so the file prefix is:  RC19

The version number must be a 4-digit number.

"""


stderrors = []
stdoutput = []

usage = "\nUsage:  python makecubes.py <path to calibration files> <prefix for calibration files>\n"
example_usage = "Example:  python makecubes.py /home/tjwilson/cal_files/RC19-mults RC19\n"


def cleanup(filepath):
    """Cleaner function which attempts to remove temporary cubes and cube lists.

        Args:
            filepath:  The path to the directory containing the filewe wish to remove.

        Raises: A general exception if anything goes wrong.  The script exits at that point.
    """

    try:
        os.system("rm "+filepath+"/temp_*.cub")
    except:
        sys.exit("Something went wrong when attempting to delete the temp cubes in:  "+ filepath)
    try:
        os.system("rm "+filepath+"/*.lis")
    except:
        sys.exit("Something went wrong when attempting to delete the cube lists in: "+filepath)


try:
    datapath = sys.argv[1]
except IndexError:
    print("Please enter a valid path to the calibration files.")
    print(usage)
    sys.exit(example_usage)

try:
    files = os.listdir(sys.argv[1])
except OSError:
    print("There is no directory named:  "+datapath)
    print(usage)
    sys.exit(example_usage)

if (len(files) == 0):
    sys.exit("There are no calibration files in directory:  "+datapath)

try:
    prefix = sys.argv[2]
except IndexError:
    print("Please enter a valid prefix for the calibration files.")
    print(usage)
    sys.exit(example_usage)

version = sys.argv[3]
match = re.search(r'[0-9]{4}',version)
if (match == None):
    sys.exit("Please enter a 4 digit version number such as: 0001")

'''
try:
    precision=sys.argv[4]
except IndexError:
    print("Please enter an integer value for the number of decimal places in the calibration files.")
    sys.exit(example_usage)
'''

precision=10

filtered = []
#This is to ensure that we are only grabbing the txt calibration files and not
#anything else which might be in the directory.
for f in files:
	if (f.startswith(prefix) and f.endswith('.txt') ):
		filtered.append(f)

if ( len(filtered) == 0):
    print("Please enter a valid prefix for the calibration files.\n")
    sys.exit("Example:  If the calibration file has the name solar.2006.txt, then a prefix of solar would work.\n")



for datafile in filtered:
    try:
        #datafile without it's extension
        datafilename=os.path.splitext(datafile)[0]
        if (datafilename.find("-") > 0 ):
            datafilename.replace("-",".")
        #import the datafile into a matrix
        data = np.genfromtxt(datapath+"/"+datafile)
        bandvals = data[:,1]
        args3="samples=64"
        args4 ="lines=64"
        args5="bands=1"
        i=1
        # This cubelist stores the list of temporary cubes which will be stacked together
        # to form the calibration cube.
        cubelistname = datafilename+".lis"
        cubelist=open(datapath+"/"+cubelistname,"w")
        #makecube creates a temporary cube of dimension (64,64,1) where each DN = bandval
        for band in bandvals:
            cmd="makecube"
            tempcubepath=datapath+"/temp_"+datafilename+"_"+str(i)+".cub"
            args1="to="+tempcubepath
            format_string = '{0:.'+str(precision)+'f}'
            fstring = format_string.format(float(band))
            args2="value="+fstring
            #print(fstring)
            try:
                p=subprocess.Popen([cmd,args1,args2,args3,args4,args5],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
            except OSError:
                sys.exit("Please set a valid ISIS version before executing this script.")
            except:
                sys.exit("Something went wrong.  Please set a valid ISIS version before executing this script.")            
            out,err = p.communicate()
            if (str (err) ):
                print(err)

            cubelist.write(tempcubepath+"\n")
            print(tempcubepath)
            i = i+1

        cubelist.close()
        #Stack the temp cubes (there are 352 of them) into the calibration cube using cubeit
        cmd = "cubeit"
        args1="fromlist="+datapath+"/"+cubelistname
        args2="to="+datapath+"/"+datafilename+"_v"+version+".cub"
        p=subprocess.Popen([cmd,args1,args2],stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        out,err=p.communicate()
        if (str (err)):
            print(err)
        print("Finished:  "+datafilename+"_v"+version+".cub")
    except KeyboardInterrupt:
        cleanup(datapath)
        sys.exit("Program exiting.")


    cleanup(datapath)
