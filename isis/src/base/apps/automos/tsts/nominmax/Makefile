APPNAME = automos

mos.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/inputlist.txt;                                           \
	$(APPNAME) fromlist=$(OUTPUT)/inputlist.txt mos=$(OUTPUT)/mos.cub > /dev/null;         \
	catlab from=$(OUTPUT)/mos.cub to=$(OUTPUT)/mos.txt > /dev/null;                                     \
	$(RM) $(OUTPUT)/mos.cub;                                                               \
	$(RM) $(OUTPUT)/inputlist.txt;
