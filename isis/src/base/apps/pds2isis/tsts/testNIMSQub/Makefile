APPNAME = pds2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/30i001ci.qub \
	 to=$(OUTPUT)/30i001ci.cub > /dev/null;
	catlab from=$(OUTPUT)/30i001ci.cub to=$(OUTPUT)/30i001ci.pvl > /dev/null;
