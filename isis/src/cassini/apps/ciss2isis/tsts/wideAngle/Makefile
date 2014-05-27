APPNAME =ciss2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/W1472855646_5.LBL \
	TO=$(OUTPUT)/W1472855646_5.truth.cub > /dev/null;
	catlab from=$(OUTPUT)/W1472855646_5.truth.cub to=$(OUTPUT)/labels.txt > /dev/null;
