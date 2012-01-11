# Tests the capability of using a map projection template file to specify the
# type, direction, and domain for the lat/lon ranges.
APPNAME = camtrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
          to=$(OUTPUT)/camtrimTruth1.cub \
          map=$(INPUT)/sinusoidal.map \
          minlat=-90 \
          maxlat=90 \
          minlon=-136 \
          maxlon=-134 > /dev/null;
