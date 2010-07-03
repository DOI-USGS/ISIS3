APPNAME = cropspecial

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/4band.cub+1-4 to=$(OUTPUT)/4crop.cub \
	HRS=yes LRS=yes HIS=yes LIS=yes \
	> /dev/null;
