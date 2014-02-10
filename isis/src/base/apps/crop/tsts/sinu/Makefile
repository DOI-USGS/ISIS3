APPNAME = crop

sinu.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/mapsinu.cub to=$(OUTPUT)/mapsinucrop.cub \
	  sample=818 nsamples=300 sinc=1 line=2070 nlines=300 linc=1 > /dev/null;
	catlab from=$(OUTPUT)/mapsinucrop.cub > $(OUTPUT)/sinu.txt;
	rm $(OUTPUT)/mapsinucrop.cub;
