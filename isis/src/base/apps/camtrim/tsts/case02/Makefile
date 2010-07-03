APPNAME = camtrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
          to=$(OUTPUT)/camtrimTruth2.cub \
          minlat=-45 \
          maxlat=45 \
          minlon=-180 \
          maxlon=180 > /dev/null;
