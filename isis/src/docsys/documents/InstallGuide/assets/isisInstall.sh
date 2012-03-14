#!/bin/bash
# For debugging add -x to line above
################################################################################
# @brief isisInstall
#
# This bash script (optionally) fetches the latest installer from the Isis
# distribution server, then launches it.  The user can also specify to run the
# installer from the command line, or a directory other than the current to
# download and run the installer.
#
# @author 2012-03-14 Travis Addair
#
################################################################################

# Constants
E_RSYNCERROR=1
E_OPTERROR=85
SCRIPT=`basename $0`

# Default parameters
CONSOLE=false
UPDATE=true
PRINT_ERRORS=false
DIR="."
USER_SERVER=""
USER_PORT=""
TIMEOUT="5"

# Option collecting process
while getopts ":hcned:s:p:t:" Option
do
  case $Option in
    h)
      echo "Usage: $SCRIPT options (-hcnedspt)"
      echo ""
      echo "Options:"
      echo "  -c"
      echo "    Run the installer from the command line, without a GUI"
      echo "  -n"
      echo "    Do not check online for the latest version of the installer"
      echo "    Runs whatever version of the installer is currently available"
      echo "  -e"
      echo "    Print rsync errors from failed connection attempts"
      echo "  -d"
      echo "    Specify a directory to download and execute the installer"
      echo "    Defaults to the current directory is unspecified"
      echo "  -s"
      echo "    Specify a specific distribution server to update from"
      echo "    Defaults to the first server for which a connection can be made"
      echo "  -p"
      echo "    Specify a specific port number to update from"
      echo "    Defaults to the default port; falls back to 80 if unsuccessful"
      echo "  -t"
      echo "    Specify a timeout (in seconds) for connection tests"
      echo "    Defaults to 5 seconds; a value of 0 means no timeout"
      echo ""
      echo "Examples:"
      echo "  $SCRIPT"
      echo "  $SCRIPT -n -d /work1/isis3"
      echo "  $SCRIPT -e -s isisdist.wr.usgs.gov -p 80 -t 0"
      exit $E_OPTERROR
      ;;
    c)
      CONSOLE=true
      ;;
    n)
      UPDATE=false
      ;;
    e)
      PRINT_ERRORS=true
      ;;
    d)
      DIR=$OPTARG
      ;;
    s)
      USER_SERVER=$OPTARG
      ;;
    p)
      USER_PORT=$OPTARG
      ;;
    t)
      TIMEOUT=$OPTARG
      ;;
    *)
      echo "Invalid option or missing argument"
      exit $E_OPTERROR
      ;;
  esac
done

# Redirect connection errors to stderr only of explicitly desired
REDIRECT="/dev/null"
if $PRINT_ERRORS
then
  REDIRECT="2"
fi

# If the user wants to update the installer, grab the latest version from the
# Isis distribution server
if $UPDATE
then
  RSYNC="rsync -azq"
  FILENAME="installer/install.jar"

  # Setup our list of valid ports, or take the user-provided one
  PORTS=( "" "--port=80" )
  if [ -n "$USER_PORT" ]
  then
    PORTS=( "--port=$USER_PORT" )
  fi

  # Setup our list of valid servers, or take the user-provided one
  SERVERS=( "isisdist.astrogeology.usgs.gov" "isisdist.wr.usgs.gov" )
  if [ -n "$USER_SERVER" ]
  then
    SERVERS=( $USER_SERVER )
  fi

  # Test connections until we get a good result
  CONNECTION=""
  for (( I=0 ; I < ${#PORTS[@]} ; I++ ))
  do
    # Standard for loop skips the empty string for some reason, so we're using
    # indexes instead
    PORT=${PORTS[$I]}
    for SERVER in ${SERVERS[@]}
    do
      # Try to establish a connection to the current port-server combination
      CONNECTION="$PORT $SERVER"
      $RSYNC --timeout=$TIMEOUT $CONNECTION:: >& $REDIRECT

      # No errors, so break out of both loops
      if [ $? -eq 0 ]
      then
        break 2
      fi

      # Errors, so reset the connection
      CONNECTION=""
    done
  done

  # None of our connection attempts succeeded, so we cannot grab the installer
  if [ -z "$CONNECTION" ]
  then
    echo "Cannot connect to a distribution server" >&2
    exit $E_RSYNCERROR
  fi

  # Download the latest installer from our first valid connection
  rsync -azq $CONNECTION::$FILENAME $DIR

  # Got a connection, but the download failed
  if [ $? != 0 ]
  then
    echo "Cannot retrieve the latest Isis installer JAR" >&2
    exit $E_RSYNCERROR
  fi
fi

# Pass the installer the console option if desired
PARAMS=""
if $CONSOLE
then
  PARAMS="-console"
fi

# Launch the installer
java -jar $DIR/install.jar $PARAMS
