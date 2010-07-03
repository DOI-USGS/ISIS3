APPNAME = rotate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/rotateTruth1.cub \
	  degrees=90 \
	  interp=nearestneighbor > /dev/null;
