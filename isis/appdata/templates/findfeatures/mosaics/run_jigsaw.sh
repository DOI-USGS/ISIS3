#!/bin/bash
#set -o pipefail
###############################################################################
#
# Run jigsaw on MESSENGER MDIS images
#
# Prerequisites:
#   ISIS3        - must have an appropriate version of system initilized
#
# Defaults;
#   Current directory set to processing root dir
#
# @author  2021-03-02 Kris Becker Original Version
###############################################################################
usage() { echo "Usage: $0 [-0 {invokes dryrun}] [-h]  \
[-f <fromlist>] [-u {update image}] \
[-c <input network>] [-o <output network>] \
" 1>&2; exit 1; }

dryrun=0
verbose=0

cnetfile="EW0218118239G_clean.net"
fromlist="lev1.lis"
onetfile="EW0218118239G.jigsaw.net"
update="no"
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
while getopts ":0hvc:f:o:pe:u" o; do
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
        c)
            cnetfile="${OPTARG}"
            ;;
        f)
            fromlist="${OPTARG}"
            ;;
        o)
            onetfile="${OPTARG}"
            ;;
        u)
            update="yes"
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

      ofile="${onetfile##*/}"
      base="${ofile%%.*}"

# Run jigsaw with nominal parameters
      cmd="jigsaw fromlist=${fromlist} cnet=${cnetfile} onet=${onetfile} update=${update}"
          cmd+=" radius=no errorpropagation=yes outlier_rejection=no sigma0=1.0e-10 maxits=10"
          cmd+=" camsolve=angles twist=yes spsolve=no camera_angles_sigma=0.025"
          cmd+=" file_prefix=${base}"
      execute "$(finesse_isis_command "$cmd")" || exit $?

    )

# Subshell exist status and testing
err=$?
if [ $err != 0 ]; then
  echo "Error encountered with file ${fromlist}"
fi

exit 0

