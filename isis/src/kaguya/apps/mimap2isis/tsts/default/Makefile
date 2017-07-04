APPNAME = mimap2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MI_MAP_02_N65E328N64E329SC.img \
	  to=$(OUTPUT)/MI_MAP_02_N65E328N64E329SC.cub > /dev/null;
	  catlab from=$(OUTPUT)/MI_MAP_02_N65E328N64E329SC.cub to=$(OUTPUT)/labels.txt > /dev/null;
	  catlab from=$(OUTPUT)/MI_MAP_02_N65E328N64E329SC.cub \
	  to=$(OUTPUT)/MI_MAP_02_N65E328N64E329SCTruth.pvl > /dev/null;
