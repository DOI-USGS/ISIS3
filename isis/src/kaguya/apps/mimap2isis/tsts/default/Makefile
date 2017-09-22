APPNAME = mimap2isis

FILE = MI_MAP_02_N65E328N64E329SC

Truthlabel.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/${FILE}.img \
	  to=$(OUTPUT)/${FILE}.cub > /dev/null;
	  catlab from=${OUTPUT}/${FILE}.cub to=$(OUTPUT)/Truthlabel.txt > /dev/null;
	  #mv ${OUTPUT}/labels.txt ${OUTPUT}/${FILE}Truth.pvl > /dev/null;
	  #catlab from=$(OUTPUT)/${FILE}.cub to=$(OUTPUT)/${FILE}Truth.pvl > /dev/null;
