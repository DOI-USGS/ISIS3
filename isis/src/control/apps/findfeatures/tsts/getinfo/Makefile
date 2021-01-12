# This tests the LISTALL, LISTMATCHERS, LISTSPEC, and TOINFO params.
# Also, ALGORITHM and ALGOSPECFILE are tested as well in terms of 
#   making sure we can grab the specified algorithm information.
# The first test tests without TOINFO (i.e. to stdout),
#   while the rest use TOINFO to get adequate scope coverage.
APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) listall=yes \
	           > $(OUTPUT)/listall.pvl
	           >& /dev/null;
	$(APPNAME) algorithm=brisk/brisk \
	           listspec=yes \
	           toinfo=$(OUTPUT)/listspec.pvl \
	           >& /dev/null;
	$(APPNAME) algospecfile=$(INPUT)/algorithms.txt \
	           listspec=yes \
	           toinfo=$(OUTPUT)/listspecs.pvl \
	           >& /dev/null;
