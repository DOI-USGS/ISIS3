APPNAME = warp

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/testCube.cub \
	  to=$(OUTPUT)/warpTruth.cub \
	  control=$(INPUT)/testCnet.txt \
	  interp=nearestneighbor \
	  osize=user \
	  onl=200 ons=200 > /dev/null;
