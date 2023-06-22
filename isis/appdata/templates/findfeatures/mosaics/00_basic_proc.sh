#!/bin/bash
#set -o pipefail
###############################################################################
#
# Applies basic ISIS processing for control preparation
#
# Prerequisites:
#   ISIS3        - must have an appropriate version of system initilized
#
# Defaults;
#   Current directory set to processing root dir
#
# @author  2021-03-02 Kris Becker Original Version
###############################################################################
usage() { echo "Usage: $0 [-0 {invokes dryrun}] [-h] [-v]  \
[-S {do not run spiceinit}] [-l] [-p] [-e {error list}] \
 *.IMG | *.cub  \
" 1>&2; exit 1; }

dryrun=0
verbose=0

run_spice=1   # run spiceinit by default, disable with -S
pwddir='$PWD'
lev0dir='$PWD/Lev0'
lev1dir='$PWD/Lev1'
lev2dir='$PWD/Lev2'
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
while getopts ":0hvSlpe:" o; do
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
        S)
            run_spice=0
            ;;
        l)
            maklog=1
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

declare -a fromfiles=( "$@" )

# Check to see if the input file list exists
# I. Variable definition
if [ ${#fromfiles[@]} -le 0 ]; then
  echo ""
  echo "ERROR: No files provided!"
  usage
  exit 1
fi


###############################################################################
#  Main processing loop. Process all give files checking for errors along
#  the way.
###############################################################################

for from in "${fromfiles[@]}"; do

    ifile="${from##*/}"
    base="${ifile%%.*}"
    ext="${ifile#*.}"

    (

# If given a cube, it is assumed import and radiometric calibration has
# been applied
      edrfile="${from}"
      lev0file="${from}"
      lev1file="${from}"

# Level0 DATA IMPORT processing
# Run import and apply radiometric calibrated images
      if [ "${ext}" != "cub" ]; then
        execute "mkdir -p ${lev0dir}"
        lev0file="${lev0dir}/${base}.cub"
        cmd="mdis2isis from=${from} to=${lev0file}"
        execute "$(finesse_isis_command "$cmd")" ||  exit $?
      fi

# Apply a prior SPICE kernels
      if [[ $run_spice -eq 1 ]]; then
        cmd="spiceinit from=${lev0file}"
        execute "$(finesse_isis_command "$cmd")" || exit $?
      fi

# Level1 - radiometric calibration processing
      if [ "${ext}" != "cub" ]; then
        execute "mkdir -p ${lev1dir}"
        lev1file="${lev1dir}/${base}.cal.cub"
        cmd="mdiscal from=${lev0file} to=${lev1file}"
        execute "$(finesse_isis_command "$cmd")" || exit $?
      fi

# Know Your Data!
      cmd="camstats from=${lev1file} attach=true linc=10 sinc=10"
      execute "$(finesse_isis_command "$cmd")" || exit $?

      cmd="footprintinit from=${lev1file} linc=10 sinc=10 maxemission=89 maxincidence=89 increaseprecision=true"
      execute "$(finesse_isis_command "$cmd")" || exit $?

    )

    # Subshell exist status and testing
    err=$?
    if [ $err != 0 ]; then
    #  If errlist file given, append this file to the list
      if [ -n "$errlist" ]; then
        [ -f "$errlist" ] || execute "touch $errlist"
        execute "echo ${from} >> $errlist"
      fi
      echo "Error encountered with file ${from}"
    fi
done

exit 0

