APPNAME = lronac2pds
FILE=nacl00015d79

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(FILE).cal.cub \
	  to=$(OUTPUT)/$(FILE).img > /dev/null;
	catlab from=$(OUTPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE)_full.pvl > /dev/null;
	cat $(OUTPUT)/$(FILE)_full.pvl \
	  | grep -v "PRODUCT_CREATION_TIME" > $(OUTPUT)/$(FILE).pvl;
	pds2isis from=$(OUTPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null;
	$(RM) $(OUTPUT)/$(FILE).img;
	$(RM) $(OUTPUT)/$(FILE)_full.pvl;
