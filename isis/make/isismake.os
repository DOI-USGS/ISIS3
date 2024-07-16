SHELL=bash

#-------------------------------------------------------------------------
# Get the basics setup
#-------------------------------------------------------------------------
include $(ISISROOT)/make/isismake.init

#---------------------------------------------------------------------------
#  Set up ISIS versioning
#---------------------------------------------------------------------------
ISISVERSIONFULL      := $(shell $(HEAD) -n 1 $(ISISROOT)/isis_version.txt | $(SED) 's/\#.*//' | $(SED) 's/ *$$//')
ISISMAJOR            := $(shell $(ECHO) $(ISISVERSIONFULL) | $(CUT) -d"." -f1)
ISISMINOR            := $(shell $(ECHO) $(ISISVERSIONFULL) | $(CUT) -d"." -f2)
ISISMINORMINOR       := $(shell $(ECHO) $(ISISVERSIONFULL) | $(CUT) -d"." -f3)
ISISMINORMINORMINOR  := $(shell $(ECHO) $(ISISVERSIONFULL) | $(CUT) -d"." -f4)
ISISVERSION          := $(ISISMAJOR).$(ISISMINOR).$(ISISMINORMINOR).$(ISISMINORMINORMINOR)
ISISLIBVERSION       := $(shell $(ECHO) $(ISISMAJOR).$(ISISMINOR).$(ISISMINORMINOR) | $(SED) 's/[a-z].*$$//')
ISISLOCALVERSION     := $(shell $(HEAD) -n 3 $(ISISROOT)/isis_version.txt | $(TAIL) -n 1 | $(SED) 's/\#.*//' | $(SED) 's/ *$$//')
ISISRELEASE          := REL_0_0

# Set up HOST_OS
ifneq ($(wildcard /etc/os-release),)
  HOST_OS := $(strip \
               $(shell $(GREP) '^NAME=' /etc/os-release | \
                 $(CUT) -s -d '=' -f 2 | \
                 $(SED) -e 's/^"//' -e 's/"$$//' | \
                 $(CUT) -d ' ' -f 1))$(strip \
               $(shell grep '^VERSION_ID=' /etc/os-release | \
                 $(CUT) -s -d '=' -f 2 | \
                 $(SED) -e 's/^"//' -e 's/"$$//'))
  HOST_OS := $(subst .,_,$(HOST_OS))

else ifneq ($(wildcard /etc/redhat-release),)
    HOST_OS = $(shell cut -d ' ' -f 1,2,3,7 /etc/redhat-release | sed 's/release//' | sed 's/ //g' | sed 's/\./_/g')

else ifneq ($(wildcard /usr/bin/sw_vers),)
  HOST_OS := $(shell sw_vers -productVersion | $(CUT) -d '.' -f1,2 | $(SED) 's/\./_/g')
  HOST_OS := MacOSX$(HOST_OS)

else ifneq ($(wildcard /etc/lsb-release),)
      HOST_OS = $(shell $(GREP) DESCRIPTION /etc/lsb-release | $(SED) 's/\([^"]*"\)\([^"]*\)\(.*\)/\2/' | $(SED) 's/\.[^\.]* LTS//' | $(SED) 's/ //g' | $(SED) 's/\./_/g')

else ifneq ($(wildcard /etc/debian_version),)
  HOST_OS := $(shell $(CAT) /etc/debian_version | $(SED) 's/\./_/g' | $(SED) 's/\//_/g')
  HOST_OS := Debian$(HOST_OS)

endif

ifeq ($(findstring $(space),$(HOST_PROC)),$(space))
    HOST_PROC = unknown
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

# Set up Xalan's command-line option names.
XALAN_VALIDATE_OPTION := -v
XALAN_OUTFILE_OPTION := -o
XALAN_PARAM_OPTION := -p
XALAN_INFILE_OPTION :=
XALAN_XSL_OPTION :=

#---------------------------------------------------------------------------
# The BSD version of grep on 10.7-10.9 is reported to be broken so use GNU
#---------------------------------------------------------------------------
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
ALLINCDIRS += $(HDF5INCDIR)
ALLINCDIRS += $(EIGENINCDIR)
ALLINCDIRS += $(FLANNINCDIR)
ALLINCDIRS += $(QHULLINCDIR)
ALLINCDIRS += $(PCLINCDIR)
ALLINCDIRS += $(VTKINCDIR)
ALLINCDIRS += $(SUPERLUINCDIR)
ALLINCDIRS += $(OPENCVINCDIR)
ALLINCDIRS += $(NNINCDIR)
ALLINCDIRS += $(BULLETINCDIR)
ALLINCDIRS += $(EMBREEINCDIR)
ALLINCDIRS += $(DEFAULTINCDIR)

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
ALLLIBDIRS += $(HDF5LIBDIR)
ALLLIBDIRS += $(PCLLIBDIR)
ALLLIBDIRS += $(VTKLIBDIR)
ALLLIBDIRS += $(SUPERLULIBDIR)
ALLLIBDIRS += $(BULLETLIBDIR)
ALLLIBDIRS += $(EMBREELIBDIR)

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
ALLLIBS += $(HDF5LIB)
ALLLIBS += $(PCLLIB)
ALLLIBS += $(VTKLIB)
ALLLIBS += $(SUPERLULIB)
ALLLIBS += $(BULLETLIB)
ALLLIBS += $(EMBREELIB)
