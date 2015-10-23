APPNAME = kaguyasp2ascii

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	#default run with new format file
	$(APPNAME) from=$(INPUT)/SP_2C_02_02509_S336_E1946.spc \
	  to=$(OUTPUT)/SP_2C_02_02509_S336_E1946.spc.txt > /dev/null;

	#run using min/maxobs 
	$(APPNAME) from=$(INPUT)/SP_2C_02_02509_S336_E1946.spc \
	  minobs=2 maxobs=5 to=$(OUTPUT)/SP_2C_02_02509_S336_E1946.spc.minmax.txt > /dev/null;
