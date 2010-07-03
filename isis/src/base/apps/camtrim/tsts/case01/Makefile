APPNAME = camtrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
          to=$(OUTPUT)/camtrimTruth1.cub \
          minlat=-90 \
          maxlat=90 \
          minlon=-360 \
          maxlon=360 > /dev/null;
