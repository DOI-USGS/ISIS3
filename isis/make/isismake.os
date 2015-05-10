SHELL=bash

#---------------------------------------------------------------------------
# Get the machine type and setup its environment
#---------------------------------------------------------------------------
HOST_ARCH := $(shell uname -s)
HOST_MACH := $(shell uname -m)
HOST_PROC := $(shell uname -p)

#--------------------------------------------------------------------------
# Setup the general tools needed by other parts of the make system
#--------------------------------------------------------------------------
include $(ISISROOT)/make/isismake.utils

#---------------------------------------------------------------------------
#  Set up ISIS versioning
#---------------------------------------------------------------------------
#ISISVERSIONFULL      := $(shell head -n 1 $(ISISROOT)/version | sed 's/\#.*//' | sed 's/ *$$//')
#ISISMAJOR            := $(shell echo $(ISISVERSIONFULL) | cut -d"." -f1)
#ISISMINOR            := $(shell echo $(ISISVERSIONFULL) | cut -d"." -f2)
#ISISMINORMINOR       := $(shell echo $(ISISVERSIONFULL) | cut -d"." -f3)
#ISISMINORMINORMINOR  := $(shell echo $(ISISVERSIONFULL) | cut -d"." -f4)
#ISISVERSION          := $(ISISMAJOR).$(ISISMINOR).$(ISISMINORMINOR).$(ISISMINORMINORMINOR)
#ISISLIBVERSION       := $(shell echo $(ISISMAJOR).$(ISISMINOR).$(ISISMINORMINOR) | sed 's/[a-z].*$$//')
#ISISLOCALVERSION     ?= $(shell head -n 3 $(ISISROOT)/version | tail -n 1 | sed 's/\#.*//' | sed 's/ *$$//')
#ISISRELEASE          := REL_0_0

# set up HOST_OS
testFile = $(wildcard /etc/os-release)
ifeq ($(testFile), /etc/os-release)
#  HOST_OS = $(shell cut -d ' ' -f 1,2,3,7 /etc/redhat-release | sed 's/release//' | sed 's/ //g' | sed 's/\./_/g')
  HOST_OS := $(strip $(shell $(GREP) '^NAME=' /etc/os-release | $(CUT) -s -d '=' -f 2))$(strip $(shell $(GREP) '^VERSION_ID=' /etc/os-release | $(CUT) -s -d '=' -f 2))
else
  testFile = $(wildcard /usr/bin/sw_vers)
  ifeq ($(testFile), /usr/bin/sw_vers)
    HOST_OS := $(shell sw_vers -productVersion | $(CUT) -d '.' -f1,2 | $(SED) 's/\./_/g')
    HOST_OS := MacOSX$(HOST_OS)
  else
    testFile = $(wildcard /etc/lsb-release)
    ifeq ($(testFile), /etc/lsb-release)
      HOST_OS = $(shell $(GREP) DESCRIPTION /etc/lsb-release | $(SED) 's/\([^"]*"\)\([^"]*\)\(.*\)/\2/' | $(SED) 's/\.[^\.]* LTS//' | $(SED) 's/ //g' | $(SED) 's/\./_/g')
    else
      testFile = $(wildcard /etc/debian_version)
      ifeq ($(testFile), /etc/debian_version)
        HOST_OS := $(shell $(CAT) /etc/debian_version | $(SED) 's/\./_/g' | $(SED) 's/\//_/g')
        HOST_OS := Debian$(HOST_OS)
      endif
    endif
  endif
endif

#---------------------------------------------------------------------------
# Do any OS specific setup
#---------------------------------------------------------------------------
empty:=
space:=$(empty) $(empty)

ifeq ($(findstring $(space),$(HOST_PROC)),$(space))
    HOST_PROC = unknown
endif

ifeq ($(HOST_ARCH),SunOS)
  include $(ISISROOT)/make/config.solaris
endif

ifeq ($(HOST_ARCH),Linux)
  include $(ISISROOT)/make/config.linux
endif

ifeq ($(HOST_ARCH),Darwin)
  include $(ISISROOT)/make/config.darwin
endif

ifndef ISISCPPFLAGS
  $(error Unsupported platform, can not make for $(HOST_ARCH))
endif

# Set up Xalan's command-line option names. Some version of Xalan use different
# option names (e.g. Ubuntu's and Debian's Xalan).
XALAN_VALIDATE_OPTION := -v
XALAN_OUTFILE_OPTION := -o
XALAN_PARAM_OPTION := -p
XALAN_INFILE_OPTION :=
XALAN_XSL_OPTION :=

# Ubuntu and Debian have a different executable name for Xalan, and it also
# does not accept the same command-line argument names as the version of Xalan
# available on the other systems. So, set up some variables to match the
# argument names that this version of Xalan is expecting.
ifneq "$(or $(findstring Ubuntu10_04, $(HOST_OS)), $(findstring Debian, $(HOST_OS)))" ""
  XALAN := xalan
  XALAN_VALIDATE_OPTION := -validate
  XALAN_OUTFILE_OPTION := -out
  XALAN_PARAM_OPTION := -param
  XALAN_INFILE_OPTION := -in
  XALAN_XSL_OPTION := -xsl
endif

ifneq "$(or $(findstring Fedora, $(HOST_OS)), $(findstring ScientificLinux, $(HOST_OS)))" ""
  XALAN = $(ISIS3LOCAL)/bin/Xalan
endif

ifneq "$(or $(findstring MacOSX10_7, $(HOST_OS)), $(findstring MacOSX10_8, $(HOST_OS)))" ""
  GREP = /opt/usgs/$(ISISLOCALVERSION)/ports/bin/grep
endif

#---------------------------------------------------------------------------
# Set up for the required elements in Isis makes
# NOTE: XTRAstuff comes from the make file of specific applications
#---------------------------------------------------------------------------
ALLINCDIRS  = -I.
ALLINCDIRS += $(XTRAINCDIRS) 
ALLINCDIRS += $(ISISINCDIR) 
ALLINCDIRS += $(CWDINCDIR) 
ALLINCDIRS += $(QTINCDIR) 
ALLINCDIRS += $(QWTINCDIR) 
ALLINCDIRS += $(XERCESINCDIR) 
ALLINCDIRS += $(GEOTIFFINCDIR) 
ALLINCDIRS += $(TIFFINCDIR) 
ALLINCDIRS += $(NAIFINCDIR) 
ALLINCDIRS += $(TNTINCDIR)
ALLINCDIRS += $(JAMAINCDIR)
ALLINCDIRS += $(GEOSINCDIR)
ALLINCDIRS += $(GSLINCDIR)
ALLINCDIRS += $(GMMINCDIR)
ALLINCDIRS += $(PROTOBUFINCDIR)
ALLINCDIRS += $(BOOSTINCDIR)
ALLINCDIRS += $(KAKADUINCDIR)
ALLINCDIRS += $(CHOLMODINCDIR)
ALLINCDIRS += $(SUPERLUINCDIR)

ALLLIBDIRS  = -L. 
ALLLIBDIRS += $(XTRALIBDIRS)
ALLLIBDIRS += $(ISISLIBDIR) 
ALLLIBDIRS += $(QTLIBDIR) 
ALLLIBDIRS += $(QWTLIBDIR) 
ALLLIBDIRS += $(XERCESLIBDIR) 
ALLLIBDIRS += $(GEOTIFFLIBDIR) 
ALLLIBDIRS += $(TIFFLIBDIR) 
ALLLIBDIRS += $(NAIFLIBDIR)
ALLLIBDIRS += $(TNTLIBDIR)
ALLLIBDIRS += $(JAMALIBDIR)
ALLLIBDIRS += $(GEOSLIBDIR)
ALLLIBDIRS += $(GSLLIBDIR)
ALLLIBDIRS += $(GMMLIBDIR)
ALLLIBDIRS += $(PROTOBUFLIBDIR)
ALLLIBDIRS += $(BOOSTLIBDIR)
ALLLIBDIRS += $(KAKADULIBDIR)
ALLLIBDIRS += $(CHOLMODLIBDIR)
ALLLIBDIRS += $(SUPERLULIBDIR)

ALLLIBS  = $(ISISLIB) 
ifeq ($(findstring STATIC, $(MODE)),STATIC)
  ALLLIBS = $(ISISSTATIC) $(ISISLIB) $(ISISDYNAMIC)
endif
ALLLIBS += $(ISISSYSLIBS)
ALLLIBS += $(XTRALIBS)
ALLLIBS += $(QTLIB) 
ALLLIBS += $(QWTLIB) 
ALLLIBS += $(XERCESLIB)
ALLLIBS += $(GEOTIFFLIB)
ALLLIBS += $(TIFFLIB)
ALLLIBS += $(NAIFLIB) 
ALLLIBS += $(TNTLIB)
ALLLIBS += $(JAMALIB)
ALLLIBS += $(GEOSLIB)
ALLLIBS += $(GSLLIB)
ALLLIBS += $(X11LIB)
ALLLIBS += $(GMMLIB)
ALLLIBS += $(PROTOBUFLIB)
ALLLIBS += $(BOOSTLIB)
ALLLIBS += $(KAKADULIB)
ALLLIBS += $(CHOLMODLIB)
ALLLIBS += $(SUPERLULIB)

#include $(ISISROOT)/make/isismake.utils

