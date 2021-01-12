# Test cnetextract using nomeasureless and tolerance filters
# The following will be extracted
#		--any points with at least one measure
#		--any points with with line/sample residual greater than 8
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
	  nomeasureless=true \
	  tolerance=true \
	  pixel=8.0 \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/newList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
