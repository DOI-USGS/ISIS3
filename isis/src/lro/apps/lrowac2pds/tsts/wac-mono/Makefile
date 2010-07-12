APPNAME = lrowac2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/wac0003330c* > $(OUTPUT)/wac-mono.lis;
	$(APPNAME) from=$(OUTPUT)/wac-mono.lis \
	  to=$(OUTPUT)/wac0003330c.img > /dev/null;
	catlab from=$(OUTPUT)/wac0003330c.img \
	  to=$(OUTPUT)/wac0003330c_full.pvl > /dev/null;
	cat $(OUTPUT)/wac0003330c_full.pvl \
	  | grep -v "PRODUCT_CREATION_TIME" > $(OUTPUT)/wac0003330c.pvl;
	pds2isis from=$(OUTPUT)/wac0003330c.img \
	  to=$(OUTPUT)/wac0003330c.cub > /dev/null;
	$(RM) $(OUTPUT)/wac0003330c.img;
	$(RM) $(OUTPUT)/wac0003330c_full.pvl;
	$(RM) $(OUTPUT)/wac-mono.lis;
