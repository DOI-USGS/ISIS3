APPNAME = cam2map

cam2mapTruth.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/cam2mapTruth.cub \
	  pixres=mpp \
	  defaultrange=camera \
	  resolution=1000 > /dev/null;
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/cam2mapCheckRange.cub \
	  defaultrange=camera \
	  MINLAT=15.0 MAXLAT=20.0 MINLON=225.0 MAXLON=230.0 \
	  pixres=mpp \
	  resolution=1000 > /dev/null;
