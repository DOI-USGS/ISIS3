APPNAME = lronac2isis
FILE=nacl00015d79

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(FILE).img \
	  to=$(OUTPUT)/$(FILE).cub > /dev/null;
	crop from=$(OUTPUT)/$(FILE).cub nlines=5 \
	  to=$(OUTPUT)/$(FILE).small.cub > /dev/null;
	catlab from=$(OUTPUT)/$(FILE).small.cub \
	  to=$(OUTPUT)/$(FILE).small.pvl > /dev/null;
	$(RM) $(OUTPUT)/$(FILE).cub;
