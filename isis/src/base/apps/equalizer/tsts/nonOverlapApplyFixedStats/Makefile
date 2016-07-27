# This tests applying recaluclated statistics (that were recalculated to 
# fix non-overlapping images by ADDING new images)
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	#$(LS) $(INPUT)/*cub > $(OUTPUT)/overlapping.lis;
	#@$(SED) -e 's#$(INPUT)\(.*\)cub#$(OUTPUT)/\1\equ\.cub#g' $(OUTPUT)/overlapping.lis \
	#  > $(OUTPUT)/tolist.lis;
	$(APPNAME) fromlist=$(INPUT)/fixed.lis \
	  tolist=$(INPUT)/toList.lis \
	  instats=$(INPUT)/recalculatedStats.pvl \
	  process=apply \
	  > /dev/null;
	$(MV) *cub $(OUTPUT);
