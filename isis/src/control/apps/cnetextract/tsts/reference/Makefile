# Test cnetextract using reference filter
# The following will be extracted
#		--every point and it's reference, as long as it is not measureless
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
	  reference=true \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/newList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
