# Tests using the lat/lon range found in the Mapping group in the input cube's
# labels.
APPNAME = maptrim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.map.cub \
	to=$(OUTPUT)/labelRangesTruth.cub > /dev/null;
