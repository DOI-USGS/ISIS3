APPNAME = lrowac2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/wac0002dd70.* > $(OUTPUT)/wac-color.lis;
	$(APPNAME) from=$(OUTPUT)/wac-color.lis \
	  to=$(OUTPUT)/wac0002dd70.img > /dev/null;
	catlab from=$(OUTPUT)/wac0002dd70.img \
	  to=$(OUTPUT)/wac0002dd70_full.pvl > /dev/null;
	cat $(OUTPUT)/wac0002dd70_full.pvl \
	  | grep -v "PRODUCT_CREATION_TIME" > $(OUTPUT)/wac0002dd70.pvl;
	pds2isis from=$(OUTPUT)/wac0002dd70.img \
	  to=$(OUTPUT)/wac0002dd70.cub > /dev/null;
	$(RM) $(OUTPUT)/wac0002dd70.img;
	$(RM) $(OUTPUT)/wac0002dd70_full.pvl;
	$(RM) $(OUTPUT)/wac-color.lis;
