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
	$(LS) $(INPUT)/I10047011EDR.proj.reduced.cub > $(OUTPUT)/nonOverlap.lis;
	$(LS) $(INPUT)/I25685003EDR.crop.proj.reduced.cub >> $(OUTPUT)/nonOverlap.lis;
	$(LS) $(INPUT)/I51718010EDR.crop.proj.reduced.cub >> $(OUTPUT)/nonOverlap.lis;
	$(LS) $(INPUT)/I56969027EDR.proj.reduced.cub >> $(OUTPUT)/nonOverlap.lis;
	if [[ `$(APPNAME) \
	  fromlist=$(OUTPUT)/nonOverlap.lis \
	  outstats=$(OUTPUT)/nonOverlapStats.pvl \
	  process=CALCULATE \
	  2>> $(OUTPUT)/nonOverlapError.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	# A)
	$(LS) $(INPUT)/I10047011EDR.proj.reduced.cub > $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I25685003EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I51718010EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I56969027EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I50695002EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
#	$(CAT) $(INPUT)/nonOverlapStats.pvl | $(SED) "s|FileName = .*odyssey/testData/|FileName = $(INPUT)/|" > $(OUTPUT)/nonOverlapStats.pvl;
	$(APPNAME) fromlist=$(OUTPUT)/fixed.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStats.pvl \
	  process=RECALCULATE \
	  > /dev/null;
	# B)
	$(LS) $(INPUT)/I25685003EDR.crop.proj.reduced.cub > $(OUTPUT)/fixedDiffOrder.lis;
	$(LS) $(INPUT)/I50695002EDR.proj.reduced.cub >> $(OUTPUT)/fixedDiffOrder.lis;
	$(LS) $(INPUT)/I56969027EDR.proj.reduced.cub >> $(OUTPUT)/fixedDiffOrder.lis;
	$(LS) $(INPUT)/I51718010EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixedDiffOrder.lis;
	$(LS) $(INPUT)/I10047011EDR.proj.reduced.cub >> $(OUTPUT)/fixedDiffOrder.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fixedDiffOrder.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStatsDiffOrder.pvl \
	  process=RECALCULATE \
	  > /dev/null;
	$(RM) $(OUTPUT)/fixed.lis;
	$(RM) $(OUTPUT)/nonOverlap.lis;
	$(RM) $(OUTPUT)/fixedDiffOrder.lis;

