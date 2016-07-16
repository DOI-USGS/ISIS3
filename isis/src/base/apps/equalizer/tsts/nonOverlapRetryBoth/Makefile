# This tests calculating statistics for input with non-overlapping files,
# correcting the input list by REMOVING non-overlaps and recalculating to 
# determine normalization gains and offsets for equalization, 
# and then applying corrections.
#
# NOTE: An exception occurs when there are non-overlaps during calculation,
# it is handled and the output is sent to nonOverlapError.txt
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	# Handle the exception: "There are input images that do not overlap..."
	if [[ `$(APPNAME) \
	  fromlist=$(INPUT)/nonOverlap.lis \
	  outstats=$(OUTPUT)/nonOverlapStats.pvl \
	  process=CALCULATE \
	  2>> $(OUTPUT)/nonOverlapError.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi
	$(APPNAME) fromlist=$(INPUT)/fixed.lis \
	  tolist=$(INPUT)/toList.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStats.pvl \
	  process=RETRYBOTH \
	  > /dev/null;
	$(MV) *.cub $(OUTPUT);
