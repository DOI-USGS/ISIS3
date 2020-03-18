APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/crism.cub \
	           to=$(OUTPUT)/crismequi.cub \
	           map=$(ISISROOT)/appdata/maps/equirectangular.map \
	           method=wholepixel \
			   > /dev/null;
