#!/bin/bash
#set -o pipefail
###############################################################################
#
# Project MESSENGER MDIS images
#
#
# Prerequisites:
#   ISIS3        - must have an appropriate version of system initilized
#   spiceinit must be run on the input images
#   parallel is used to run multiple files simultaneously
#
# Defaults;
#   Current directory set to processing root dir
#
# @author  2021-03-02 Kris Becker Original Version
###############################################################################
usage() { echo "Usage: $0 [-0 {invokes dryrun}] [-h]  \
[-o <output image_dir>] -f fromlist.lis \
" 1>&2; exit 1; }

dryrun=0
verbose=0

fromlist="lev1.lis"
odir="$PWD/Lev2"
map="equi_clon180_PosEast_200m.map"
prtfile=""
errlist=""

##################################################################
#  finesse_isis_command - Adds additional components to ISIS commmand
#
#  This function will add logging and preference files to an ISIS
#  command that will be executed within this script.
#
# Should be called as:
#    newcmd=$(finesse_isis_command "cmd")
#
# @author Kris Becker 2018-05-23
##################################################################
function finesse_isis_command() {
command="$1"

# Check for logging
logger=""
if [ -n "${prtfile}" ]; then
  logger="-log=${prtfile}"
else
  logger="-log=/dev/null"
fi

echo "${command} ${logger}"
}

##################################################################
#  execute - Executes a command as a subshell
#
#  This function will execute a command to the users shell and
#  return status. If the variable $dryurn is set, it will only
#  print out the command and not execute
#
# @author Kris Becker 2018-05-23
##################################################################
function execute() {  # (command)
  command="$1"
  echo "$command"
  if [ $dryrun -eq 1 ]; then
    status=0
  else
    eval $command
    status=$?
  fi
  return $status
}

###################################################################
# MAIN script section
# Accumulate the parameters
###################################################################
while getopts ":0hvf:o:pe:" o; do
    case "${o}" in
        0)
            dryrun=1
            verbose=1
            ;;
        h)
            usage
            ;;
        v)
            verbose=1
            ;;
        f)
            fromlist="${OPTARG}"
            ;;
        o)
            odir="${OPTARG}"
            ;;
        p)
            prtfile="print.prt"
            ;;
        e)
            errlist="${OPTARG}"
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))


#dryrun=1

###############################################################################
#  Main processing loop. Process all give files checking for errors along
#  the way.
###############################################################################

    (
      execute "mkdir -p ${odir}"
      cmd="cat ${fromlist} | parallel --no-notice cam2map from={} to=${odir}/{/.}.proj.cub map=${map} pixres=map"
      execute "$cmd" || exit $?

    )

# Subshell exist status and testing
err=$?
if [ $err != 0 ]; then
  echo "Error encountered with file ${fromlist}"
fi

exit 0

