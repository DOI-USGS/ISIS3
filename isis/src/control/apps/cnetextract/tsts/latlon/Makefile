# Test cnetextract using latlon range filters
# The following will be extracted
#		--any points within the given lat/lon range
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
	  latlon=true \
	  minlat=15.0 \
	  maxlat=28.0 \
	  minlon=20.0 \
	  maxlon=28.0 \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/newList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
