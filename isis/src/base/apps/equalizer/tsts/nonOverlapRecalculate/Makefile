# This tests calculating statistics for input with non-overlapping files,
# then correcting the input list by ADDING a new image,
# and recalculating to determine normalization gains and offsets for equalization.
#
# A) First, we simply append the new image to the end of the original list of images to fix the
#    non-overlapping areas.
# B) Second, we rearrange the original list of input images and add the new image in the middle, 
#    since order should not matter. This ensures that in the input list, if an existing overlap's
#    Y (second) image comes before the X (first) image, the overlap is properly added and 
#    normalizations are calculated correctly.
#
# The two output statistics pvl files should have identical information, just in a different order.
# 
# NOTE: An exception occurs when calculating and there are non-overlaps,
#       it is redirected to nonOverlapError.txt.
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
	fi;
	# A)
	$(APPNAME) fromlist=$(INPUT)/fixed.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStats.pvl \
	  process=RECALCULATE \
	  > /dev/null;
	# B)
	$(APPNAME) fromlist=$(INPUT)/fixedDiffOrder.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStatsDiffOrder.pvl \
	  process=RECALCULATE \
	  > /dev/null;
