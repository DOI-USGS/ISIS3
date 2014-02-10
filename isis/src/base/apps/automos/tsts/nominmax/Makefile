APPNAME = automos

mos.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/cube.lis mos=$(OUTPUT)/mos.cub > /dev/null;
	catlab from=$(OUTPUT)/mos.cub > $(OUTPUT)/mos.txt;
	rm $(OUTPUT)/mos.cub;
