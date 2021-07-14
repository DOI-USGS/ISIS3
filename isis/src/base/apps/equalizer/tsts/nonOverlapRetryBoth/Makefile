# This tests calculating statistics for input with non-overlapping files,
# correcting the input list by REMOVING non-overlaps and recalculating to
# determine normalization gains and offsets for equalization,
# and then applying corrections.
#
# NOTE: An exception occurs when there are non-overlaps during calculation,
# it is handled and the output is sent to nonOverlapError.txt
APPNAME = equalizer

I10047011EDR.proj.reduced.cub.TOLERANCE = .00001

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
	fi
	$(LS) $(INPUT)/I10047011EDR.proj.reduced.cub > $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I25685003EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I51718010EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I56969027EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I50695002EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(APPNAME) fromlist=$(OUTPUT)/fixed.lis \
	  tolist=$(INPUT)/toList.lis \
	  instats=$(OUTPUT)/nonOverlapStats.pvl \
	  outstats=$(OUTPUT)/recalculatedStats.pvl \
	  process=RETRYBOTH \
	  > /dev/null;
	$(MV) *.cub $(OUTPUT);
	$(RM) $(OUTPUT)/fixed.lis;
	$(RM) $(OUTPUT)/nonOverlap.lis;

