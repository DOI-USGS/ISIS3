APPNAME = map2map

labels.txt.IGNORELINES = Bytes ByteOrder
# This tolerance is so high because it's ObliqueCylindrical and
#   the interpolation causes pixel differences.
map2mapTruth.cub.TOLERANCE = 0.000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/ab102401.cub \
	MAP=$(INPUT)/oblique.map \
	TO=$(OUTPUT)/map2mapTruth.cub \
	> /dev/null;
	catlab FROM=$(OUTPUT)/map2mapTruth.cub >& $(OUTPUT)/labels.txt;
