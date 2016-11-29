APPNAME = pds2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.img \
	  to=$(OUTPUT)/pds2isisTruth.cub > /dev/null;
	  catlab from=$(OUTPUT)/pds2isisTruth.cub to=$(OUTPUT)/pds2isisTruth.pvl > /dev/null;
