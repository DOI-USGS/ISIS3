APPNAME = kaguyatc2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/TC2W2B0_01_02735N583E3115.img \
	           to=$(OUTPUT)/TC2W2B0_01_02735N583E3115.cub > /dev/null;
	  catlab from=$(OUTPUT)/TC2W2B0_01_02735N583E3115.cub \
	         to=$(OUTPUT)/TC2W2B0_01_02735N583E3115.pvl > /dev/null;
