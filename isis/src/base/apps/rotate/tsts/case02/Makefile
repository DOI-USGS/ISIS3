APPNAME = rotate

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/rotateTruth2.cub \
	  degrees=112 \
	  interp=nearestneighbor > /dev/null;
