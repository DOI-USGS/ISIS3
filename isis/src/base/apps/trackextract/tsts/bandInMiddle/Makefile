# Verify that we can extract the tracking band that is not the last band in the cube.
#
# history 2018-07-27 Kaitlyn Lee - Original version.

APPNAME = trackextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/automosTruth1.cub \
		to=$(OUTPUT)/mosaic > /dev/null;
