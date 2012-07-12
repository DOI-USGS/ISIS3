# Test cnetextract using pointlist filter
# The following will be extracted
#		--every non-ignored point in the point list with at least one measure
#		for each of these points the following measures will be extracted:
#			--every non-ignored measure
#			--every ignored reference measure
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/out \
	  tolist=$(OUTPUT)/newList.lis \
	  onet=$(OUTPUT)/newNet.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  nomeasureless=true \
	  pointlist=$(INPUT)/points.lis \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/newList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
