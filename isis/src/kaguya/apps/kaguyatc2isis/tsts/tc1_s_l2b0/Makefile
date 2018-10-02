APPNAME = kaguyatc2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/TC1W2B0_01_00296N081E2387.img \
	           to=$(OUTPUT)/TC1W2B0_01_00296N081E2387.cub > /dev/null;
	  catlab from=$(OUTPUT)/TC1W2B0_01_00296N081E2387.cub \
	         to=$(OUTPUT)/TC1W2B0_01_00296N081E2387.pvl > /dev/null;
