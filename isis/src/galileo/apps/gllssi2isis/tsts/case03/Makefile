APPNAME = gllssi2isis

3700R.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/3700R.LBL \
	TO=$(OUTPUT)/3700R.cub > /dev/null;
	catlab FROM=$(OUTPUT)/3700R.cub to=$(OUTPUT)/3700R.txt > /dev/null;
