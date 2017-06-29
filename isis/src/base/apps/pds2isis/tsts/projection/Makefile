APPNAME = pds2isis

labels.txt.IGNORELINES = Bytes StartByte ByteOrder TileSamples TileLines

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ff17.lbl \
	  to=$(OUTPUT)/ff17Truth.cub \
	  | $(SED) 's+.*% Processed.*#+#+' \
	  | grep -v "Processed" \
	  | grep -v "Working" \
	  | grep -v "Importing" \
	  >& $(OUTPUT)/print.txt;
	  catlab from=$(OUTPUT)/ff17Truth.cub to=$(OUTPUT)/ff17Truth.pvl > /dev/null;
