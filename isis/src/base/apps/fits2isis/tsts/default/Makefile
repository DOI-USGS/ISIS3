APPNAME = fits2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/WFPC2u5780205r_c0fx.fits \
	  to=$(OUTPUT)/fitsTruth.cub > /dev/null;
	catlab from=$(OUTPUT)/fitsTruth.cub \
		to=$(OUTPUT)/fitsTruth.pvl > /dev/null;
