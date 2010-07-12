APPNAME = lronac2isis
FILE=nacl0002df94

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null;
	crop from=$(OUTPUT)/$(FILE).cub nlines=5 \
	  to=$(OUTPUT)/$(FILE).small.cub > /dev/null;
	catlab from=$(OUTPUT)/$(FILE).small.cub \
	  > $(OUTPUT)/$(FILE).small.pvl;
	$(RM) $(OUTPUT)/$(FILE).cub;
