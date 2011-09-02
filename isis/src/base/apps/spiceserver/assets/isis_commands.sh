#!/bin/bash

infile="$1"
outfile="$2"
logfile="$3"

HOME='/var/www'
export HOME

ISISROOT='/work1/isis3/isis'
export ISISROOT

. $ISISROOT/scripts/isis3Startup.sh
spiceserver from="$infile" to="$outfile" -log="$logfile"

