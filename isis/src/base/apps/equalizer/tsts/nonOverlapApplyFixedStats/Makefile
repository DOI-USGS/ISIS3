# This tests applying recaluclated statistics (that were recalculated to
# fix non-overlapping images by ADDING new images)
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	#$(LS) $(INPUT)/*cub > $(OUTPUT)/overlapping.lis;
	#@$(SED) -e 's#$(INPUT)\(.*\)cub#$(OUTPUT)/\1\equ\.cub#g' $(OUTPUT)/overlapping.lis \
	#  > $(OUTPUT)/tolist.lis;
	$(LS) $(INPUT)/I10047011EDR.proj.reduced.cub > $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I25685003EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I51718010EDR.crop.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I56969027EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(LS) $(INPUT)/I50695002EDR.proj.reduced.cub >> $(OUTPUT)/fixed.lis;
	$(CAT) $(INPUT)/recalculatedStats.pvl | $(SED) "s|FileName = .*odyssey/testData/|FileName = $(INPUT)/|" > $(OUTPUT)/recalculatedStats.pvl;
	$(APPNAME) fromlist=$(OUTPUT)/fixed.lis \
	  tolist=$(INPUT)/toList.lis \
	  instats=$(OUTPUT)/recalculatedStats.pvl \
	  process=apply \
	  > /dev/null;
	$(MV) *cub $(OUTPUT);
	$(RM) $(OUTPUT)/fixed.lis;
	$(RM) $(OUTPUT)/recalculatedStats.pvl;
