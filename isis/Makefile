include $(ISISROOT)/make/isismake.macros

#----------------------------------------------------------------------------
# Let the user know how to use the system
#----------------------------------------------------------------------------
help:
	echo "Isis Make System Commands"
	echo "------------------------ "
	echo "make all       : Build and install the entire system (incs,api,apps,docs)"
	echo "make incs      : Install API include files"
	echo "make api       : Build and install the Isis API"
	echo "make apps      : Build and install Isis Applications"
	echo "make docs      : Build Isis Web Documentation"
	echo "make cleansrc  : Clean binaries from source tree" 
	echo "make clean     : Clean source tree and install area"
	echo "make unitTest  : Build and execute unit tests for Isis API"
	echo "make appTest   : Build and execute application tests"
	echo "make catTest   : Build and execute category tests"
	echo "make thirdParty: Copy required 3rdParty libraries into distribution"

#----------------------------------------------------------------------------
# Target = all
# Dependencies = includes api applications documentation thirdParty
#
# The API include files must be installed before the API can be constructed.
# After the API is created then the applications can be individually built
# and installed. Finally create the web documentation for the entire system.
#----------------------------------------------------------------------------
all: incs thirdParty api apps docs

#----------------------------------------------------------------------------
# Target = incs
# Dependencies = none
#
# The API include files will be installed in $(ISISROOT)/inc.  The Isis 
# make system will traverse the objs directories (src/base/objs/*) and
# copy the include file from the individual object directory into the
# system directory $(ISISROOT)/inc.
#----------------------------------------------------------------------------
incs:
	echo "Installing include files"
	mkdir -p inc
	cd src; $(MAKE) includes
	echo "Finished installing include files"
	echo " "

#----------------------------------------------------------------------------
# Target = api
# Dependencies = objects
#
# The Isis API is essentially libisis.a which will be created in the
# directory $(ISISROOT)/lib.  Make api traverses the objs directories
# (src/base/objs/*) and archives the ".o" files into libisis.a.  Therefore
# the ".o" files must exist.
#
# Target = objects
# Dependencies = none
#
# Before the API can be built each individual object must be built.  
# This target will traverse the objs directories and create the C++
# object classes leaving the ".o" file in each individual directory.
# The API is not created until "make api" is performed
#
# Finally after the API is completed the shared libraries will be
# constructed from libisis.a
#----------------------------------------------------------------------------
api:
	echo "Building Isis API" 
	mkdir -p lib
	cd src; $(MAKE) objects
	cd src; $(MAKE) api
	echo "Finished building Isis API" 
	echo " "

#	echo "Building Isis API with debugging" 
#	cd src; $(MAKE) objects MODE=DEBUG
#	cd src; $(MAKE) api MODE=DEBUG
#	echo "Finished building Isis API with debugging" 
#	echo " "

	echo "Creating Shared Libraries ..."
	cp make/Makefile.libs lib/Makefile
	cd lib; $(MAKE) shared
	echo "Finished creating Shared Libraries ..."
	echo " "

#----------------------------------------------------------------------------
# Target = apps
# Dependencies = none
#
# This will build and install all the Isis application in the directory
# $(ISISROOT)/bin.  It also installs the application Xml file in
# $(ISISROOT/bin/xml.  Again the make system traverse the apps directories
# (src/base/apps) and builds/installs each application.  Of course the
# API must be built for this to work properly
#----------------------------------------------------------------------------
apps: 
	echo "Building Isis Applications" 
	mkdir -p bin/xml
	cd src; $(MAKE) applications
	echo "Finished building Isis Applications" 
	echo " "

#----------------------------------------------------------------------------
# Target = docs
# Dependencies = none
#
# This target traverse both the objs and apps directories and moves the
# Xml and assets directories into specific sub-directories under
# $(ISISROOT)/src/docsys.  Then it builds the entire docsys tree which
# generates the Isis Documentation under $(ISISROOT)/doc
#----------------------------------------------------------------------------
docs:
	echo "Building Isis Documentation" 
	mkdir -p doc
	cd src; $(MAKE) documentation
	cd src/docsys; $(MAKE) docs
	echo "Finished building Isis Documentation" 

#----------------------------------------------------------------------------
# Target = clean
# Dependencies = cleansrc
#
# This target cleans the entire Isis system.  It cleans ".o" files and
# binary executables from the source tree.  It also, clears the running
# areas under $ISISROOT (inc, doc, bini, bin/xml, and lib)
#
# Target = cleansrc
# Dependencies = none
#
# This walks the src tree and removes ".o" files and binary files
#----------------------------------------------------------------------------
clean: cleansrc
	echo "Cleaning Isis"
	cd src; $(MAKE) clean
	rm -rf inc
	rm -rf doc
	rm -rf bin
	rm -rf lib
	cd 3rdParty; $(MAKE) clean
	echo "Finished Cleaning Isis"

cleansrc:
	cd src; $(MAKE) clean

#----------------------------------------------------------------------------
# Target = unitTest appTest appTest2 catTest
# Dependencies = none
#
# This target traverses both the objs and apps directories. In the object
# directories it will build the unitTest executable, run it, and difference
# the results with the unitTest truth data.  In the application directories
# it will run the appTest script to ensure the program generates the proper
# output data (whether it be a cube, text file, postscript file, etc)
#----------------------------------------------------------------------------
unitTest:
	echo "Testing Isis API" 
	cd src; $(MAKE) unitTest
	echo "Finished testing Isis API" 

appTest:
	echo "Testing Isis Applications version 2" 
	cd src; $(MAKE) appTest
	echo "Finished testing Isis Applications version 2"

catTest:
	echo "Testing Isis Category" 
	cd src; $(MAKE) catTest
	echo "Finished testing Isis Category"

#----------------------------------------------------------------------------
# Target = thirdParty
# Dependencies = none
#
# This target is used only for external distributions that do not have
# the need to program.  As a convenience we provide the shared libraries
# for 3rdParty packages used in ISIS so that external customers do
# not have to download and install RPMs.
#----------------------------------------------------------------------------
HOST_ARCH ?= $(shell uname -s)
HOST_MACH  = $(shell uname -m)

thirdParty:
	echo "Installing 3rdParty Libraries"
	rm -f $(ISISROOT)/3rdParty/lib/lib*
	echo $(HOST_ARCH)
	$(MAKE) -C $(ISISROOT)/3rdParty install

