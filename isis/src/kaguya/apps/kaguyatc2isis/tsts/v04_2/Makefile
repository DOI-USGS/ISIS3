APPNAME = kaguyatc2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/TCO_MAPe04_N78E009N75E012SC.img \
	  to=$(OUTPUT)/tco_k2i_16bit_testing_012sc_spx.cub+16bit+-327.525:327.675 \
	  setnullrange=yes nullmin=0 nullmax=0 > /dev/null;
	  catlab from=$(OUTPUT)/tco_k2i_16bit_testing_012sc_spx.cub >& $(OUTPUT)/labels.txt;
