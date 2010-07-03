APPNAME = fits2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/WFPC2u5780205r_c0fx.fits \
	to=$(OUTPUT)/fitsTruth.cub \
	> /dev/null;
	raw2isis from=$(INPUT)/WFPC2u5780205r_c0fx.fits \
	to=$(OUTPUT)/rawTruth.cub \
	SAMPLES=200 LINES=200 BANDS=4 SKIP=23040 \
	BITTYPE=REAL BYTEORDER=MSB \
	> /dev/null;
