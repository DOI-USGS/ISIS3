APPNAME = map2map

labels.txt.IGNORELINES = Bytes ByteOrder StartByte TileSamples TileLines
# This tolerance is so high because it's ObliqueCylindrical and
#   the interpolation causes pixel differences.
map2mapTruth.cub.TOLERANCE = 0.000001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/ab102401.cub \
	  MAP=$(INPUT)/oblique.map \
	  TO=$(OUTPUT)/map2mapTruth.cub \
	  > /dev/null;
	$(APPNAME) FROM=$(INPUT)/WAC_GLD100_V1.0_GLOBAL_with_LOLA_30M_POLE.10km.cub \
	  MAP=$(INPUT)/orthographic.map TO=$(OUTPUT)/orthoTruth.cub DEFAULTRANGE=MAP \
	  > /dev/null;
	catlab FROM=$(OUTPUT)/map2mapTruth.cub to=$(OUTPUT)/labels.txt > /dev/null;
