include $(ISISROOT)/make/isismake.os

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
#ISISLOCALVERSION     := $(shell head -n 3 $(ISISROOT)/version | tail -n 1 | sed 's/\#.*//' | sed 's/ *$$//')
#ISISRELEASE          := REL_0_0

.NOTPARALLEL:

#----------------------------------------------------------------------------
# Let the user know how to use the system
#----------------------------------------------------------------------------
help:
	echo "Isis Make System Commands"
	echo "------------------------ "
	echo "make all        : Build and install the entire system (incs,api,apps,docs)"
	echo "make thirdParty : Copy required 3rdParty libraries into distribution"
	echo "make incs       : Install API include files"
	echo "make api        : Build and install the Isis API"
	echo "make apps       : Build and install Isis Applications"
	echo "make docs       : Build Isis Web Documentation"
	echo "make quickclean : Clean binaries from source tree but not app test areas"
	echo "make cleansrc   : Clean binaries from source tree"
	echo "make clean      : Clean source tree and install area"
	echo "make unitTest   : Build and execute unit tests for Isis API"
	echo "make appTest    : Build and execute application tests"
	echo "make catTest    : Build and execute category tests"
	echo "make coverage   : Build a test coverage report from generated coverage info"

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
	echo $(CURTIMESTAMP) "Installing include files"
	mkdir -p inc
	$(MAKE) --directory=src includes
	echo $(CURTIMESTAMP) "Finished installing include files"
	echo $(CURTIMESTAMP) " "

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
	echo $(CURTIMESTAMP) "Building Isis API"
	mkdir -p lib
	$(MAKE) --directory=src objects
	echo $(CURTIMESTAMP) "Finished building Isis API"
	echo $(CURTIMESTAMP) ""
	echo $(CURTIMESTAMP) "Adding API objects"
	if [ "$(HOST_ARCH)" == "Linux" ]; then            \
		$(MAKE) --directory=src api;	                  \
	elif [ "$(HOST_ARCH)" == "Darwin" ]; then         \
		$(MAKE) osx_static;															\
	fi;
	echo $(CURTIMESTAMP) "Finished adding API objects"
	echo $(CURTIMESTAMP) " "
	echo $(CURTIMESTAMP) "Creating Shared Libraries ..."
	cp make/Makefile.libs lib/Makefile
	$(MAKE) --directory=lib shared
	echo $(CURTIMESTAMP) "Finished creating Shared Libraries ..."
	echo $(CURTIMESTAMP) " "

# Make the static library on Macos, faster then recursivly call make
osx_static:
	$(eval UNIT_TESTS=$(wildcard src/*/objs/*/unitTest.o))
	$(eval PLUGIN_DIRS=$(dir $(wildcard src/*/objs/*/*.plugin)))
	$(eval PLUGIN_FILES=$(foreach dir,$(PLUGIN_DIRS),$(wildcard $(dir)*.o)))
	$(eval API_OBJS=$(filter-out $(PLUGIN_FILES),$(filter-out $(UNIT_TESTS),$(wildcard src/*/objs/*/*.o))))
	$(AR) -crs $(ISISROOT)/lib/libisis$(ISISLIBVERSION).a $(API_OBJS);
	for i in $(PLUGIN_DIRS); do 			  					\
		$(MAKE) --directory=$$i plugin install; 	  \
	done

#	echo "Building Isis API with debugging"
#	cd src; $(MAKE) objects MODE=DEBUG
#	cd src; $(MAKE) api MODE=DEBUG
#	echo "Finished building Isis API with debugging"
#	echo " "

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
	echo $(CURTIMESTAMP) "Building Isis Applications"
	mkdir -p bin/xml
	$(MAKE) --directory=src applications
	echo $(CURTIMESTAMP) "Finished building Isis Applications"
	echo $(CURTIMESTAMP) " "

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
	echo $(CURTIMESTAMP) "Building Isis Documentation"
	mkdir -p doc
	$(MAKE) --directory=src documentation
	$(MAKE) --directory=src/docsys docs
	echo $(CURTIMESTAMP) "Finished building Isis Documentation"


#----------------------------------------------------------------------------
# Target = coverage
# Dependencies = none
#
# This target builds a report on how much of Isis is tested in the automated
# tests. This is currently excluding applications that run other applications
# because of the amount of time it takes to generate the report (and that's
# not how we want to test our applications ideally).
#----------------------------------------------------------------------------
coverage:
	if [ "$(CODE_COVERAGE_BIN_DIR)" == "" ]; then                         \
	  echo "Make sure you use MODE=TC when building Isis for a coverage"  \
	       "report.";                                                     \
	  exit 1;                                                             \
	fi;                                                                   \
	$(ECHO) $(CURTIMESTAMP) "Finding coverage information...";            \
	CSMESFILES=( lib/libisis$(ISISLIBVERSION).so.csmes                    \
	             `find src/*/apps -name "*.o.csmes" | grep -v moc_` );    \
	CSEXEFILES=(`find src/ -name "*.csexe"` `find bin/ -name "*.csexe"`); \
	if [ "$${#CSMESFILES[@]}" == "0" ] ||                                 \
	   [ "$${#CSEXEFILES[@]}" == "0" ]; then                              \
	  echo "Please build Isis and run the tests with MODE=TC before "     \
	       "trying to build a coverage report.";                          \
	  exit 1;                                                             \
	fi;                                                                   \
	$(ECHO) $(CURTIMESTAMP) "Merging source file information into isis.csmes..."; \
	$(CODE_COVERAGE_BIN_DIR)/cmmerge $${CSMESFILES[@]} -o isis.csmes;     \
	$(ECHO) $(CURTIMESTAMP) "Adding test execution records into isis.csmes..."; \
	for CSEXEFILE in $${CSEXEFILES[@]}; do                                \
	  EXETESTNAME="$$CSEXEFILE";                                          \
	  APPTESTAPPNAME=`echo $$CSEXEFILE | sed 's#\\(.*/apps/\\)\\([a-zA-Z0-9]*\\)\\(/tsts/\\)\([^/]*\)\(/.*\)#\\2#'`; \
	  CSEXEAPPNAME=`basename $$CSEXEFILE | sed 's/.csexe//'`;             \
	  if [ "$$APPTESTAPPNAME" == "$$CSEXEFILE" ] ||                       \
	     [ "$$APPTESTAPPNAME" == "$$CSEXEAPPNAME" ] ||                    \
	     [ "`dirname $$CSEXEFILE`" == "bin" ]; then                       \
	    EXETESTNAME=`echo $$EXETESTNAME | sed 's#\\(.*/apps/\\)\\([a-zA-Z0-9]*\\)\\(/tsts/\\)\([^/]*\)\(/.*\)#\\2 Application Test (case \\4\\)#'`;      \
	    EXETESTNAME=`echo $$EXETESTNAME | sed 's#\\(src/\\)\\([a-zA-Z0-9]*\\)\\(/tsts/\\)\\([a-zA-Z0-9]*\\)\\(.*\\)#\\2 Category Test (case \\4\\)#'`; \
	    EXETESTNAME=`echo $$EXETESTNAME | sed 's#\\(.*/\\)\\([^/]*\\)\\(/unitTest.csexe\\)#\\2 Unit Test#'`;                                           \
	    $(ECHO) -e $(CURTIMESTAMP) "  Adding [$$EXETESTNAME] `basename $$CSEXEFILE`"; \
	    $(CODE_COVERAGE_BIN_DIR)/cmcsexeimport -t "$$EXETESTNAME"             \
	        -e $$CSEXEFILE -m "isis.csmes";                                   \
	  fi;                                                                     \
	done;                                                                     \
	$(ECHO) -ne $(CURTIMESTAMP) "Scope Coverage:      ";                      \
	$(CODE_COVERAGE_BIN_DIR)/cmreport                                         \
	    --title="Isis System-Wide Test Scope Coverage"                        \
	    -m "`basename $$PWD`.csmes" --select=".*" --bargraph --toc --stat     \
	    --global=all --method=all --source=all --execution=all                \
	    --method-sort=coverage --execution-sort=coverage                      \
	    --source-sort=coverage -h scopecoverage;                              \
	$(ECHO) -ne $(CURTIMESTAMP) "Line Coverage:       ";                      \
	$(CODE_COVERAGE_BIN_DIR)/cmreport                                         \
	    --title="Isis System-Wide Test Line Coverage"                         \
	    -m "`basename $$PWD`.csmes" --select=".*" --bargraph --toc --stat     \
	    --global=all --method=all --source=all --execution=all                \
	    --method-sort=coverage --execution-sort=coverage                      \
	    --source-sort=coverage --line-coverage -h linecoverage;               \
	$(ECHO) -ne $(CURTIMESTAMP) "Function Coverage:   ";                      \
	$(CODE_COVERAGE_BIN_DIR)/cmreport                                         \
	    --title="Isis System-Wide Test Function Coverage"                     \
	    -m "`basename $$PWD`.csmes" --select=".*" --bargraph --toc --stat     \
	    --global=all --method=all --source=all --execution=all                \
	    --method-sort=coverage --execution-sort=coverage                      \
	    --source-sort=coverage --function-coverage -h functioncoverage;


#----------------------------------------------------------------------------
# Target = quickclean
#
# This target cleans the entire Isis system enough for a recompile.  It cleans ".o" files and
# binary executables from the source tree.  It also, clears the running
# areas under $ISISROOT (inc, doc, bin, bin/xml, and lib)
#
# Target = quickclean
# Dependencies = none
#
# This walks the src tree and removes ".o" files and binary files
#----------------------------------------------------------------------------
quickclean:
	echo $(CURTIMESTAMP) "Cleaning Isis (quick)"; \
	$(MAKE) --directory=src quickclean;   \
	rm -rf inc;                           \
	rm -rf doc;                           \
	rm -rf bin;                           \
	rm -rf lib;                           \
	rm -rf scripts/tabcomplete.csh;       \
	rm -rf scopecoverage scopecoverage.html linecoverage linecoverage.html \
	  functioncoverage functioncoverage.html *.csmes *.csexe; \
	$(MAKE) --directory=3rdParty clean;   \
	echo $(CURTIMESTAMP) "Finished cleaning Isis";

#----------------------------------------------------------------------------
# Target = clean
# Dependencies = cleansrc
#
# This target cleans the entire Isis system.  It cleans ".o" files and
# binary executables from the source tree.  It also clears the running
# areas under $ISISROOT (inc, doc, bini, bin/xml, and lib)
#
# Target = cleansrc
# Dependencies = none
#
# This walks the src tree and removes ".o" files and binary files
#----------------------------------------------------------------------------
clean:
	echo $(CURTIMESTAMP) "Cleaning Isis"; \
	$(MAKE) cleansrc;                     \
	rm -rf inc;                           \
	rm -rf doc;                           \
	rm -rf bin;                           \
	rm -rf lib;                           \
	rm -rf scripts/tabcomplete.csh;       \
	rm -rf scopecoverage scopecoverage.html linecoverage linecoverage.html \
	  functioncoverage functioncoverage.html *.csmes *.csexe; \
	$(MAKE) --directory=3rdParty clean;   \
	echo $(CURTIMESTAMP) "Finished cleaning Isis";

cleansrc:
	$(MAKE) --directory=src clean

tabcomplete:
	if [ ! -d bin ]; then                                                    \
	  echo $(CURTIMESTAMP) "You must build the applications first";          \
	elif [ ! -f bin/isiscomplete ]; then                                     \
	  echo $(CURTIMESTAMP) "Isis application 'isiscomplete' is missing";     \
	else                                                                     \
	  echo "#!/bin/csh" > $$ISISROOT/scripts/tabcomplete.csh;                \
	  isiscomplete `ls $$ISISROOT/bin | grep -v xml` | sed 's/; /;~/g' |     \
	               tr '~' '\n' > $$ISISROOT/scripts/tabcomplete.csh;         \
	fi;

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
	echo $(CURTIMESTAMP) "Testing Isis API"
	$(MAKE) --directory=src unitTest
	echo $(CURTIMESTAMP) "Finished testing Isis API"

appTest:
	echo $(CURTIMESTAMP) "Testing Isis Applications version 2"
	$(MAKE) --directory=src appTest
	echo $(CURTIMESTAMP) "Finished testing Isis Applications version 2"

catTest:
	echo $(CURTIMESTAMP) "Testing Isis Category"
	$(MAKE) --directory=src catTest
	echo $(CURTIMESTAMP) "Finished testing Isis Category"

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
HOST_MACH ?= $(shell uname -m)

thirdParty:
	echo $(CURTIMESTAMP) "Installing 3rdParty libraries"
	rm -f $(ISISROOT)/3rdParty/lib/lib*
	$(MAKE) -C $(ISISROOT)/3rdParty install
	echo $(CURTIMESTAMP) "Finished installing 3rdParty libraries"
	echo $(CURTIMESTAMP) " "


#----------------------------------------------------------------------------
# Use to see values of variables
#  Example: make print-HOST_OS
#  Will print the make variable HOST_OS
#----------------------------------------------------------------------------
#print-%  :
#	@echo '$* = $($*)'

#----------------------------------------------------------------------------
# Standard make FORCE target. Do not remove unless you know what you are doing
#----------------------------------------------------------------------------

FORCE:

#----------------------------------------------------------------------------
# Include the make file debugging targets
#----------------------------------------------------------------------------
include $(ISISROOT)/make/isismake.print
