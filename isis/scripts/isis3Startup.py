#!/usr/bin/env python

import os
from optparse import OptionParser

def setisis():
  #Try to see if ISISROOT is set...
  try:
    ISISROOT = os.environ["ISISROOT"]
  except:
    ISISROOT = None

  #... and if it's not give it a default value
  if ISISROOT is None:
    ISISROOT = "/usgs/pkgs/isis3/isis"
    os.environ['ISISROOT'] = ISISROOT

  #Check for the ISIS3DATA directory. If it does not exist use a default
  if os.path.exists("%s/../data" % (ISISROOT)):
    os.environ['ISIS3DATA'] = "%s/../data" % (ISISROOT)
  else:
    os.environ['ISIS3DATA'] = "/usgs/cpkgs/isis3/data"

  #Check for the ISIS3TESTDATA directory. If it does not exist use a default
  if os.path.exists("%s/../testData" % (ISISROOT)):
    os.environ['ISIS3TESTDATA'] = "%s/../testData" % ISISROOT
  else:
    os.environ['ISIS3TESTDATA'] = "/usgs/cpkgs/isis3/testData"

  #If PATH is not set, just set it to a default location. Else append
  #the isis path to the end of the current path
  try:
    os.environ['PATH']
  except:
    os.environ['PATH'] = "%s/bin" % (ISISROOT)
  else:
    os.environ['PATH'] = "%s:%s/bin" % (os.environ['PATH'], ISISROOT)

if __name__ == "__main__":
  setisis()
