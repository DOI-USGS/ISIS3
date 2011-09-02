#!/usr/bin/perl -w

################################################################################
#
# spiceinit.cgi
#
# This script services POST requests containing hex-encoded cube 
# labels (stored in XML), runs spiceserver on those labels, and returns 
# relevant spice data (also encoded and stored in an XML document).
#
# Author: Jai Rideout
#
################################################################################
use strict;
use File::Temp ();

################################################################################
#
# CONFIGURATION PARAMETERS
#
################################################################################

# specify the directory for temp files (this directory must have already been
# created)
use constant TMPDIR => '/tmp/spice_web_service/';

# how many bytes were read from the file
my $bytes_read;

# holds post data
my $buffer;

my $output_data;

# name and value in query string
my ($name, $value);

# temp file to hold post data
my $input_file;

# temp file to hold results of output to be sent back to client
my $output_file;

# temp file to hold detailed results of spiceserver
my $log_file;

# store status of I/O ops
my $status;

# print required header
print "Content-type:text/html\r\n\r\n";

# check the request method (ie. GET or POST)
$ENV{'REQUEST_METHOD'} =~ tr/a-z/A-Z/;
if ($ENV{'REQUEST_METHOD'} eq 'POST') {
  # read in post data
  $bytes_read = read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

  if ($bytes_read != $ENV{'CONTENT_LENGTH'}) {
    print "Error reading POST data\n";
    exit;
  }

  # grab name/value pair
  ($name, $value) = split(/=/, $buffer);
}
else {
  print "Invalid request method! The request method must be POST\n";
  exit;
}

if (! -d TMPDIR ) {
  if (! mkdir TMPDIR) {
    print "Could not create temporary folder " . TMPDIR . "\n";
    exit;
  }
}

# create a new temp files in TMPDIR to hold the post data that was received, as
#  well as the results returned from spiceserver
$input_file = File::Temp->new( DIR => TMPDIR );
$output_file = File::Temp->new( DIR => TMPDIR );
$log_file = File::Temp->new( DIR => TMPDIR );

$status = open(INPUT, ">$input_file");
unless ($status != 0) {
  print "Couldn't create file for writing: $!\n";
  exit;
}

# write to file
print INPUT $value;

$status = close(INPUT); 
unless ($status) { 
  print "Couldn't close file: $!\n";
  exit;
}

# run spiceinit web service on input file. The results will be stored in an
# output file.
$status = system("./isis_commands.sh $input_file $output_file $log_file");
if ($status == -1) {
  # something went horribly wrong
  print "Couldn't execute system(): $!\n";
}
elsif ($status == 0) {
  # spiceserver executed just fine, so read contents of output file and 
  # pass them back to the client
  $status = open(OUTPUT, "<$output_file"); 
  unless ($status != 0) {
    print "Couldn't open file: $!\n";
    exit;
  }
  binmode(OUTPUT);

  while (($bytes_read = read(OUTPUT, $buffer, 1024)) != 0) {
    $output_data .= $buffer; 
  }

  $status = close(OUTPUT); 
  unless ($status) {
    print "Couldn't close file: $!\n";
    exit;
  }

  print $output_data;
}
else {
  # something went wrong with spiceserver, so return the contents of the error
  # log
  $status = open(LOG, "<$log_file"); 
  unless ($status != 0) {
    print "Couldn't open log file: $!\n";
    exit;
  }
  binmode(LOG);

  while (($bytes_read = read(LOG, $buffer, 1024)) != 0) {
    $output_data .= $buffer; 
  }

  $status = close(LOG);
  unless ($status) {
    print "Couldn't close log file: $!\n";
    exit;
  }

  print $output_data;
}

exit;

