APPNAME = map2cam

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102402.lev2.cub \
	match= $(INPUT)/ab102401.cub \
	to=$(OUTPUT)/map2camTruth2.cub \
	interp=nearestneighbor> /dev/null;
