APPNAME = cam2map

cam2mapTruth.cub.TOLERANCE = .000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/cam2mapTruth.cub \
	  map=$(INPUT)/map.map \
	  pixres=mpp \
	  defaultrange=camera \
	  resolution=1000 > /dev/null;
