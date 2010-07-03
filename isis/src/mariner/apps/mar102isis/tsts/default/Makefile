APPNAME = mar102isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MVE_002.117 to=$(OUTPUT)/MVE_002.117.cub \
	> /dev/null;
	catlab from=$(OUTPUT)/MVE_002.117.cub to=$(OUTPUT)/MVE_002.117.pvl \
	> /dev/null;
	rm $(OUTPUT)/MVE_002.117.cub;
