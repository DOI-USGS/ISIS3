APPNAME = pixel2map

crismequi.cub.TOLERANCE = 4
crismequi-count-.cub.TOLERANCE = 4


include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/crism.cub \
	           to=$(OUTPUT)/crismequi.cub \
	           map=$(ISISROOT)/appdata/templates/maps/equirectangular.map \
	           method=wholepixel \

