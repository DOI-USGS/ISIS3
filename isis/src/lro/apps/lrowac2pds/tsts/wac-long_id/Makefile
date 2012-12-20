APPNAME = lrowac2pds
FILE=M1109308373CE

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/$(FILE).*.cub > $(OUTPUT)/$(FILE).lis;
	$(APPNAME) from=$(OUTPUT)/$(FILE).lis \
	  to=$(OUTPUT)/$(FILE).img > /dev/null;
	catlab from=$(OUTPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE)_full.pvl > /dev/null;
	cat $(OUTPUT)/$(FILE)_full.pvl \
	  | grep -v "PRODUCT_CREATION_TIME" > $(OUTPUT)/$(FILE).pvl;
	pds2isis from=$(OUTPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null;
	$(RM) $(OUTPUT)/$(FILE).lis;
	$(RM) $(OUTPUT)/$(FILE).img;
	$(RM) $(OUTPUT)/$(FILE)_full.pvl;
