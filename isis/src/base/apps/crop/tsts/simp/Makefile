APPNAME = crop

simp.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mapsimp.cub to=$(OUTPUT)/mapsimpcrop.cub \
	  sample=818 nsamples=300 sinc=1 line=2070 nlines=300 linc=1 > /dev/null;
	catlab from=$(OUTPUT)/mapsimpcrop.cub > $(OUTPUT)/simp.txt;
	rm $(OUTPUT)/mapsimpcrop.cub;
