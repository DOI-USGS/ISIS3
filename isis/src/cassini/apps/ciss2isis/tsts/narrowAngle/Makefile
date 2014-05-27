APPNAME = ciss2isis

labels.txt.IGNORELINES = Bytes StartByte TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/N1472853667_1.LBL \
	to=$(OUTPUT)/N1472853667_1.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/N1472853667_1.truth.cub to=$(OUTPUT)/labels.txt > /dev/null;
