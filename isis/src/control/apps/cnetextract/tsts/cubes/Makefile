# Tests cnetextract using CUBES filters
#
# (1) The first run is a basic test of the filter. It extracts
#		--non-ignored points that have at least one measure in the cubelist 
#		for each of these points the following measures will be extracted:
#			--reference measures (ignored or not)
# 			--non-ignored measures
# (2) The second run uses an extra filter to remove any measures that are not
#	  in the cubelist 
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/point \
	  tolist=$(OUTPUT)/newList.lis \
	  onet=$(OUTPUT)/newNetPoint.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  cubes=true \
	  cubelist=$(OUTPUT)/list.lis \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/pointNewList.txt;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/measure \
	  tolist=$(OUTPUT)/newList.lis \
	  onet=$(OUTPUT)/newNetMeasure.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  cubes=true cubelist=$(OUTPUT)/list.lis \
	  cubemeasures=true \
	  retain_reference=true \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/measureNewList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
