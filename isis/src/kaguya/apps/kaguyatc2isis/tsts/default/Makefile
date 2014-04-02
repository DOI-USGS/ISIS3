APPNAME = kaguyatc2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/TC_EVE_02_S09E000S12E003SC.img \
	  to=$(OUTPUT)/kaguyatcTruth.cub > /dev/null;
	  catlab from=$(OUTPUT)/kaguyatcTruth.cub to=$(OUTPUT)/labels.txt > /dev/null;
